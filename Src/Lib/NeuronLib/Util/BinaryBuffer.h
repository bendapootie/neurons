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
	ErrorStatus GetErrorStatus() const { return m_error; }

	const char* GetPtr() const { return m_buffer; }
	int GetBufferSize() const { return m_bufferSize; }
	int GetCurrent() const { return m_current; }

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
		m_bufferSize(size)
	{
	}

	Array<char, 4> IntToCharArray(int value);
	int CharArrayToInt(Array<char, 4> src);

protected:
	char* m_buffer = nullptr;
	int m_bufferSize = 0;
	int m_current = 0;
	BufferStatus m_status = BufferStatus::Unset;
	ErrorStatus m_error = ErrorStatus::NoError;
};

//=============================================================================
// Version of BinaryBuffer that allocates a statically-sized buffer on the stack
template<int k_size>
class StackBuffer : public BinaryBuffer
{
public:
	StackBuffer() :
		BinaryBuffer(m_staticBuffer, k_size)
	{		
	}

private:
	char m_staticBuffer[k_size];
};

//=============================================================================
// Version of BinaryBuffer that allocates a statically-sized buffer on the heap
class HeapBuffer : public BinaryBuffer
{
public:
	HeapBuffer(int size) :
		m_dynamicBuffer(new char[size]),
		BinaryBuffer(m_dynamicBuffer, size)
	{
		m_buffer = m_dynamicBuffer;
	}
	~HeapBuffer()
	{
		_ASSERT(m_dynamicBuffer != nullptr);
		delete m_dynamicBuffer;
	}

private:
	char* m_dynamicBuffer = nullptr;
};

//=============================================================================
