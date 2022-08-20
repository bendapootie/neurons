#include "pch.h"
#include "Serializable.h"

#include "BinaryBuffer.h"

//-----------------------------------------------------------------------------

void ISerializable::SerializeInt(BinaryBuffer& stream, const int value)
{
	stream.WriteInt(value);
}

void ISerializable::SerializeFloat(BinaryBuffer& stream, const float value)
{
	stream.WriteFloat(value);
}

void ISerializable::SerializeBuffer(BinaryBuffer& stream, const void* ptr, const int size)
{
	const char* src = reinterpret_cast<const char*>(ptr);
	stream.WriteBytes(src, size);
}

//-----------------------------------------------------------------------------

void ISerializable::DeserializeInt(BinaryBuffer& stream, int& value)
{
	stream.ReadInt(value);
}

void ISerializable::DeserializeFloat(BinaryBuffer& stream, float& value)
{
	stream.ReadFloat(value);
}

void ISerializable::DeserializeBuffer(BinaryBuffer& stream, void* ptr, const int size)
{
	char* dst = reinterpret_cast<char*>(ptr);
	stream.ReadBytes(dst, size);
}

//-----------------------------------------------------------------------------
