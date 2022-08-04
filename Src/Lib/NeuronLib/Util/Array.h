#pragma once

// For _ASSERT
#include "crtdbg.h"

// For memset
#include "memory.h"

// Wrapper for statically sized arrays.  Provides error checking in debug builds
// and has no additional memory cost.
template <class Type, int Size>
class Array
{
public:
	// Iterator class
	class It {
	public:
		It() {}
		It(Type* p) : p_(p) {}
		void operator ++() { ++p_; }
		Type& operator *() const { return *p_; }
		bool operator ==(const It& other) const { return p_ == other.p_; }
		bool operator !=(const It& other) const { return p_ != other.p_; }
	public:
		Type* p_ = nullptr;
	};

	// Const Iterator
	class CIt {
	public:
		CIt() {}
		CIt(const Type* p) : p_(p) {}
		void operator ++() { ++p_; }
		const Type& operator *() const { return *p_; }
		bool operator ==(const CIt& other) const { return p_ == other.p_; }
		bool operator !=(const CIt& other) const { return p_ != other.p_; }
	public:
		const Type* p_ = nullptr;
	};

public:
	// Returns the maximum capacity.  For arrays, this is always the same as Count.
	constexpr static int Capacity() { return Size; }

	// Returns the number of entries in the list
	constexpr static int Count() { return Size; }

	// Returns 'true' if the passed in index is valid, 'false' otherwise
	constexpr static bool IsValidIndex(int index) { return (index >= 0) && (index < Size); }

	// Memsets all data to 0
	void Zero() { memset(m_array, 0x00, sizeof(m_array)); }

	// Bracket operator to make indexing like an array simple
	const Type& operator[](int index) const
	{
		_ASSERT(IsValidIndex(index));
		return m_array[index];
	}

	Type& operator[](int index)
	{
		_ASSERT(IsValidIndex(index));
		return m_array[index];
	}

	const Type* Ptr() const { return m_array; }
	Type* Ptr() { return m_array; }

	// These are here to support "foreach" syntax
	// ie. for (auto i : list) {}
	CIt begin() const
	{
		return CIt(m_array);
	}

	CIt end() const
	{
		return CIt(m_array + Count());
	}

	It begin()
	{
		return It(m_array);
	}

	It end()
	{
		return It(m_array + Count());
	}

private:
	Type m_array[Size];
};
