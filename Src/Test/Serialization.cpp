#include "pch.h"
#include "CppUnitTest.h"

#include "NeuralNet/Network.h"
#include "Util/BinaryBuffer.h"
#include "Util/Math.h"
#include "Util/Random.h"
#include <streambuf>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(TestSerialization)
	{
	public:
		TEST_METHOD(NetworkComparison)
		{
			// Newly constructed networks with identical topology should be identical
			std::vector<int> neuronsPerLevel = { 8, 2, 4, 16, 3, 4 };
			Network network0(neuronsPerLevel);
			Network network1(neuronsPerLevel);
			Assert::IsTrue(network0 == network1);

			// If one of the networks is randomized, they should no longer be equal
			network0.Randomize();
			Assert::IsFalse(network0 == network1);

			// Assignment should restore equality
			network1 = network0;
			Assert::IsTrue(network0 == network1);
		}

		TEST_METHOD(NetworkSerialization)
		{
			// 10k buffer is big enough for this test
			constexpr int k_bufferSize = 1024 * 10;
			StackBuffer<k_bufferSize> buffer;

			std::vector<int> neuronsPerLevel = { 8, 2, 4, 16, 3, 4 };
			Network baseNetwork(neuronsPerLevel);
			baseNetwork.Randomize();

			baseNetwork.Serialize(buffer);

			Network copyNetwork;
			Assert::IsFalse(copyNetwork == baseNetwork);

			buffer.Seek(0);
			copyNetwork.Deserialize(buffer);
			Assert::IsTrue(copyNetwork == baseNetwork);
		}

		TEST_METHOD(BinaryBuffer)
		{
			const char* srcStr = "Poop Is Tasty";
			const int srcInt = 0xFA479009;
			const float srcFloat = 3.1415926f;

			StackBuffer<64> buffer;
			buffer.WriteBytes(srcStr, static_cast<int>(strlen(srcStr) + 1));
			buffer.WriteInt(srcInt);
			buffer.WriteFloat(srcFloat);

			char dstStr[64] = { 0 };
			int dstInt = 0;
			float dstFloat = 0.0f;

			buffer.Seek(0);
			buffer.ReadBytes(dstStr, static_cast<int>(strlen(srcStr) + 1));
			buffer.ReadInt(dstInt);
			buffer.ReadFloat(dstFloat);

			Assert::IsTrue(strcmp(srcStr, dstStr) == 0);
			Assert::IsTrue(dstInt == srcInt);
			Assert::IsTrue(dstFloat == srcFloat);
		}
	};
}
