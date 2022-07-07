#pragma  once

constexpr float k_pi = 3.14159265358979323846f;
constexpr float k_2pi = k_pi * 2.0f;
constexpr float k_piOver180 = k_pi / 180.0f;
constexpr float k_180OverPi = 180.0f / k_pi;

constexpr float k_floatSmallNumber = 1e-4f;			// "smallish" number, useful for epsilon tests

constexpr float RadToDeg(float rad) { return rad * k_180OverPi; }
constexpr float DegToRad(float deg) { return deg * k_piOver180; }

namespace Math
{
	static_assert(sizeof(float) == 4, "Assuming that float is 4 bytes");
	static_assert(sizeof(double) == 8, "Assuming that double is 8 bytes");

	constexpr double Pi = 3.14159265358979323846264338327950;
	constexpr double TwoPi = Pi * 2.0;
	constexpr double PiOver2 = Pi / 2.0;
	constexpr float PiF = 3.14159265358979f;
	constexpr float TwoPiF = PiF * 2.0f;
	constexpr float PiOver2F = PiF / 2.0f;

// 	constexpr int8 Int8Min = (-127) - 1;
// 	constexpr int8 Int8Max = 127;
// 	constexpr int16 Int16Min = (-32767) - 1;
// 	constexpr int16 Int16Max = 32767;
// 	constexpr int32 Int32Min = (-2147483647) - 1;
// 	constexpr int32 Int32Max = 2147483647;
// 	constexpr int64 Int64Min = (-9223372036854775807) - 1;
// 	constexpr int64 Int64Max = 9223372036854775807;
// 
// 	constexpr uint8 UInt8Min = 0;
// 	constexpr uint8 UInt8Max = 255;
// 	constexpr uint16 UInt16Min = 0;
// 	constexpr uint16 UInt16Max = 65535;
// 	constexpr uint32 UInt32Min = 0;
// 	constexpr uint32 UInt32Max = 4294967295;
// 	constexpr uint64 UInt64Min = 0;
// 	constexpr uint64 UInt64Max = 18446744073709551615;

	constexpr float FloatEpsilon = 1.192092896e-07F;	// smallest value such that (1.0 + FloatEpsilon) != 1.0
	constexpr float FloatMax = 3.402823466e+38F;		// max value
	constexpr float FloatMin = 1.175494351e-38F;		// min positive value
	constexpr float FloatSmallNumber = 1e-4f;			// "smallish" number, useful for epsilon tests

	constexpr double DoubleEpsilon = 2.2204460492503131e-016;	// smallest value such that (1.0 + DoubleEpsilon) != 1.0
	constexpr double DoubleMax = 1.7976931348623158e+308;		// max value
	constexpr double DoubleMin = 2.2250738585072014e-308;		// min positive value
	constexpr double DoubleSmallNumber = 1e-8;					// "smallish" number, useful for epsilon tests
};
