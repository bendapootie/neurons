#pragma once

#include <type_traits>	// for std::is_trivially_copyable

class BinaryBuffer;

class ISerializable
{
public:
	virtual void Serialize(BinaryBuffer& stream) const = 0;
	virtual void Deserialize(BinaryBuffer& stream) = 0;

protected:
	static void SerializeInt(BinaryBuffer& stream, const int value);
	static void SerializeFloat(BinaryBuffer& stream, const float value);
	static void SerializeBuffer(BinaryBuffer& stream, const void* ptr, const int size);
	template<typename T>
	static void SerializeSimpleObject(BinaryBuffer& stream, const T& object);

	static void DeserializeInt(BinaryBuffer& stream, int& value);
	static void DeserializeFloat(BinaryBuffer& stream, float& value);
	static void DeserializeBuffer(BinaryBuffer& stream, void* ptr, const int size);
	template<typename T>
	static void DeserializeSimpleObject(BinaryBuffer& stream, T& object);
};

template<typename T>
void ISerializable::SerializeSimpleObject(BinaryBuffer& stream, const T& object)
{
	static_assert(std::is_trivially_copyable<T>::value, "This should only be called on simple objects");
	const void* ptr = &object;
	SerializeBuffer(stream, ptr, sizeof(object));
}

template<typename T>
void ISerializable::DeserializeSimpleObject(BinaryBuffer& stream, T& object)
{
	static_assert(std::is_trivially_copyable<T>::value, "This should only be called on simple objects");
	void* ptr = &object;
	DeserializeBuffer(stream, ptr, sizeof(object));
}
