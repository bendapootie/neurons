#include "pch.h"
#include "CppUnitTest.h"

#include "Util/Math.h"
#include "Util/Random.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(TestRandom)
	{
	public:
		class SequenceStats
		{
		public:
			void AddSample(double sample)
			{
				m_count++;
				double delta = sample - m_mean;
				m_mean += delta / static_cast<double>(m_count);
				m_m2 += delta * (sample - m_mean);
				m_variance = m_m2 / static_cast<double>(m_count);
				m_min = Math::Min(m_min, sample);
				m_max = Math::Max(m_max, sample);
			}
			double GetAverage() const { return m_mean; }
			int GetSampleCount() const { return m_count; }
			double GetVariance() const { return m_variance; }
			double GetStdDev() const { return ::sqrt(m_variance); }
			double GetMinSample() const { return m_min; }
			double GetMaxSample() const { return m_max; }

			static double ExpectedStdDevForUniformDistribution(double min, double max)
			{
				return (max - min) / Math::Sqrt(12.0);
			}
		public:
			double m_sum = 0;
			double m_mean = 0.0;
			double m_m2 = 0.0;
			double m_variance = 0.0;
			double m_min = Math::DoubleMax;
			double m_max = -Math::DoubleMax;
			int m_count = 0;
		};

		static constexpr int k_numSamples = 1000000;
		static constexpr double k_rangeTolerance = 0.01;
		static constexpr double k_sampleTolerance = 0.0001;
		static constexpr double k_avgTolerance = 0.001;
		static constexpr double k_stdTolerance = 0.002;

		// Verify SequenceStats helper class correctly calculates average, count, and standard deviation
		TEST_METHOD(SequenceStatsTest)
		{
			SequenceStats stats;
			stats.AddSample(6);
			stats.AddSample(8);
			stats.AddSample(12);
			stats.AddSample(14);
			Assert::AreEqual(stats.GetSampleCount(), 4);
			Assert::AreEqual(stats.GetAverage(), 10.0);
			Assert::IsTrue(abs(stats.GetStdDev() - 3.1622776601684) < 0.000001);
		}

		TEST_METHOD(RandomNextInt)
		{
			SequenceStats stats;
 			for (int i = 0; i < k_numSamples; i++)
 			{
				stats.AddSample(Random::NextInt());
 			}
			const double expectedAverage = (static_cast<double>(Math::Int32Min) + static_cast<double>(Math::Int32Max)) * 0.5;
			const double deltaFromAvg = Math::Abs(stats.GetAverage() - expectedAverage);
			const double ratioFromAvg = deltaFromAvg / Math::Int32Max;
			Assert::IsTrue(ratioFromAvg < k_avgTolerance,
				L"Average should be close to the center of the range"
			);

			const double range = static_cast<double>(Math::Int32Max) - static_cast<double>(Math::Int32Min);

			const double ratioFromMin = 1.0 - ((static_cast<double>(Math::Int32Max) - stats.GetMinSample()) / range);
			const double ratioFromMax = 1.0 - ((static_cast<double>(Math::Int32Max) - stats.GetMaxSample()) / range);
			Assert::IsTrue((ratioFromMin >= 0.0) && (ratioFromMin < k_sampleTolerance));
			Assert::IsTrue((ratioFromMax <= 1.0) && (ratioFromMax > (1.0 - k_sampleTolerance)));

			double expectedStddev = SequenceStats::ExpectedStdDevForUniformDistribution(Math::Int32Min, Math::Int32Max);
			double stddev = stats.GetStdDev();
			double ratio = stddev / expectedStddev;
			Assert::IsTrue(ratio > (1.0 - k_stdTolerance) && ratio < (1.0 + k_stdTolerance));
		}

		static void RandomNextIntRangeHelper(const int minInclusive, const int maxExclusive, const int numSamples = k_numSamples)
		{
			const double range = static_cast<double>(maxExclusive - 1.0) - static_cast<double>(minInclusive);

			SequenceStats stats;
			for (int i = 0; i < numSamples; i++)
			{
				stats.AddSample(Random::NextInt(minInclusive, maxExclusive));
			}
			double expectedAvg = (static_cast<double>(minInclusive) + static_cast<double>(maxExclusive - 1.0)) * 0.5;
			Assert::IsTrue(Math::Equals(stats.GetAverage(), expectedAvg, range * k_avgTolerance));
			// If there are sufficient samples (ie. 10x), assume that there will be full coverage of range
			double expectedMin = static_cast<double>(minInclusive);
			double expectedMax = static_cast<double>(maxExclusive - 1.0);
			// If there aren't enough samples to assume coverage, relax the requirement
			if (k_numSamples < (range * 10.0))
			{
				const double distFromBounds = 10.0 * (range / k_numSamples);
				expectedMin = minInclusive + distFromBounds;
				expectedMax = maxExclusive - 1.0 - distFromBounds;
			}

			Assert::IsTrue(stats.GetMinSample() <= expectedMin);
			Assert::IsTrue(stats.GetMaxSample() >= expectedMax);

			// Note: maxExclusive is used here instead of (maxExclusive - 1) because it gives a better approximation
			//       since NextInt will over-sample the extremes compared to a continuous uniform distribution 
			// Note: This estimation gets increasingly worse as the range gets small
			double expectedStddev = SequenceStats::ExpectedStdDevForUniformDistribution(minInclusive, maxExclusive);
			Assert::IsTrue(Math::Equals(stats.GetStdDev(), expectedStddev, expectedStddev * k_stdTolerance));
		}

		TEST_METHOD(RandomNextIntRange)
		{
			RandomNextIntRangeHelper(3, 45);
			RandomNextIntRangeHelper(-1243, -5);
			RandomNextIntRangeHelper(-50000, 975006);
		}

		TEST_METHOD(NextFloat)
		{
			SequenceStats stats;
			for (int i = 0; i < k_numSamples; i++)
			{
				stats.AddSample(Random::NextFloat());
			}
			Assert::IsTrue(Math::Equals(stats.GetAverage(), 0.5, k_avgTolerance));

			Assert::IsTrue((stats.GetMinSample() >= 0.0) && (stats.GetMinSample() < k_sampleTolerance));
			Assert::IsTrue((stats.GetMaxSample() <= 1.0) && (stats.GetMaxSample() > (1.0 - k_sampleTolerance)));

			double expectedStddev = SequenceStats::ExpectedStdDevForUniformDistribution(0.0, 1.0);
			Assert::IsTrue(Math::Equals(stats.GetStdDev(), expectedStddev, k_stdTolerance));
		}

		// TODO: Implement
// 		TEST_METHOD(NextFloatRange)
// 		{
// 			Assert::IsTrue(false, L"TODO: Implement test");
// 		}

		// TODO: Implement
// 		TEST_METHOD(NextGaussian)
// 		{
// 			Assert::IsTrue(false, L"TODO: Implement test");
// 		}
	};
}
