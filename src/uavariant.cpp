/* © Copyright Piotr Nikiel, CERN, 2015.  All rights not expressly granted are reserved.
 * uavariant.cpp
 *
 *  Created on: 15 Nov,  2015
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *      Author: Ben Farnham <benjamin.farnham@cern.ch>
 *      changes for arrays: May 2017, Michael Ludwig <michael.ludwig@cern.ch>
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

#include <open62541_compat.h>
#include <iostream>
#include <sstream>
#include <bitset>
#include <Utils.h>
#include <boost/format.hpp>
#include <boost/date_time.hpp>


#include <open62541_compat_common.h>


UA_Variant* UaVariant::createAndCheckOpen62541Variant()
{
	UA_Variant* open62541Variant = UA_Variant_new();
	if (!open62541Variant)
	{
		throw std::runtime_error("UA_Variant_new() returned 0");
	}
	return open62541Variant;
}

void UaVariant::destroyOpen62541Variant(UA_Variant* open62541Variant)
{
	if(open62541Variant)
	{
		UA_Variant_deleteMembers( open62541Variant );
		UA_Variant_delete( open62541Variant );
	}
}

UaVariant::UaVariant ()
:m_impl(createAndCheckOpen62541Variant())
{
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( OpcUa_UInt32 v )
:m_impl(createAndCheckOpen62541Variant())
{
	setUInt32( v );
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( OpcUa_Int32 v )
:m_impl(createAndCheckOpen62541Variant())
{
	setInt32( v );
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( OpcUa_Int32v v )
:m_impl(createAndCheckOpen62541Variant())
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::UaVariant " << std::endl;
	setInt32v( v );
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( const UaString& v )
:m_impl(createAndCheckOpen62541Variant())
{
	setString( v );
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( OpcUa_Float v )
:m_impl(createAndCheckOpen62541Variant())
{
	setFloat(v);
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( OpcUa_Boolean v )
:m_impl(createAndCheckOpen62541Variant())
{
	setBool(v);
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

UaVariant::UaVariant( const UaVariant& other)
:m_impl(createAndCheckOpen62541Variant())
{
	const UA_StatusCode status = UA_Variant_copy( other.m_impl, this->m_impl );
	if (status != UA_STATUSCODE_GOOD)
		throw std::runtime_error("UA_Variant_copy failed 0x"+Utils::toHexString(status) );
	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}


void UaVariant::operator= (const UaVariant &other)
{
	destroyOpen62541Variant(m_impl);
	m_impl = createAndCheckOpen62541Variant();

	const UA_StatusCode status = UA_Variant_copy( other.m_impl, this->m_impl );
	if (status != UA_STATUSCODE_GOOD)
		throw std::runtime_error("UA_Variant_copy failed 0x"+Utils::toHexString(status) );

	LOG(Log::TRC) << __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
}

bool UaVariant::operator==(const UaVariant& other) const
		{
	if(m_impl == 0 || other.m_impl == 0) return false; // uninitialized - cannot compare.
	if(type() != other.type()) return false;
	if(m_impl->arrayLength != other.m_impl->arrayLength) return false;
	if(m_impl->arrayDimensionsSize != other.m_impl->arrayDimensionsSize) return false;
	for(size_t arrayDimensionIndex = 0; arrayDimensionIndex < m_impl->arrayDimensionsSize; ++arrayDimensionIndex)
	{
		if(m_impl->arrayDimensions[arrayDimensionIndex] != other.m_impl->arrayDimensions[arrayDimensionIndex]) return false;
	}
	if(0 != memcmp(m_impl->data, other.m_impl->data, m_impl->arrayLength)) return false;

	return true;
		}

UaVariant::UaVariant( const UA_Variant& other )
:m_impl(createAndCheckOpen62541Variant())
{
	UA_StatusCode status = UA_Variant_copy( &other, this->m_impl );
	if (status != UA_STATUSCODE_GOOD)
		throw std::runtime_error("UA_Variant_copy failed");
}

UaVariant::~UaVariant()
{
	LOG(Log::TRC) <<"+"<< __PRETTY_FUNCTION__ << " m_impl="<<m_impl<<" m_impl.data="<<m_impl->data;
	destroyOpen62541Variant(m_impl);
	m_impl = 0;
}

OpcUaType UaVariant::type() const
{
	if (!m_impl->data)
		return OpcUaType_Null;
	else
	{
		if (m_impl->type->typeId.identifierType == UA_NODEIDTYPE_NUMERIC)
			return static_cast<OpcUaType>( m_impl->type->typeId.identifier.numeric );
		else
			throw std::runtime_error("No support for non built-in data types in variant! (yet)");
	}
}

void UaVariant::reuseOrRealloc( const UA_DataType* dataType, void* newValue )
{
	if ((m_impl->data != 0) && (m_impl->type == dataType))
	{
		/* No reason to realloc - the data of the same type will fit for sure! */
		// Piotr: not sure if this is safe for const-size data types like String
		UA_copy( newValue, m_impl->data, dataType );
	}
	else
	{
		/* Data type different - have to realloc */
		UA_Variant_deleteMembers( m_impl );
		// TODO throw when failed
		UA_StatusCode status = UA_Variant_setScalarCopy( m_impl, newValue, dataType);
		if (status != UA_STATUSCODE_GOOD)
			throw std::runtime_error("UA_Variant_setScalarCopy failed:"+Utils::toHexString(status));
	}
}


template<typename T>
void UaVariant::reuseOrReallocArray( const UA_DataType* dataType, size_t dim0, T* newValue )
{
	if (m_impl->arrayDimensionsSize > 1 ) {
		std::cout << __FILE__ << " " << __LINE__ << " reuseOrReallocArray: m_impl->arrayDimensionsSize= " << m_impl->arrayDimensionsSize << std::endl;
		throw std::runtime_error("reuseOrReallocArray: only 1-dim arrays are supported");
	}

	if ((m_impl->data != 0) && (m_impl->type == dataType)
			&& (m_impl->arrayLength == dim0 ))
	{
		/* No reason to realloc - the data of the same type will fit for sure! */
		UA_StatusCode status = UA_Array_copy( newValue, dim0, &m_impl->data, dataType );
		if (status != UA_STATUSCODE_GOOD)
			throw std::runtime_error("UA_ArrayCopy failed:"+Utils::toHexString(status));
	} else {
		/* Data type or size is different - have to realloc */
		UA_Variant_deleteMembers( m_impl );
		UA_StatusCode status = UA_Variant_setArrayCopy( m_impl, newValue, dim0, dataType);
		if (status != UA_STATUSCODE_GOOD)
			throw std::runtime_error("UA_Variant_setArrayCopy failed:"+Utils::toHexString(status));
	}
}

#if 0
// overloaded
void UaVariant::reuseOrReallocArray( const UA_DataType* dataType, size_t dim0, bool* newValue )
{
	if (m_impl->arrayDimensionsSize > 1 ) {
		std::cout << __FILE__ << " " << __LINE__ << " reuseOrReallocArray: m_impl->arrayDimensionsSize= " << m_impl->arrayDimensionsSize << std::endl;
		throw std::runtime_error("reuseOrReallocArray: only 1-dim arrays are supported");
	}

	if ((m_impl->data != 0) && (m_impl->type == dataType)
			&& (m_impl->arrayLength == dim0 ))
	{
		/* No reason to realloc - the data of the same type will fit for sure! */
		UA_StatusCode status = UA_Array_copy( newValue, dim0, &m_impl->data, dataType );
		if (status != UA_STATUSCODE_GOOD)
			throw std::runtime_error("UA_ArrayCopy failed:"+Utils::toHexString(status));
	} else {
		/* Data type or size is different - have to realloc */
		UA_Variant_deleteMembers( m_impl );
		UA_StatusCode status = UA_Variant_setArrayCopy( m_impl, newValue, dim0, dataType);
		if (status != UA_STATUSCODE_GOOD)
			throw std::runtime_error("UA_Variant_setArrayCopy failed:"+Utils::toHexString(status));
	}
}
#endif


void UaVariant::setBool( OpcUa_Boolean value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_BOOLEAN], &value );
}
void UaVariant::setBoolv( OpcUa_Booleanv value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setBoolv " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_BOOLEAN], value.size(), &value ); // attention! bool is handled differently by the compiler
}
void UaVariant::setBoolv( bool b )
{
	if (!b ) {
		OpcUa_Booleanv value;
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setBoolv init 0" << std::endl;
		reuseOrReallocArray( &UA_TYPES[UA_TYPES_BOOLEAN], value.size(), &value );// attention! bool is handled differently by the compiler
	} else {
		// the constructor has called this with b==true, I am not sure what this should actually do. ml
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setBoolv( bool==true ) not sure what you are trying to do" << std::endl;
	}
}

void UaVariant::setInt16( OpcUa_Int16 value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_INT16], &value );
}
void UaVariant::setInt16v( OpcUa_Int16v value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt16v " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_INT16], value.size(), (int16_t *) (&value[ 0 ]) );
}
void UaVariant::setInt16v( bool b )
{
	if (!b ) {
		OpcUa_Int16v value;
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt16v " << std::endl;
		reuseOrReallocArray( &UA_TYPES[UA_TYPES_INT16], value.size(), (int16_t *) (&value[ 0 ]) );
	} else {
		// the constructor has called this with b==true, I am not sure what this should actually do. ml
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt16v( bool==true ) not sure what you are trying to do" << std::endl;
	}
}


void UaVariant::setInt32( OpcUa_Int32 value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_INT32], &value );
}
void UaVariant::setInt32v( OpcUa_Int32v value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt32v " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_INT32], value.size(), (int32_t *) (&value[ 0 ]) );
}
void UaVariant::setInt32v( bool b )
{
	if (!b ) {
		OpcUa_Int32v value;
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt32v " << std::endl;
		reuseOrReallocArray( &UA_TYPES[UA_TYPES_INT32], value.size(), (int32_t *) (&value[ 0 ]) );
	} else {
		// the constructor has called this with b==true, I am not sure what this should actually do. ml
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt32v( bool==true ) not sure what you are trying to do" << std::endl;
	}
}

void UaVariant::setUInt32( OpcUa_UInt32 value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_UINT32], &value );
}
void UaVariant::setUInt32v( OpcUa_UInt32v value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setUInt32v " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_UINT32], value.size(), (uint32_t *) (&value[ 0 ]) );
}


void UaVariant::setInt64( OpcUa_Int64 value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_INT64], &value );
}
void UaVariant::setInt64v( OpcUa_Int64v value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt64v " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_INT64], value.size(), (int64_t *) (&value[ 0 ]) );
}
void UaVariant::setInt64v( bool b )
{
	if (!b ) {
		OpcUa_Int64v value;
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt64v " << std::endl;
		reuseOrReallocArray( &UA_TYPES[UA_TYPES_INT64], value.size(),  (int64_t *) (&value[ 0 ]) );
	} else {
		// the constructor has called this with b==true, I am not sure what this should actually do. ml
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setInt64v( bool==true ) not sure what you are trying to do" << std::endl;
	}
}

void UaVariant::setUInt64( OpcUa_UInt64 value )
{ 
	reuseOrRealloc( &UA_TYPES[UA_TYPES_UINT64], &value );
}
void UaVariant::setUInt64v( OpcUa_UInt64v value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setUInt64v " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_UINT64], value.size(), (uint64_t *)(&value[ 0 ]) );
}

void UaVariant::setFloat( OpcUa_Float value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_FLOAT], &value );
}
void UaVariant::setFloatv( OpcUa_Floatv value )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setFloatv " << std::endl;
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_FLOAT], value.size(), (float *)(&value[ 0 ]) );
}
void UaVariant::setFloatv( bool b )
{
	if (!b ) {
		OpcUa_Floatv value;
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setFloatv " << std::endl;
		reuseOrReallocArray( &UA_TYPES[UA_TYPES_FLOAT], value.size(), (float *) (&value[ 0 ]) );
	} else {
		// the constructor has called this with b==true, I am not sure what this should actually do. ml
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setFloatv( bool==true ) not sure what you are trying to do" << std::endl;
	}
}

void UaVariant::setDouble( OpcUa_Double value )
{
	reuseOrRealloc( &UA_TYPES[UA_TYPES_DOUBLE], &value );
}
void UaVariant::setDoublev( OpcUa_Doublev value )
{
	reuseOrReallocArray( &UA_TYPES[UA_TYPES_DOUBLE], value.size(), (double *) (&value[ 0 ]) );
}
void UaVariant::setDoublev( bool b )
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setDoublev " << std::endl;
	if (!b ) {
		OpcUa_Doublev value;
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setDoublev init 0" << std::endl;
		reuseOrReallocArray( &UA_TYPES[UA_TYPES_DOUBLE], value.size(), (double *) (&value[ 0 ]) );
	} else {
		// the constructor has called this with b==true, I am not sure what this should actually do. ml
		std::cout << __FILE__ << " " << __LINE__ << " UaVariant::setDoublev( bool==true ) not sure what you are trying to do" << std::endl;
	}
}

void UaVariant::setString( const UaString& value )
{
	/* UASTRING isn't a simple type and has to be handled with care. */
	if (m_impl->data != 0)
		UA_Variant_deleteMembers( m_impl );
	/* Now we assume that the variant is empty */
	UA_StatusCode s = UA_Variant_setScalarCopy( m_impl, value.impl(), &UA_TYPES[UA_TYPES_STRING]);

	if (s != UA_STATUSCODE_GOOD)
		throw alloc_error();
}

void UaVariant::setByteString( const UaByteString& value, bool detach)
{
	if (detach)
		throw std::runtime_error("value detachment not yet implemented");
	if (m_impl->data != 0)
		UA_Variant_deleteMembers( m_impl );
	UA_StatusCode s = UA_Variant_setScalarCopy( m_impl, value.impl(), &UA_TYPES[UA_TYPES_BYTESTRING]);
	if (s != UA_STATUSCODE_GOOD)
		throw alloc_error();
}

UaStatus UaVariant::toBool( OpcUa_Boolean& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_BOOLEAN], &out );
}
UaStatus UaVariant::toBoolv( OpcUa_Booleanv& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toBoolv " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_BOOLEAN], &out );
	return( ret );
}


UaStatus UaVariant::toInt16( OpcUa_Int16& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_INT16], &out );
}
UaStatus UaVariant::toInt16v( OpcUa_Int16v& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toInt16v " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_INT16], &out );
	return( ret );
}

UaStatus UaVariant::toInt32( OpcUa_Int32& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_INT32], &out );
}


UaStatus UaVariant::toInt32v( OpcUa_Int32v& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toInt32v " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_INT32], &out );
	return( ret );
}
UaStatus UaVariant::toUInt32( OpcUa_UInt32& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_UINT32], &out );
}

UaStatus UaVariant::toInt64( OpcUa_Int64& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_INT64], &out );
}
UaStatus UaVariant::toInt64v( OpcUa_Int64v& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toInt64v " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_INT64], &out );
	return( ret );
}

UaStatus UaVariant::toByte(OpcUa_Byte& out) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_BYTE], &out );
}
UaStatus UaVariant::toBytev( OpcUa_Bytev& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toBytev " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_BYTE], &out );
	return( ret );
}

UaStatus UaVariant::toFloat( OpcUa_Float& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_FLOAT], &out );
}
UaStatus UaVariant::toFloatv( OpcUa_Floatv& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toFloatv " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_FLOAT], &out );
	return( ret );
}

UaStatus UaVariant::toDouble( OpcUa_Double& out ) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_DOUBLE], &out );
}
UaStatus UaVariant::toDoublev( OpcUa_Doublev& out ) const
{
	std::cout << __FILE__ << " " << __LINE__ << " UaVariant::toDoublev " << std::endl;
	UaStatus ret = toArrayType( &UA_TYPES[UA_TYPES_DOUBLE], &out );
	return( ret );
}

UaStatus UaVariant::toByteString( UaByteString& out) const
{
	return toSimpleType( &UA_TYPES[UA_TYPES_BYTESTRING], &out );
}

UaString UaVariant::toString( ) const
{
	if (m_impl->type != &UA_TYPES[UA_TYPES_STRING])
		throw std::runtime_error("not-a-string");
	return UaString( (UA_String*)m_impl->data );
}

UaString UaVariant::toFullString() const
{
	std::ostringstream result;
	if(m_impl)
	{
		result << "type ["<<(m_impl->type)<<"] dimensions count ["<<m_impl->arrayDimensionsSize<<"]";
	}
	else
	{
		result << "type [EMPTY!]";
	}
	return UaString(result.str().c_str());
}

template<typename T>
UaStatus UaVariant::toSimpleType( const UA_DataType* dataType, T* out ) const
{
	//TODO: Add case when array
	//TODO: in principle it should be possible to convert i.e. Int16->Int32, or Byte->Int16, or so... but this is not supported yet
	if (!m_impl || !m_impl->data)
	{
		LOG(Log::DBG) << __PRETTY_FUNCTION__ << " conversion failed, variant is null";
		return OpcUa_Bad;
	}

	if (dataType != m_impl->type)
	{
		LOG(Log::DBG) << __PRETTY_FUNCTION__ << " conversion failed, target type ["<<dataType<<"] does not match variant type ["<<m_impl->type<<"]";
		return OpcUa_Bad;  // Incompatible data type
	}

	*out = *static_cast<T*>(m_impl->data);
	return OpcUa_Good;
}



UaStatus UaVariant::_doArrayTypesMatch( const UA_DataType* dataType ) const
{
	//TODO: Add case when array
	//TODO: in principle it should be possible to convert i.e. Int16->Int32, or Byte->Int16, or so... but this is not supported yet
	if (!m_impl || !m_impl->data)
	{
		LOG(Log::DBG) << __PRETTY_FUNCTION__ << " conversion failed, variant is null";
		return OpcUa_Bad;
	}

	if (dataType != m_impl->type)
	{
		LOG(Log::DBG) << __PRETTY_FUNCTION__ << " conversion failed, target type ["<<dataType<<"] does not match variant type ["<<m_impl->type<<"]";
		return OpcUa_Bad;  // Incompatible data type
	}
	return OpcUa_Good;
}

UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Booleanv *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Boolean *) m_impl->data)[ i ]);
	return OpcUa_Good;
}
UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Bytev *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Byte *) m_impl->data)[ i ]);
	return OpcUa_Good;
}
UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Int16v *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Int16 *) m_impl->data)[ i ]);
	return OpcUa_Good;
}
UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Int32v *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Int32 *) m_impl->data)[ i ]);
	return OpcUa_Good;
}
UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Int64v *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Int64 *) m_impl->data)[ i ]);
	return OpcUa_Good;
}
UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Floatv *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Float *) m_impl->data)[ i ]);
	return OpcUa_Good;
}
UaStatus UaVariant::toArrayType( const UA_DataType* dataType, OpcUa_Doublev *outv ) const
{
	UaStatus ret = _doArrayTypesMatch( dataType );
	if ( ret != OpcUa_Good ) return( ret );
	for ( unsigned int i = 0; i < m_impl->arrayLength; i++ )
		outv->push_back( ((OpcUa_Double *) m_impl->data)[ i ]);
	return OpcUa_Good;
}


