/* OPI: Orbital Propagation Interface
 * Copyright (C) 2014 Institute of Aerospace Systems, TU Braunschweig, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include "opi_population.h"
#include "opi_host.h"
#include "opi_indexlist.h"
#include "opi_gpusupport.h"
#include "internal/opi_synchronized_data.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string.h> //memcpy
namespace OPI
{
	/**
	 * \cond INTERNAL_DOCUMENTATION
	 */

	// this holds all internal Population variables (pimpl)
	struct ObjectRawData
	{
			ObjectRawData(Host& _host):
				host(_host),
				data_orbit(host),
                data_properties(host),
				data_position(host),
				data_velocity(host),
                data_acceleration(host),
                data_covariance(host),
                data_bytes(host)
			{

			}

			Host& host;

			SynchronizedData<Orbit> data_orbit;
			SynchronizedData<ObjectProperties> data_properties;
			SynchronizedData<Vector3> data_position;
			SynchronizedData<Vector3> data_velocity;
            SynchronizedData<Vector3> data_acceleration;
            SynchronizedData<Covariance> data_covariance;
            SynchronizedData<char> data_bytes;

            // non-synchronized data
            std::vector<std::string> object_names;
            std::string lastPropagatorName;

			// data size
			int size;
            int byteArraySize;
	};
	/**
	 * \endcond
	 */

	Population::Population(Host& host, int size):
		data(host)
	{
		data->size = 0;
        data->byteArraySize = 1;
        data->lastPropagatorName = "None";
		resize(size);        
	}

    Population::Population(const Population& source) : data(source.getHostPointer())
    {
        data->size = 0;
        data->byteArraySize = 1;
        data->lastPropagatorName = source.getLastPropagatorName();
        int s = source.getSize();
        int b = source.getByteArraySize();
        resize(s);
        resizeByteArray(b);

        // TODO Use std::copy instead
        memcpy(getOrbit(), source.getOrbit(), s*sizeof(Orbit));
        memcpy(getObjectProperties(), source.getObjectProperties(), s*sizeof(ObjectProperties));
        memcpy(getPosition(), source.getPosition(), s*sizeof(Vector3));
        memcpy(getVelocity(), source.getVelocity(), s*sizeof(Vector3));
        memcpy(getAcceleration(), source.getAcceleration(), s*sizeof(Vector3));
        memcpy(getCovariance(), source.getCovariance(), s*sizeof(Covariance));
        memcpy(getBytes(), source.getBytes(), b*s*sizeof(char));

        update(DATA_ORBIT);
        update(DATA_PROPERTIES);
        update(DATA_POSITION);
        update(DATA_VELOCITY);
        update(DATA_ACCELERATION);
        update(DATA_COVARIANCE);
        update(DATA_BYTES);
    }

    Population::Population(const Population& source, IndexList &list) : data(source.getHostPointer())
    {
        data->size = 0;
        data->byteArraySize = 1;
        data->lastPropagatorName = source.getLastPropagatorName();
        int s = list.getSize();
        int b = source.getByteArraySize();
        resize(s);
        resizeByteArray(b);
        int* listdata = list.getData(DEVICE_HOST);

        Orbit* orbits = source.getOrbit(DEVICE_HOST, false);
        ObjectProperties* props = source.getObjectProperties(DEVICE_HOST, false);
        Vector3* pos = source.getPosition(DEVICE_HOST, false);
        Vector3* vel = source.getVelocity(DEVICE_HOST, false);
        Vector3* acc = source.getAcceleration(DEVICE_HOST, false);
        Covariance* cov = source.getCovariance(DEVICE_HOST, false);
        char* bytes = source.getBytes(DEVICE_HOST, false);

        Orbit* thisOrbit = getOrbit();
        ObjectProperties* thisProps = getObjectProperties();
        Vector3* thisPos = getPosition();
        Vector3* thisVel = getVelocity();
        Vector3* thisAcc = getAcceleration();
        Covariance* thisCov = getCovariance();
        char* thisBytes = getBytes();

        for(int i = 0; i < list.getSize(); ++i)
        {
            thisOrbit[i] = orbits[listdata[i]];
            thisProps[i] = props[listdata[i]];
            thisPos[i] = pos[listdata[i]];
            thisVel[i] = vel[listdata[i]];
            thisAcc[i] = acc[listdata[i]];
            thisCov[i] = cov[listdata[i]];
            for (int j=0; j<b; j++)
            {
                thisBytes[i*b+j] = bytes[listdata[i]*b+j];
            }
        }

        update(DATA_ORBIT);
        update(DATA_PROPERTIES);
        update(DATA_POSITION);
        update(DATA_VELOCITY);
        update(DATA_ACCELERATION);
        update(DATA_COVARIANCE);
        update(DATA_BYTES);
    }

	Population::~Population()
	{
    }

	/**
	 * \detail
	 * The File will contain a 32-bit integer containing the number of Objects.
	 * Following are the blocks of data consisting of:
	 * A 32-bit integer declaring the type of the block, followed by another 32-bit integer defining the size of one entry
	 * followed by entry_size * number_of_objects bytes containing the actual data
	 *
	 * This will not work between machines with different endianness!
	 */
	void Population::write(const std::string& filename)
	{
		int temp;
        int versionNumber = 1;
        int magic = 47627;
        int nameLength = data->lastPropagatorName.length();
		std::ofstream out(filename.c_str(), std::ofstream::binary);
		if(out.is_open())
		{                        
            out.write(reinterpret_cast<char*>(&magic), sizeof(int));
            out.write(reinterpret_cast<char*>(&versionNumber), sizeof(int));
			out.write(reinterpret_cast<char*>(&data->size), sizeof(int));
            out.write(reinterpret_cast<char*>(&nameLength), sizeof(int));
            out.write(reinterpret_cast<char*>(&data->lastPropagatorName), data->lastPropagatorName.length());
			if(data->data_orbit.hasData())
			{
				temp = DATA_ORBIT;
				out.write(reinterpret_cast<char*>(&temp), sizeof(int));
				temp = sizeof(Orbit);
				out.write(reinterpret_cast<char*>(&temp), sizeof(int));
				out.write(reinterpret_cast<char*>(getOrbit()), sizeof(Orbit) * data->size);
			}
			if(data->data_properties.hasData())
			{
				temp = DATA_PROPERTIES;
				out.write(reinterpret_cast<char*>(&temp), sizeof(int));
				temp = sizeof(ObjectProperties);
				out.write(reinterpret_cast<char*>(&temp), sizeof(int));
				out.write(reinterpret_cast<char*>(getObjectProperties()), sizeof(ObjectProperties) * data->size);
			}
            if(data->data_position.hasData())
            {
                temp = DATA_POSITION;
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                temp = sizeof(Vector3);
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                out.write(reinterpret_cast<char*>(getPosition()), sizeof(Vector3) * data->size);
            }
            if(data->data_velocity.hasData())
            {
                temp = DATA_VELOCITY;
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                temp = sizeof(Vector3);
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                out.write(reinterpret_cast<char*>(getVelocity()), sizeof(Vector3) * data->size);
            }
            if(data->data_acceleration.hasData())
            {
                temp = DATA_ACCELERATION;
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                temp = sizeof(Vector3);
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                out.write(reinterpret_cast<char*>(getAcceleration()), sizeof(Vector3) * data->size);
            }
            if(data->data_covariance.hasData())
            {
                temp = DATA_COVARIANCE;
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                temp = sizeof(Covariance);
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                out.write(reinterpret_cast<char*>(getCovariance()), sizeof(Covariance) * data->size);
            }
            if(data->data_bytes.hasData())
            {
                temp = DATA_BYTES;
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                temp = data->byteArraySize;
                out.write(reinterpret_cast<char*>(&temp), sizeof(int));
                out.write(reinterpret_cast<char*>(getBytes()), data->byteArraySize * data->size);
            }
		}
	}

	/**
	 * \detail
	 * See Population::write for more information
	 */
	ErrorCode Population::read(const std::string& filename)
	{
        int number_of_objects = 0;
        int magicNumber = 0;
        int versionNumber = 0;
        int propagatorNameLength = 0;
        char* propagatorName;

        std::ifstream in(filename.c_str(), std::ifstream::binary);
		if(in.is_open())
        {
            in.read(reinterpret_cast<char*>(&magicNumber), sizeof(int));
            if (magicNumber == 47627)
            {
                in.read(reinterpret_cast<char*>(&versionNumber), sizeof(int));
                if (versionNumber == 1)
                {
                    in.read(reinterpret_cast<char*>(&number_of_objects), sizeof(int));
                    resize(number_of_objects);
                    data->size = number_of_objects;
                    in.read(reinterpret_cast<char*>(&propagatorNameLength), sizeof(int));
                    in.read(reinterpret_cast<char*>(&propagatorName), propagatorNameLength*sizeof(char));
                    data->lastPropagatorName = std::string(propagatorName);
                    while(in.good())
                    {
                        int type;
                        int size;
                        in.read(reinterpret_cast<char*>(&type), sizeof(int));
                        if(!in.eof())
                        {
                            in.read(reinterpret_cast<char*>(&size), sizeof(int));
                            switch(type)
                            {
                            case DATA_ORBIT:
                                if(size == sizeof(Orbit))
                                {
                                    Orbit* orbit = getOrbit(DEVICE_HOST, true);
                                    in.read(reinterpret_cast<char*>(orbit), sizeof(Orbit) * number_of_objects);
                                    data->data_orbit.update(DEVICE_HOST);
                                    break;
                                }
                            case DATA_PROPERTIES:
                                if(size == sizeof(ObjectProperties))
                                {
                                    ObjectProperties* prop = getObjectProperties(DEVICE_HOST, true);
                                    in.read(reinterpret_cast<char*>(prop), sizeof(ObjectProperties) * number_of_objects);
                                    data->data_properties.update(DEVICE_HOST);
                                    break;
                                }
                            case DATA_POSITION:
                                if(size == sizeof(Vector3))
                                {
                                    Vector3* pos = getPosition(DEVICE_HOST, true);
                                    in.read(reinterpret_cast<char*>(pos), sizeof(Vector3) * number_of_objects);
                                    data->data_position.update(DEVICE_HOST);
                                    break;
                                }
                            case DATA_VELOCITY:
                                if(size == sizeof(Vector3))
                                {
                                    Vector3* vel = getVelocity(DEVICE_HOST, true);
                                    in.read(reinterpret_cast<char*>(vel), sizeof(Vector3) * number_of_objects);
                                    data->data_velocity.update(DEVICE_HOST);
                                    break;
                                }
                            case DATA_ACCELERATION:
                                if(size == sizeof(Vector3))
                                {
                                    Vector3* acc = getAcceleration(DEVICE_HOST, true);
                                    in.read(reinterpret_cast<char*>(acc), sizeof(Vector3) * number_of_objects);
                                    data->data_acceleration.update(DEVICE_HOST);
                                    break;
                                }
                            case DATA_COVARIANCE:
                                if(size == sizeof(Covariance))
                                {
                                    Covariance* cov = getCovariance(DEVICE_HOST, true);
                                    in.read(reinterpret_cast<char*>(cov), sizeof(Covariance) * number_of_objects);
                                    data->data_covariance.update(DEVICE_HOST);
                                    break;
                                }
                            case DATA_BYTES:
                                if(size == size) //TODO
                                {
                                    resizeByteArray(size);
                                    char* bytes = getBytes(DEVICE_HOST, true);
                                    in.read(bytes, size * number_of_objects * sizeof(char));
                                    data->data_bytes.update(DEVICE_HOST);
                                    break;
                                }
                            default:
                                std::cout << "Found unknown block id " << type << std::endl;
                                in.seekg(number_of_objects * size);
                            }
                        }
                    }
                }
                else std::cout << "Unknown file version" << std::endl;
            }
            else std::cout << filename << " does not appear to be an OPI population file." << std::endl;
		}
		return SUCCESS;
	}

    void Population::resize(int size, int byteArraySize)
	{
		if(data->size != size)
		{
			data->data_orbit.resize(size);
			data->data_properties.resize(size);
			data->data_position.resize(size);
			data->data_velocity.resize(size);
            data->data_acceleration.resize(size);
            data->data_covariance.resize(size);
            data->data_bytes.resize(size*byteArraySize);
            data->object_names.resize(size);
			data->size = size;
            data->byteArraySize = byteArraySize;
		}
	}

    void Population::resizeByteArray(int size)
    {
        data->data_bytes.resize(data->size * size);
        data->byteArraySize = size;
    }

    std::string Population::getLastPropagatorName() const
    {
        return data->lastPropagatorName;
    }

    void Population::setLastPropagatorName(std::string propagatorName)
    {
        data->lastPropagatorName = propagatorName;
    }

    std::string Population::getObjectName(int index)
    {
        if (index < data->size)
            return data->object_names[index];
        else return "";
    }

    void Population::setObjectName(int index, std::string name)
    {
        if (index < data->size)
        {
            data->object_names[index] = name;
        }
        else std::cout << "Cannot set object name: Index (" << index << ") out of range!" << std::endl;
    }


	/**
	 * @details
	 * If no_sync is set to false, a synchronization is performed to ensure the latest up-to-date data on the
	 * requested device.
	 */
	Orbit* Population::getOrbit(Device device, bool no_sync) const
	{
		return data->data_orbit.getData(device, no_sync);
	}

	/**
	 * @details
	 * If no_sync is set to false, a synchronization is performed to ensure the latest up-to-date data on the
	 * requested device.
	 */
	ObjectProperties* Population::getObjectProperties(Device device, bool no_sync) const
	{
		return data->data_properties.getData(device, no_sync);
	}

	/**
	 * @details
	 * If no_sync is set to false, a synchronization is performed to ensure the latest up-to-date data on the
	 * requested device.
	 */
    Vector3* Population::getPosition(Device device, bool no_sync) const
	{
		return data->data_position.getData(device, no_sync);
	}

	/**
	 * @details
	 * If no_sync is set to false, a synchronization is performed to ensure the latest up-to-date data on the
	 * requested device.
	 */
	Vector3* Population::getVelocity(Device device, bool no_sync) const
	{
		return data->data_velocity.getData(device, no_sync);
	}

	/**
	 * @details
	 * If no_sync is set to false, a synchronization is performed to ensure the latest up-to-date data on the
	 * requested device.
	 */
	Vector3* Population::getAcceleration(Device device, bool no_sync) const
	{
		return data->data_acceleration.getData(device, no_sync);
    }

    /**
     * @details
     * If no_sync is set to false, a synchronization is performed to ensure the latest up-to-date data on the
     * requested device.
     */
    Covariance* Population::getCovariance(Device device, bool no_sync) const
    {
        return data->data_covariance.getData(device, no_sync);
    }

    char* Population::getBytes(Device device, bool no_sync) const
    {
        return data->data_bytes.getData(device, no_sync);
    }

	void Population::remove(IndexList &list)
	{
		list.sort();
		int* listdata = list.getData(DEVICE_HOST);
		int offset = 0;
		for(int i = 0; i < list.getSize(); ++i)
		{
			remove(listdata[i] - offset);
			offset++;
		}
	}

    void Population::insert(Population& source, IndexList& list)
    {
        int* listdata = list.getData(DEVICE_HOST);

        Orbit* orbits = source.getOrbit(DEVICE_HOST, false);
        ObjectProperties* props = source.getObjectProperties(DEVICE_HOST, false);
        Vector3* pos = source.getPosition(DEVICE_HOST, false);
        Vector3* vel = source.getVelocity(DEVICE_HOST, false);
        Vector3* acc = source.getAcceleration(DEVICE_HOST, false);
        Covariance* cov = source.getCovariance(DEVICE_HOST, false);
        char* bytes = source.getBytes(DEVICE_HOST, false);

        Orbit* thisOrbit = getOrbit();
        ObjectProperties* thisProps = getObjectProperties();
        Vector3* thisPos = getPosition();
        Vector3* thisVel = getVelocity();
        Vector3* thisAcc = getAcceleration();
        Covariance* thisCov = getCovariance();
        char* thisBytes = getBytes();

        if (getByteArraySize() != source.getByteArraySize())
        {
            std::cout << "Warning: Cannot insert byte array into population!" << std::endl;
        }

        if (list.getSize() >= source.getSize())
        {
            for(int i = 0; i < list.getSize(); ++i)
            {
                int l = listdata[i];
                if (l < getSize())
                {
                    thisOrbit[l] = orbits[i];
                    thisProps[l] = props[i];
                    thisPos[l] = pos[i];
                    thisVel[l] = vel[i];
                    thisAcc[l] = acc[i];
                    thisCov[l] = cov[i];
                    if (getByteArraySize() == source.getByteArraySize())
                    {
                        int b = getByteArraySize();
                        for (int j=0; j<b; j++)
                        {
                            thisBytes[l*b+j] = bytes[i*b+j];
                        }
                    }
                }
                else {
                    std::cout << "Cannot insert - index out of range: " << l << std::endl;
                }
            }
        }
        else {
            std::cout << "Cannot insert - not enough elements in index list!" << std::endl;
        }

        update(DATA_ORBIT);
        update(DATA_PROPERTIES);
        update(DATA_POSITION);
        update(DATA_VELOCITY);
        update(DATA_ACCELERATION);
        update(DATA_COVARIANCE);
        update(DATA_BYTES);
    }

	void Population::remove(int index)
	{
		data->data_acceleration.remove(index);
		data->data_orbit.remove(index);
		data->data_position.remove(index);
		data->data_properties.remove(index);
        data->data_velocity.remove(index);
        data->data_covariance.remove(index);
        data->data_bytes.remove(index*data->byteArraySize, data->byteArraySize);
		data->size--;
	}

	ErrorCode Population::update(int type, Device device)
	{
		ErrorCode status = SUCCESS;
		switch(type)
		{
			case DATA_ORBIT:
				data->data_orbit.update(device);
				break;
			case DATA_PROPERTIES:
				data->data_properties.update(device);
				break;
			case DATA_VELOCITY:
				data->data_velocity.update(device);
				break;
            case DATA_POSITION:
				data->data_position.update(device);
				break;
			case DATA_ACCELERATION:
				data->data_acceleration.update(device);
                break;
            case DATA_COVARIANCE:
                data->data_covariance.update(device);
                break;
            case DATA_BYTES:
                data->data_bytes.update(device);
                break;
			default:
				status = INVALID_TYPE;
		}
		data->host.sendError(status);
		return status;
	}

	int Population::getSize() const
	{
		return data->size;
	}

    int Population::getByteArraySize() const
    {
        return data->byteArraySize;
    }

    Host& Population::getHostPointer() const
    {
        return data->host;
    }

	std::string Population::sanityCheck()
	{
		if (getSize()==0) return std::string("Population is empty.");

		// This function auto-syncs to host so it might be slow
		Orbit* orbit = getOrbit(DEVICE_HOST);
		ObjectProperties* props = getObjectProperties(DEVICE_HOST);

		std::stringstream result;
		result.str("");
        const double twopi = 6.2831853;
		for (int i=0; i<getSize(); i++) {

			// SMA is smaller than Earth's radius but object has not been marked as decayed
			if (orbit[i].semi_major_axis < 6378.0f && orbit[i].eol <= 0.0f) {
				result << "Object " << i << " (ID " << props[i].id << "): Unmarked deorbit: ";
				result << "SMA: " << orbit[i].semi_major_axis << ", EOL: " << orbit[i].eol << "\n";
			}
			// Eccentricity is zero or smaller than zero
			if (orbit[i].eccentricity <= 0.0f) {
				result << "Object " << i << "(" << props[i].id << "): Eccentricity below zero: ";
				result << orbit[i].eccentricity << "\n";
			}
			// Eccentricity is larger than one. This might occur when decayed objects are propagated
			// further so only issue a warning if the object has not been marked as decayed.
			// For some use cases hyperbolic orbits might actually be valid so this value might have
			// to be adjusted. One possibility would be to calculate the delta-V required to achieve
			// the given eccentricity and issue a warning when unrealistic speeds occur.
			if (orbit[i].eccentricity > 1.0f && orbit[i].eol <= 0.0f) {
				result << "Object " << i << "(" << props[i].id << "): Eccentricity larger than one: ";
				result << orbit[i].eccentricity << "\n";
			}
			// Angles are outside of radian range (possibly given in degrees)
			if (orbit[i].inclination < -twopi || orbit[i].inclination > twopi) {
				result << "Object " << i << "(" << props[i].id << "): Inclination not in radian range: ";
				result << orbit[i].inclination << "\n";
			}
			if (orbit[i].raan < -twopi || orbit[i].raan > twopi) {
				result << "Object " << i << "(" << props[i].id << "): RAAN not in radian range: ";
				result << orbit[i].raan << "\n";
			}
			if (orbit[i].arg_of_perigee < -twopi || orbit[i].arg_of_perigee > twopi) {
				result << "Object " << i << "(" << props[i].id << "): Arg. of perigee not in radian range: ";
				result << orbit[i].arg_of_perigee << "\n";
			}
			if (orbit[i].mean_anomaly < -twopi || orbit[i].mean_anomaly > twopi) {
				result << "Object " << i << "(" << props[i].id << "): Mean anomaly not in radian range: ";
				result << orbit[i].mean_anomaly << "\n";
			}
			// Drag and reflectivity coefficients are not set (which may lead to early decays or division by zero)
			if (props[i].drag_coefficient <= 0.0f) {
				result << "Object " << i << "(" << props[i].id << "): Invalid drag coefficient: ";
				result << props[i].drag_coefficient << "\n";
			}
			if (props[i].reflectivity <= 0.0f) {
				result << "Object " << i << "(" << props[i].id << "): Invalid reflectivity coefficient: ";
				result << props[i].reflectivity << "\n";
			}
			//any number is NaN
			//unrealistic A2m ratio (possible mixup with m2a)
		}
		return result.str();
	}

    // adapted from SGP4 reference implementation by D. Vallado e.a.
    // https://celestrak.com/publications/AIAA/2006-6753/
    ErrorCode Population::convertOrbitsToStateVectors()
    {
        if (data->data_orbit.hasData())
        {
            for (int i=0; i<getSize(); i++)
            {
                Orbit o = getOrbit()[i];

                double ta = trueAnomaly(o);
                double argp = o.arg_of_perigee;
                const double arglat = argp + ta;
                double raan = o.raan;
                const double small = 1e-15;

                if (o.eccentricity < small)
                {
                    // circular equatorial
                    if ((o.inclination < small) || (fabs(o.inclination-M_PI) < small))
                    {
                        argp = 0.0;
                        raan = 0.0;
                        ta = o.raan + arglat;
                    }
                    else {
                        // circular inclined
                        argp = 0.0;
                        ta = arglat;
                    }
                }
                else {
                    // elliptical equatorial
                    if ((o.inclination < small) || (fabs(o.inclination-M_PI) < small))
                    {
                        argp = o.raan + o.arg_of_perigee;
                        raan = 0.0;
                    }
                }

                // ----------  form pqw position and velocity vectors ----------
                double p = o.semi_major_axis * (1.0 - pow(o.eccentricity, 2.0));
                const double mu = 398600.4418;
                const double temp = p / (1.0 + o.eccentricity * cos(ta));
                Vector3 r = {temp*cos(ta), temp*sin(ta), 0.0};
                if (fabs(p) < 1e-8 ) p = 1e-8;

                Vector3 v = {
                    -sin(ta) * sqrt(mu/p),
                    (o.eccentricity + cos(ta)) * sqrt(mu/p),
                    0.0
                };

                r = rotateZ(r,-argp);
                r = rotateX(r,-o.inclination);
                r = rotateZ(r,-raan);
                getPosition()[i] = r;

                v = rotateZ(v,-argp);
                v = rotateX(v,-o.inclination);
                v = rotateZ(v,-raan);
                getVelocity()[i] = v;
            }

            update(DATA_POSITION);
            update(DATA_VELOCITY);
            return SUCCESS;
        }
        else return INVALID_DATA;
    }

    // adapted from SGP4 reference implementation by D. Vallado e.a.
    // https://celestrak.com/publications/AIAA/2006-6753/
    ErrorCode Population::convertStateVectorsToOrbits()
    {
        ErrorCode e = SUCCESS;
        if (data->data_position.hasData() && data->data_velocity.hasData())
        {
            enum typeorbit_t {
                CIRCULAR_EQUATORIAL,
                CIRCULAR_INCLINED,
                ELLIPTICAL_EQUATORIAL,
                ELLIPTICAL_INCLINED
            } typeorbit;

            const double twopi = 2.0 * M_PI;
            const double halfpi = 0.5 * M_PI;
            const double small = 1e-15;
            const double mu = 398600.4418;
            const double nan = std::numeric_limits<double>::quiet_NaN();

            for (int i=0; i<getSize(); i++)
            {
                Vector3 r = getPosition()[i];
                Vector3 v = getVelocity()[i];
                Orbit o;

                const double magr = length(r);
                const double magv = length(v);

                Vector3 hbar = cross(r,v);
                const double magh = length(hbar);

                if (magh > small)
                {
                    // ------------------  find h n and e vectors   ----------------
                    Vector3 nbar = {-hbar.y, hbar.x, 0.0 };
                    const double magn = length(nbar);
                    const double c1 = magv*magv - mu/magr;
                    const double rdotv = r * v;
                    Vector3 ebar = (r*c1 - v*rdotv) / mu;
                    o.eccentricity = length(ebar);

                    // ------------  find a e and semi-latus rectum   ----------
                    const double sme = (magv*magv*0.5) - (mu / magr);
                    if (fabs(sme) > small)
                    {
                        o.semi_major_axis = -mu / (2.0 *sme);
                    }
                    else {
                        o.semi_major_axis = std::numeric_limits<double>::infinity();
                        e = INVALID_DATA;
                    }
                    //const double p = magh*magh / mu;

                    // -----------------  find inclination   -------------------
                    const double hk = hbar.z / magh;
                    o.inclination = acos(hk);

                    // --------  determine type of orbit for later use  --------
                    // ------ elliptical, parabolic, hyperbolic inclined -------
                    typeorbit = ELLIPTICAL_INCLINED;
                    if (o.eccentricity < small)
                    {
                        // ----------------  circular equatorial ---------------
                        if ((o.inclination < small) || (fabs(o.inclination - M_PI) < small))
                            typeorbit = CIRCULAR_EQUATORIAL;
                        else
                            // --------------  circular inclined ---------------
                            typeorbit = CIRCULAR_INCLINED;
                    }
                    else {
                        // - elliptical, parabolic, hyperbolic equatorial --
                        if ((o.inclination < small) || (fabs(o.inclination - M_PI) < small))
                            typeorbit = ELLIPTICAL_EQUATORIAL;
                    }

                    // ----------  find longitude of ascending node ------------
                    if (magn > small)
                    {
                        double temp = nbar.x / magn;
                        if (temp > 1.0) temp = 1.0;
                        else if (temp < -1.0) temp = -1.0;
                        o.raan = acos(temp);
                        if (nbar.y < 0.0) o.raan = twopi - o.raan;
                    }
                    else {
                        o.raan = nan;
                        e = INVALID_DATA;
                    }

                    // ---------------- find argument of perigee ---------------
                    if (typeorbit == ELLIPTICAL_INCLINED)
                    {
                        o.arg_of_perigee = angle(nbar,ebar);
                        if (ebar.z < 0.0) o.arg_of_perigee = twopi - o.arg_of_perigee;
                    }
                    else {
                        o.arg_of_perigee = nan;
                        e = INVALID_DATA;
                    }

                    // ------------  find true anomaly at epoch    -------------
                    double nu = nan;
                    if (typeorbit == ELLIPTICAL_EQUATORIAL || typeorbit == ELLIPTICAL_INCLINED)
                    {
                        nu = angle(ebar,r);
                        if (rdotv < 0.0) nu = twopi - nu;
                    }

                    // ----  find argument of latitude - circular inclined -----
                    double arglat = nan;
                    if (typeorbit == CIRCULAR_INCLINED)
                    {
                        arglat = angle(nbar,r);
                        if (r.z < 0.0) arglat = twopi - arglat;
                        o.mean_anomaly = arglat;
                    }

                    // -- find longitude of perigee - elliptical equatorial ----
                    double lonper = nan;
                    if ((o.eccentricity > small) && (typeorbit == ELLIPTICAL_EQUATORIAL))
                    {
                        double temp = ebar.x / o.eccentricity;
                        if (temp > 1.0) temp = 1.0;
                        else if (temp < -1.0) temp = -1.0;
                        lonper = acos(temp);
                        if (ebar.y < 0.0) lonper = twopi - lonper;
                        if (o.inclination > halfpi) lonper = twopi - lonper;
                    }

                    // -------- find true longitude - circular equatorial ------
                    double truelon = nan;
                    if ((magr>small) && (typeorbit == CIRCULAR_EQUATORIAL))
                    {
                        double temp = r.x / magr;
                        if (temp > 1.0) temp = 1.0;
                        else if (temp < -1.0) temp = -1.0;
                        truelon = acos(temp);
                        if (r.y < 0.0) truelon = twopi - truelon;
                        if (o.inclination > halfpi) truelon = twopi - truelon;
                        o.mean_anomaly = truelon;
                    }

                    // ------------ find mean anomaly for all orbits -----------
                    if (typeorbit == ELLIPTICAL_EQUATORIAL || typeorbit == ELLIPTICAL_INCLINED)
                    {
                        // get mean anomaly from true anomaly
                        const double ecc = o.eccentricity;
                        double ea = std::numeric_limits<double>::infinity();
                        double ma = std::numeric_limits<double>::infinity();
                        double small = 1e-15;

                        if (fabs(ecc) < small)
                        {
                            // circular
                            ea = nu;
                            ma = nu;
                        }
                        else if (ecc < 1.0 - small)
                        {
                            // elliptical
                            const double sine = (sqrt(1.0 - pow(ecc,2.0)) * sin(nu)) / (1.0 + ecc*cos(nu));
                            const double cose = (ecc + cos(nu)) / (1.0 + ecc*cos(nu));
                            ea = atan2(sine, cose);
                            ma = ea - ecc*sin(ea);
                        }
                        else if (ecc > 1.0 + small)
                        {
                            // hyperbolic
                            if ((ecc > 1.0) && (fabs(nu) + 0.00001 < M_PI - acos(1.0 / ecc)))
                            {
                                const double sine = (sqrt(pow(ecc, 2.0) - 1.0) * sin(nu)) / (1.0 + ecc*cos(nu));
                                ea = asinh(sine);
                                ma = ecc*sinh(ea) - ea;
                            }
                        }
                        else if (fabs(nu) < 168.0*M_PI / 180.0)
                        {
                            // parabolic
                            ea = tan(nu*0.5);
                            ma = ea + pow(ea,3.0)/3.0;
                        }

                        if (ecc < 1.0)
                        {
                            ma = fmod(ma, 2.0 * M_PI);
                            if (ma < 0.0) ma += 2.0*M_PI;
                        }

                        o.mean_anomaly = ma;
                    }
                }
                else {
                    o.semi_major_axis = nan;
                    o.eccentricity = nan;
                    o.inclination = nan;
                    o.raan = nan;
                    o.arg_of_perigee = nan;
                    o.mean_anomaly = nan;
                    e = INVALID_DATA;
                }
                o.bol = 0.0;
                o.eol = 0.0;
                getOrbit()[i] = o;
            }
            update(DATA_ORBIT);
            return e;
        }
        else return INVALID_DATA;
    }

}
