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
#ifndef OPI_ERROR_H
#define OPI_ERROR_H
#include "opi_common.h"
#include "opi_datatypes.h"
#ifdef __cplusplus
extern "C" {
#endif

OPI_API_EXPORT const char* OPI_ErrorMessage(int code);
#ifdef __cplusplus
}
namespace OPI
{
	OPI_API_EXPORT const char* ErrorMessage(int code);
}
#endif


#endif
