#pragma once

// Simple class for managing ref counts
class RefCount
{
public:
	RefCount() {}

	void AddRef()
	{
		m_count++;
	}

	void Release()
	{
#ifdef _ASSERT
		_ASSERT(m_count > 0);
#endif // _ASSERT

		m_count--;
	}

	bool IsFree() const
	{
		return m_count == 0;
	}

private:
	int m_count = 0;
};
