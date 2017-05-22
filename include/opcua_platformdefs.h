/* © Copyright Piotr Nikiel, CERN, 2015.  All rights not expressly granted are reserved.
 *
 *  Created on: 15 Nov,  2015
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *		mludwig@cern.ch : added vectors
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdint.h>
#include <vector>

typedef bool     OpcUa_Boolean;
typedef uint8_t  OpcUa_Byte;
typedef int8_t   OpcUa_SByte;
typedef uint16_t OpcUa_UInt16;
typedef int16_t  OpcUa_Int16;
typedef uint32_t OpcUa_UInt32;
typedef int32_t  OpcUa_Int32;
typedef uint64_t OpcUa_UInt64;
typedef int64_t  OpcUa_Int64;
typedef float    OpcUa_Float;
typedef double   OpcUa_Double;

// arrays = vectors
typedef std::vector<OpcUa_Boolean>  OpcUa_Booleanv;
typedef std::vector<OpcUa_Byte>  OpcUa_Bytev;
typedef std::vector<OpcUa_SByte>  OpcUa_SBytev;
typedef std::vector<OpcUa_Int16>  OpcUa_Int16v;
typedef std::vector<OpcUa_UInt16>  OpcUa_UInt16v;
typedef std::vector<OpcUa_UInt32>  OpcUa_UInt32v;
typedef std::vector<OpcUa_Int32>  OpcUa_Int32v;
typedef std::vector<OpcUa_UInt64>  OpcUa_UInt64v;
typedef std::vector<OpcUa_Int64>  OpcUa_Int64v;
typedef std::vector<OpcUa_Float>  OpcUa_Floatv;
typedef std::vector<OpcUa_Double>  OpcUa_Doublev;

