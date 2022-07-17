#pragma once

#include <cmath>
#include "Constants.h"

// These "PLIB_xxxx_MACRO" #defines are here to allow optimization of inner loops in debug
// builds. The macro expansions can sometimes be faster since they avoid extra function calls,
// but they can also be slower if used incorrectly because the parameters get duplicated.
// These should only be used when you're absolutely sure you'll need them.
// TODO: Add tests to verify Macro and Function versions yield identical results
#ifdef DEBUG_BUILD
#define PLIB_CLAMP_MACRO(v, a, b) ((v) < (a) ? (a) : ((v) > (b) ? (b) : (v)))
#define PLIB_LERP_MACRO(a, b, t) ((a) + (((b) - (a)) * (t)))
#else // #ifdef DEBUG_BUILD
#define PLIB_CLAMP_MACRO(v, a, b) Math::Clamp(v, a, b)
#define PLIB_LERP_MACRO(a, b, t) Math::Lerp(a, b, t)
#endif // #ifdef DEBUG_BUILD

namespace Math
{
	// Returns whether x is a normal value: i.e., whether it is neither infinity, NaN, zero or subnormal.
	inline bool IsNormal(float f) { return ::isnormal(f); }
	inline bool IsNormal(double d) { return ::isnormal(d); }
	// Returns whether x is a NaN(Not - A - Number) value.
	inline bool IsNan(float f) { return ::isnan(f); }
	inline bool IsNan(double d) { return ::isnan(d); }
	// Returns whether x is an infinity value (either positive infinity or negative infinity).
	inline bool IsInfinite(float f) { return ::isinf(f); }
	inline bool IsInfinite(double d) { return ::isinf(d); }
	// Returns whether x is a finite value. A finite value is any floating - point value that is neither infinite nor NaN(Not - A - Number).
	inline bool IsFinite(float f) { return ::isfinite(f); }
	inline bool IsFinite(double d) { return ::isfinite(d); }

	inline float Abs(float f) { return ::fabsf(f); }
	inline double Abs(double d) { return ::fabs(d); }
	inline __int8 Abs(__int8 n) { return (__int8)::abs(n); }
	inline __int16 Abs(__int16 n) { return (__int16)::abs(n); }
	inline __int32 Abs(__int32 n) { return ::abs(n); }
	inline __int64 Abs(__int64 n) { return (n >= 0) ? n : -n; }		// Compiler barks if int64 tries to call ::abs()

	template <class T> inline const T& Min(const T& a, const T& b) { return (a < b) ? a : b; }
	template <class T> inline const T& Max(const T& a, const T& b) { return !(a < b) ? a : b; }

	template <class T> inline const T Clamp(const T& v, const T& a, const T& b) { return Max(a, Min(b, v)); }
	template <class T> inline const T Lerp(const T& a, const T& b, const T& t) { return a + ((b - a) * t); }
	template <class T> inline const T LerpF(const T& a, const T& b, const float t) { return a + ((b - a) * t); }

	inline bool Equals(const float& a, const float& b, const float tolerance = Math::FloatSmallNumber)
	{
		return Math::Abs(a - b) <= tolerance;
	}
	inline bool Equals(const double& a, const double& b, const double tolerance = Math::DoubleSmallNumber)
	{
		return Math::Abs(a - b) <= tolerance;
	}

	inline float Ceil(float f) { return ::ceilf(f); }
	inline double Ceil(double d) { return ::ceil(d); }
	inline float Floor(float f) { return ::floorf(f); }
	inline double Floor(double d) { return ::floor(d); }
	inline float Round(float f) { return ::roundf(f); }
	inline double Round(double d) { return ::round(d); }
	// Returns the fractional part of the float such that [Frac(f) + (int)f == f]
	// Note: If input is negative, a negative fraction is returned
	inline float Frac(float f) { return (f > 0.0f) ? (f - ::floorf(f)) : (f - ::ceilf(f)); }
	inline double Frac(double d) { return (d > 0.0f) ? (d - ::floor(d)) : (d - ::ceil(d)); }
	inline float Sign(float f) { return static_cast<float>(0.0f < f) - static_cast<float>(f < 0.0f); }
	inline double Sign(double d) { return static_cast<double>(0.0 < d) - static_cast<double>(d < 0.0); }

	// Normalizes a number to an arbitrary range, assuming the range wraps
	// around when going below min or above max 
	inline float Normalize(const float value, const float min, const float max)
	{
		const float width = max - min;
		const float offsetValue = value - min;
		return (offsetValue - (Floor(offsetValue / width) * width)) + min;
	}
	inline double Normalize(const double value, const double min, const double max)
	{
		const double width = max - min;
		const double offsetValue = value - min;
		return (offsetValue - (Floor(offsetValue / width) * width)) + min;
	}


	// Trig functions
	// Returns equivalent degrees in range [-180.0f .. 180.0f]
	inline float NormalizeDegrees(const float degrees) { return Normalize(degrees, -180.0f, 180.0f); }
	inline float RotateTowardsDegrees(const float src_degrees, const float dst_degrees, const float max_rotation_degrees)
	{
		const float normalized_delta = NormalizeDegrees(dst_degrees - src_degrees);
		if (Abs(max_rotation_degrees) < Abs(normalized_delta))
		{
			return src_degrees + Sign(normalized_delta) * max_rotation_degrees;
		}
		return dst_degrees;
	}
	constexpr float DegToRad(float degrees) { return degrees * (PiF / 180.0f); }
	constexpr float RadToDeg(float radians) { return radians * (180.0f / PiF); }
	inline float Sin(float f) { return ::sinf(f); }
	inline float Cos(float f) { return ::cosf(f); }
	inline float Tan(float f) { return ::tanf(f); }
	inline float ASin(float f) { return ::asinf(f); }
	inline float ACos(float f) { return ::acosf(f); }
	inline float ATan(float f) { return ::atanf(f); }
	inline float ATan2(float y, float x) { return ::atan2f(y, x); }
	inline void SinCos(float f, float& s, float& c) { s = Sin(f); c = Cos(f); }
	inline float Sqr(float f) { return f * f; }
	inline float Sqrt(float f) { return ::sqrtf(f); }
	inline float InvSqrt(float f) { return 1.0f / ::sqrtf(f); }
	inline float Power(float base, float exponent) { return ::powf(base, exponent); }
	inline float Mod(float numerator, float denominator) { return ::fmodf(numerator, denominator); }

	// Returns equivalent degrees in range [-180.0 .. 180.0]
	inline double NormalizeDegrees(const double degrees) { return Normalize(degrees, -180.0, 180.0); }
	inline double RotateTowardsDegrees(const double src_degrees, const double dst_degrees, const double max_rotation_degrees)
	{
		const double normalized_delta = NormalizeDegrees(dst_degrees - src_degrees);
		if (Abs(max_rotation_degrees) < Abs(normalized_delta))
		{
			return src_degrees + Sign(normalized_delta) * max_rotation_degrees;
		}
		return dst_degrees;
	}
	inline double DegToRad(double degrees) { return degrees * (Pi / 180.0); }
	inline double RadToDeg(double radians) { return radians * (180.0 / Pi); }
	inline double Sin(double d) { return ::sin(d); }
	inline double Cos(double d) { return ::cos(d); }
	inline double Tan(double d) { return ::tan(d); }
	inline double ASin(double d) { return ::asin(d); }
	inline double ACos(double d) { return ::acos(d); }
	inline double ATan(double d) { return ::atan(d); }
	inline double ATan2(double y, double x) { return ::atan2(y, x); }
	inline void SinCos(double d, double& s, double& c) { s = Sin(d); c = Cos(d); }
	inline double Sqr(double d) { return d * d; }
	inline double Sqrt(double d) { return ::sqrt(d); }
	inline double InvSqrt(double d) { return 1.0 / ::sqrt(d); }
	inline double Power(double base, double exponent) { return ::pow(base, exponent); }
	inline double Mod(double numerator, double denominator) { return ::fmod(numerator, denominator); }
};
