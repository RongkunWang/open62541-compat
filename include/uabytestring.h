/*
 * uabytestring.h
 *
 *  Created on: Jan 26, 2017
 *      Author: pnikiel
 *
 *      Contents moved from uavariant.h as a cleanup effort
 *
 *
 */

#ifndef OPEN62541_COMPAT_INCLUDE_UABYTESTRING_H_
#define OPEN62541_COMPAT_INCLUDE_UABYTESTRING_H_

#include <opcua_platformdefs.h>
#include <open62541.h>

class UaByteString
{
public:
	// a byte-string of length 0
	UaByteString ();

	UaByteString( const int len, OpcUa_Byte* data);
	UaByteString( const UaByteString& other);

	~UaByteString ();

	OpcUa_Int32 length() const;
	const OpcUa_Byte* data() const { return m_impl->data; }

	void setByteString (const int len, OpcUa_Byte *Data);

	UA_ByteString* impl() const { return m_impl; }
private:

	UA_ByteString * m_impl;

};




#endif /* OPEN62541_COMPAT_INCLUDE_UABYTESTRING_H_ */
