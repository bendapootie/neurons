#include "pch.h"
#include "CppUnitTest.h"

#include "Util/RefCount.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(TestRefCount)
	{
	public:
		TEST_METHOD(AddRefAndRelease)
		{
			RefCount r;
			Assert::IsTrue(r.IsFree());

			r.AddRef();	// ref == 1
			Assert::IsFalse(r.IsFree());

			r.AddRef();	// ref == 2
			Assert::IsFalse(r.IsFree());

			r.Release(); // ref == 1
			Assert::IsFalse(r.IsFree());

			r.AddRef();	// ref == 2
			Assert::IsFalse(r.IsFree());

			r.Release(); // ref == 1
			Assert::IsFalse(r.IsFree());

			r.Release(); // ref == 0
			Assert::IsTrue(r.IsFree());
		}
	};
}
