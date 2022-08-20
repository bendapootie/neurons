#pragma once

#include "Array.h"

// Base class for binary buffer streams
// Not directly instantiable because it's constructors are protected. Use derived classes instead.
class BinaryBuffer
{
public:
	enum class BufferStatus
	{
		Unset,
		Read,
		Write,
		ReadWrite
	};
	enum class ErrorStatus
	{
		NoError,
		EndOfBuffer
	};
	enum class SeekBase
	{
		Beginning,
		Current,
		End
	};

public:
	void Seek(int pos);
	void Seek(int offset, SeekBase base);

	bool WriteBytes(const char* src, int bytes);
	bool WriteInt(int value);
	bool WriteFloat(float value);

	bool ReadBytes(char* dst, int bytes);
	bool ReadInt(int& dst);
	bool ReadFloat(float& dst);

protected:
	BinaryBuffer(char* buffer, int size) :
		m_buffer(buffer),
		m_size(size)
	{
	}

	Array<char, 4> IntToCharArray(int value);
	int CharArrayToInt(Array<char, 4> src);

protected:
	char* m_buffer = nullptr;
	int m_size = 0;
	int m_current = 0;
	BufferStatus m_status = BufferStatus::Unset;
	ErrorStatus m_error = ErrorStatus::NoError;
};

//=============================================================================

template<int k_size>
class StaticBuffer : public BinaryBuffer
{
public:
	StaticBuffer() :
		BinaryBuffer(m_staticBuffer, k_size)
	{		
	}

private:
	char m_staticBuffer[k_size];
};

//=============================================================================

//=============================================================================
