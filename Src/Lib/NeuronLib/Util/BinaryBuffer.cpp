#include "pch.h"
#include "BinaryBuffer.h"

void BinaryBuffer::Seek(int pos)
{
	_ASSERT(pos >= 0 && pos <= m_bufferSize);
	m_current = pos;
}

void BinaryBuffer::Seek(int offset, SeekBase base)
{
	switch(base)
	{
	case SeekBase::Beginning:
		Seek(offset);
		break;
	case SeekBase::Current:
		Seek(m_current + offset);
		break;
	case SeekBase::End:
		Seek(m_bufferSize - offset);
		break;
	default:
		_ASSERT(false);	// Shouldn't ever get here!
		break;
	}
}


bool BinaryBuffer::WriteBytes(const char* src, int bytes)
{
	if (m_current + bytes <= m_bufferSize)
	{
		memcpy(m_buffer + m_current, src, bytes);
		m_current += bytes;
		return true;
	}
	m_error = ErrorStatus::EndOfBuffer;
	return false;
}

bool BinaryBuffer::WriteInt(int value)
{
	Array<char, 4> tmp = IntToCharArray(value);
	return WriteBytes(tmp.Ptr(), tmp.Count());
}

bool BinaryBuffer::WriteFloat(float value)
{
	int valueAsInt = *reinterpret_cast<int*>(&value);
	return WriteInt(valueAsInt);
}

bool BinaryBuffer::ReadBytes(char* dst, int bytes)
{
	if (m_current + bytes <= m_bufferSize)
	{
		memcpy(dst, m_buffer + m_current, bytes);
		m_current += bytes;
		return true;
	}
	m_error = ErrorStatus::EndOfBuffer;
	return false;
}

bool BinaryBuffer::ReadInt(int& dst)
{
	Array<char, 4> tmp;
	const bool success = ReadBytes(tmp.Ptr(), tmp.Count());
	if (success)
	{
		dst = CharArrayToInt(tmp);
	}
	return success;
}

bool BinaryBuffer::ReadFloat(float& dst)
{
	int tmp;
	const bool success = ReadInt(tmp);
	if (success)
	{
		dst = *reinterpret_cast<float*>(&tmp);
	}
	return success;
}

Array<char, 4> BinaryBuffer::IntToCharArray(int value)
{
	Array<char, 4> dst;
	dst[0] = value;
	dst[1] = value >> 8;
	dst[2] = value >> 16;
	dst[3] = value >> 24;
	return dst;
}

int BinaryBuffer::CharArrayToInt(Array<char, 4> src)
{
	return
		(static_cast<int>(src[0]) & 0xFF) |
		((static_cast<int>(src[1]) & 0xFF) << 8) |
		((static_cast<int>(src[2]) & 0xFF) << 16) |
		((static_cast<int>(src[3]) & 0xFF) << 24);
}
