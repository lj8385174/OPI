
COMMENT("This type contains data required to describe a Kepler-Orbit")
BEGIN_STRUCTURE( Orbit )
  COMMENT( "Semi major axis")
  STRUCTURE_VARIABLE(double semi_major_axis)
  COMMENT( "Eccentricity")
  STRUCTURE_VARIABLE(double eccentricity)
  COMMENT( "Inclination")
  STRUCTURE_VARIABLE(double inclination)
  COMMENT( "Rectascension of ascending node")
  STRUCTURE_VARIABLE(double raan)
  COMMENT( "Argument of perigee")
  STRUCTURE_VARIABLE(double arg_of_perigee)
  COMMENT( "Mean anomaly")
  STRUCTURE_VARIABLE(double mean_anomaly)
  COMMENT( "Beginning of life" )
  STRUCTURE_VARIABLE(double bol)
  COMMENT( "End of life" )
  STRUCTURE_VARIABLE(double eol)
END_STRUCTURE( Orbit )


COMMENT("This type contains the objects additional properties")
BEGIN_STRUCTURE( ObjectProperties )
  COMMENT("The objects mass in [kg]")
  STRUCTURE_VARIABLE(double mass)
  COMMENT("The objects diameter in [m]")
  STRUCTURE_VARIABLE(double diameter)
  COMMENT("The objects area to mass ratio [m²/kg]")
  STRUCTURE_VARIABLE(double area_to_mass)
  COMMENT("The objects drag coefficient")
  STRUCTURE_VARIABLE(double drag_coefficient)
  COMMENT("The objects reflectivity coefficient")
  STRUCTURE_VARIABLE(double reflectivity)
  COMMENT("the object's id")
  STRUCTURE_VARIABLE(int id)
END_STRUCTURE( ObjectProperties )

COMMENT("This type represents a 3-dimensional vector")
BEGIN_STRUCTURE( Vector3 )
  COMMENT("x position")
  STRUCTURE_VARIABLE(double x)
  COMMENT("y position")
  STRUCTURE_VARIABLE(double y)
  COMMENT("z position")
  STRUCTURE_VARIABLE(double z)
END_STRUCTURE( Vector3 )

COMMENT("This type represents a covariance matrix")
BEGIN_STRUCTURE( Covariance )
  COMMENT("1st kinematic x 1st kinematic")
  STRUCTURE_VARIABLE(double k1_k1)
  COMMENT("2nd kinematic x 1st kinematic")
  STRUCTURE_VARIABLE(double k2_k1)
  COMMENT("2nd kinematic x 2nd kinematic")
  STRUCTURE_VARIABLE(double k2_k2)
  COMMENT("3rd kinematic x 1st kinematic")
  STRUCTURE_VARIABLE(double k3_k1)
  COMMENT("3rd kinematic x 2nd kinematic")
  STRUCTURE_VARIABLE(double k3_k2)
  COMMENT("3rd kinematic x 3rd kinematic")
  STRUCTURE_VARIABLE(double k3_k3)
  COMMENT("4th kinematic x 1st kinematic")
  STRUCTURE_VARIABLE(double k4_k1)
  COMMENT("4th kinematic x 2nd kinematic")
  STRUCTURE_VARIABLE(double k4_k2)
  COMMENT("4th kinematic x 3rd kinematic")
  STRUCTURE_VARIABLE(double k4_k3)
  COMMENT("4th kinematic x 4th kinematic")
  STRUCTURE_VARIABLE(double k4_k4)
  COMMENT("5th kinematic x 1st kinematic")
  STRUCTURE_VARIABLE(double k5_k1)
  COMMENT("5th kinematic x 2nd kinematic")
  STRUCTURE_VARIABLE(double k5_k2)
  COMMENT("5th kinematic x 3rd kinematic")
  STRUCTURE_VARIABLE(double k5_k3)
  COMMENT("5th kinematic x 4th kinematic")
  STRUCTURE_VARIABLE(double k5_k4)
  COMMENT("5th kinematic x 5th kinematic")
  STRUCTURE_VARIABLE(double k5_k5)
  COMMENT("6th kinematic x 1st kinematic")
  STRUCTURE_VARIABLE(double k6_k1)
  COMMENT("6th kinematic x 2nd kinematic")
  STRUCTURE_VARIABLE(double k6_k2)
  COMMENT("6th kinematic x 3rd kinematic")
  STRUCTURE_VARIABLE(double k6_k3)
  COMMENT("6th kinematic x 4th kinematic")
  STRUCTURE_VARIABLE(double k6_k4)
  COMMENT("6th kinematic x 5th kinematic")
  STRUCTURE_VARIABLE(double k6_k5)
  COMMENT("6th kinematic x 6th kinematic")
  STRUCTURE_VARIABLE(double k6_k6)
  COMMENT("1st dynamic x 1st kinematic")
  STRUCTURE_VARIABLE(double d1_k1)
  COMMENT("1st dynamic x 2nd kinematic")
  STRUCTURE_VARIABLE(double d1_k2)
  COMMENT("1st dynamic x 3rd kinematic")
  STRUCTURE_VARIABLE(double d1_k3)
  COMMENT("1st dynamic x 4th kinematic")
  STRUCTURE_VARIABLE(double d1_k4)
  COMMENT("1st dynamic x 5th kinematic")
  STRUCTURE_VARIABLE(double d1_k5)
  COMMENT("1st dynamic x 6th kinematic")
  STRUCTURE_VARIABLE(double d1_k6)
  COMMENT("1st dynamic x 1st dynamic")
  STRUCTURE_VARIABLE(double d1_d1)
  COMMENT("2nd dynamic x 1st kinematic")
  STRUCTURE_VARIABLE(double d2_k1)
  COMMENT("2nd dynamic x 2nd kinematic")
  STRUCTURE_VARIABLE(double d2_k2)
  COMMENT("2nd dynamic x 3rd kinematic")
  STRUCTURE_VARIABLE(double d2_k3)
  COMMENT("2nd dynamic x 4th kinematic")
  STRUCTURE_VARIABLE(double d2_k4)
  COMMENT("2nd dynamic x 5th kinematic")
  STRUCTURE_VARIABLE(double d2_k5)
  COMMENT("2nd dynamic x 6th kinematic")
  STRUCTURE_VARIABLE(double d2_k6)
  COMMENT("2nd dynamic x 1st dynamic")
  STRUCTURE_VARIABLE(double d2_d1)
  COMMENT("2nd dynamic x 2nd dynamic")
  STRUCTURE_VARIABLE(double d2_d2)
END_STRUCTURE( Covariance )

COMMENT("This type represents the V matrix for perturbations")
BEGIN_STRUCTURE( VMatrix )
  COMMENT("dAccelerationX / dPosX")
  STRUCTURE_VARIABLE(double accX_posX)
  COMMENT("dAccelerationY / dPosX")
  STRUCTURE_VARIABLE(double accY_posX)
  COMMENT("dAccelerationZ / dPosX")
  STRUCTURE_VARIABLE(double accZ_posX)
  COMMENT("dAccelerationX / dPosY")
  STRUCTURE_VARIABLE(double accX_posY)
  COMMENT("dAccelerationY / dPosY")
  STRUCTURE_VARIABLE(double accY_posY)
  COMMENT("dAccelerationZ / dPosY")
  STRUCTURE_VARIABLE(double accZ_posY)
  COMMENT("dAccelerationX / dPosZ")
  STRUCTURE_VARIABLE(double accX_posZ)
  COMMENT("dAccelerationY / dPosZ")
  STRUCTURE_VARIABLE(double accY_posZ)
  COMMENT("dAccelerationZ / dPosZ")
  STRUCTURE_VARIABLE(double accZ_posZ)
  COMMENT("dAccelerationX / dVelX")
  STRUCTURE_VARIABLE(double accX_velX)
  COMMENT("dAccelerationY / dVelX")
  STRUCTURE_VARIABLE(double accY_velX)
  COMMENT("dAccelerationZ / dVelX")
  STRUCTURE_VARIABLE(double accZ_velX)
  COMMENT("dAccelerationX / dVelY")
  STRUCTURE_VARIABLE(double accX_velY)
  COMMENT("dAccelerationY / dVelY")
  STRUCTURE_VARIABLE(double accY_velY)
  COMMENT("dAccelerationZ / dVelY")
  STRUCTURE_VARIABLE(double accZ_velY)
  COMMENT("dAccelerationX / dVelZ")
  STRUCTURE_VARIABLE(double accX_velZ)
  COMMENT("dAccelerationY / dVelZ")
  STRUCTURE_VARIABLE(double accY_velZ)
  COMMENT("dAccelerationZ / dVelZ")
  STRUCTURE_VARIABLE(double accZ_velZ)
  COMMENT("dAccelerationX / dKinematic1")
  STRUCTURE_VARIABLE(double accX_k1)
  COMMENT("dAccelerationY / dKinematic1")
  STRUCTURE_VARIABLE(double accY_k1)
  COMMENT("dAccelerationZ / dKinematic1")
  STRUCTURE_VARIABLE(double accZ_k1)
  COMMENT("dAccelerationX / dKinematic2")
  STRUCTURE_VARIABLE(double accX_k2)
  COMMENT("dAccelerationY / dKinematic2")
  STRUCTURE_VARIABLE(double accY_k2)
  COMMENT("dAccelerationZ / dKinematic2")
  STRUCTURE_VARIABLE(double accZ_k2)
END_STRUCTURE( VMatrix )

COMMENT("This type represents a pair of two object indices")
BEGIN_STRUCTURE( IndexPair )
  COMMENT("the first object")
  STRUCTURE_VARIABLE(int object1)
  COMMENT("the second object")
  STRUCTURE_VARIABLE(int object2)
END_STRUCTURE( IndexPair )

COMMENT("This type contains all available object data values")
BEGIN_ENUM(DataType)
  ENUM_VALUE(DATA_ORBIT 0)
  ENUM_VALUE(DATA_PROPERTIES 1)
  ENUM_VALUE(DATA_POSITION 2)
  ENUM_VALUE(DATA_VELOCITY 3)
  ENUM_VALUE(DATA_ACCELERATION 4)
  ENUM_VALUE(DATA_COVARIANCE 5)
  ENUM_VALUE(DATA_BYTES 6)
  ENUM_VALUE(DATA_VMATRIX 7)
END_ENUM(DataType)

COMMENT("This type contains all available device types")
BEGIN_ENUM_AS_INT(Device)
  ENUM_VALUE(DEVICE_NOT_SET -1)
  ENUM_VALUE(DEVICE_HOST 0)
  ENUM_VALUE(DEVICE_CUDA 1)
  ENUM_VALUE(DEVICE_CUDA_LAST 100)
END_ENUM(Device)


COMMENT("This type contains all known object property types")
BEGIN_ENUM(PropertyType)
  ENUM_VALUE(TYPE_UNKNOWN 0)
# basic types
  ENUM_VALUE(TYPE_INTEGER 1)
  ENUM_VALUE(TYPE_FLOAT 2)
  ENUM_VALUE(TYPE_DOUBLE 3)
  ENUM_VALUE(TYPE_STRING 4)

# array types
  ENUM_VALUE(TYPE_INTEGER_ARRAY 11)
  ENUM_VALUE(TYPE_FLOAT_ARRAY 12)
  ENUM_VALUE(TYPE_DOUBLE_ARRAY 13)
END_ENUM(PropertyType)

COMMENT("This type contains a list of reference frames for cartesian coordinates")
BEGIN_ENUM(ReferenceFrame)
  ENUM_VALUE(REF_NONE -1)
  ENUM_VALUE(REF_UNSPECIFIED 0)
  ENUM_VALUE(REF_TEME 1)
  ENUM_VALUE(REF_GCRF 2)
  ENUM_VALUE(REF_ITRF 3)
  ENUM_VALUE(REF_ECI 4)
  ENUM_VALUE(REF_ECEF 5)
  ENUM_VALUE(REF_MOD 6)
  ENUM_VALUE(REF_TOD 7)
  ENUM_VALUE(REF_J2000 8)
  ENUM_VALUE(REF_MULTIPLE 99)
  ENUM_VALUE(REF_UNLISTED 100)
END_ENUM(ReferenceFrame)

COMMENT("This type is used to specify the setup of the covariance matrix")
BEGIN_ENUM(CovarianceType)
  COMMENT("The propagator doesn't support covariance matrices.")
  ENUM_VALUE(CV_NONE -1)
  COMMENT("The kinematic parameters are state vectors.")
  ENUM_VALUE(CV_STATE_VECTORS 0)
  COMMENT("The kinematic parameters are state vectors, the dynamic parameters are unused.")
  ENUM_VALUE(CV_STATE_VECTORS_NO_DYNAMICS 1)
  COMMENT("The kinematic parameters are keplerian elements.")
  ENUM_VALUE(CV_KEPLERIAN 2)
  COMMENT("The kinematic parameters are keplerian elements, the dynamic parameters are unused.")
  ENUM_VALUE(CV_KEPLERIAN_NO_DYNAMICS 3)
  COMMENT("The kinematic parameters are equinoctial elements.")
  ENUM_VALUE(CV_EQUINOCTIALS 4)
  COMMENT("The kinematic parameters are equinoctial elements, the dynamic parameters are unused.")
  ENUM_VALUE(CV_EQUINOCTIALS_NO_DYNAMICS 5)
END_ENUM(CovarianceType)

COMMENT("This type contains all error values")
BEGIN_ENUM(ErrorCode)
  ENUM_VALUE(SUCCESS 0)
  ENUM_VALUE(UNKNOWN_ERROR 1)
  ENUM_VALUE(INVALID_ARGUMENT 2)
  ENUM_VALUE(UNKNOWN_VARIABLE 3)
  ENUM_VALUE(INDEX_RANGE 4)
  ENUM_VALUE(INVALID_TYPE 5)
  ENUM_VALUE(DIRECTORY_NOT_FOUND 6)
  ENUM_VALUE(INVALID_DEVICE 7)
  ENUM_VALUE(INVALID_PROPERTY 8)
  ENUM_VALUE(INCOMPATIBLE_TYPES 9)
  ENUM_VALUE(NOT_IMPLEMENTED 50)
  ENUM_VALUE(CUDA_REQUIRED 100)
  ENUM_VALUE(CUDA_OLDVERSION 101)
END_ENUM(ErrorCode)
