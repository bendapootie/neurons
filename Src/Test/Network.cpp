#include "pch.h"
#include "CppUnitTest.h"

#include "NeuralNet/Network.h"
#include "Util/Math.h"
#include "Util/Random.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(TestNeuralNetwork)
	{
	public:
		TEST_METHOD(AddLevel)
		{
			std::vector<int> neuronsPerLevel = { 2, 1 };
			Network testNetwork(neuronsPerLevel);

			Random rand;
			rand.Seed(1234);

			testNetwork.Randomize(rand);

			std::vector<float> inputs = { 0.4f, -0.8f };
			std::vector<float> outControl = testNetwork.Evaluate(inputs);

			// New level is expected to be an identity level
			testNetwork.AddIdentityLevel(1);

			std::vector<float> outTest = testNetwork.Evaluate(inputs);

			// If this fails
			Assert::IsTrue(outControl == outTest);
		}
	};
}
