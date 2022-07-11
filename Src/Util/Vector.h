#pragma once

#include "Math.h"
#include "Constants.h"

struct Point2;
struct Vector2;
struct Vector3;
struct Vector4;

// TODO: Make a template to support float and double vectors?
struct Vector2
{
public:
	static const Vector2 Zero;		// (0, 0)
	static const Vector2 One;		// (1, 1)
	static const Vector2 UnitX;		// (1, 0)
	static const Vector2 UnitY;		// (0, 1)

public:
	float x = 0.0f;
	float y = 0.0f;

	// Default constructor doesn't do any initialization
	Vector2() {}
	Vector2(float _x, float _y) : x(_x), y(_y) {}
	Vector2(const Vector2& other) : x(other.x), y(other.y) {}

	// Somewhat hacky functions to get a 'float' pointer to the start of the Vector
	const float* Ptr() const { return &x; }
	float* Ptr() { return &x; }

	Point2 ToPoint2() const;
	Vector3 ToVector3(float z = 0.0f) const;

	bool operator == (const Vector2& other) const { return (x == other.x) && (y == other.y); }
	bool operator != (const Vector2& other) const { return !(operator==(other)); }

	bool Equals(const Vector2& other, const float tolerance = k_floatSmallNumber) const
	{
		return Math::Abs(x - other.x) <= tolerance && Math::Abs(y - other.y) <= tolerance;
	}

	float Dot(const Vector2& other) const { return (x * other.x) + (y * other.y); }

	// In-place vector math functions
	void Add(const Vector2& rhs);
	void Subtract(const Vector2& rhs);

	void NormalizeInPlace();
	Vector2 GetNormalized() const;

	// Versions of 'normalize' that return Zero if called on a zero length vector
	void SafeNormalizeInPlace();
	Vector2 GetSafeNormalized() const;
	// Same as GetSafeNormalized, except the 'length' output parameter
	// is set to thelength of the input vector. Useful for minimizing calculations
	Vector2 GetSafeNormalized(__out float& length) const;

	float GetLength() const;
	float GetLengthSquared() const;

	float GetDistance(const Vector2& point) const;
	float GetDistanceSquared(const Vector2& point) const;

	Vector2 RotateAroundOrigin(const float radians) const;
	Vector2 RotateAroundPoint(const float radians, const Vector2& centerOfRotation) const;

	// Other vector functions
	Vector2 PerElementMultiply(const Vector2& rhs) const;
	Vector2 PerElementDivide(const Vector2& rhs) const;

	// Vector overloaded operators
	Vector2 operator + (const Vector2& rhs) const;
	void operator += (const Vector2& rhs);
	Vector2 operator - (const Vector2& rhs) const;
	void operator -= (const Vector2& rhs);
	// Unary negation operator
	Vector2 operator - () const;
	// Component-wise multiply
	Vector2 operator * (const Vector2& rhs) const { return Vector2(x * rhs.x, y * rhs.y); }
	void operator *= (const Vector2& rhs) { x *= rhs.x; y *= rhs.y; }
	// Component-wise divide
	Vector2 operator / (const Vector2& rhs) const { return Vector2(x / rhs.x, y / rhs.y); }
	void operator /= (const Vector2& rhs) { x /= rhs.x; y /= rhs.y; }
	// Scalar multiply
	Vector2 operator * (const float scale) const { return Vector2(x * scale, y * scale); }
	void operator *= (const float scale) { x *= scale; y *= scale; }
	// Scalar divide
	Vector2 operator / (const float scale) const { return Vector2(x / scale, y / scale); }
	void operator /= (const float scale) { x /= scale; y /= scale; }

	// Returns true if any component is NaN
	bool IsNan() const { return Math::IsNan(x) || Math::IsNan(y); }
	// Returns true if any component is Infinite
	bool IsInfinite() const { return Math::IsInfinite(x) || Math::IsInfinite(y); }
	// Returns true if _all_ components are Finite
	bool IsFinite() const { return Math::IsFinite(x) && Math::IsFinite(y); }
};

struct Vector3
{
public:
	static const Vector3 Zero;		// (0, 0, 0)
	static const Vector3 One;		// (1, 1, 1)
	static const Vector3 UnitX;		// (1, 0, 0)
	static const Vector3 UnitY;		// (0, 1, 0)
	static const Vector3 UnitZ;		// (0, 0, 1)

public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	// Default constructor doesn't do any initialization
	Vector3() {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

	// Somewhat hacky functions to get a 'float' pointer to the start of the Vector
	const float* Ptr() const { return &x; }
	float* Ptr() { return &x; }

	bool operator == (const Vector3& other) const { return (x == other.x) && (y == other.y) && (z == other.z); }
	bool operator != (const Vector3& other) const { return !(operator==(other)); }

	bool Equals(const Vector3& other, const float tolerance = k_floatSmallNumber) const
	{
		return Math::Abs(x - other.x) <= tolerance && Math::Abs(y - other.y) <= tolerance && Math::Abs(z - other.z) <= tolerance;
	}

	float Dot(const Vector3& other) const { return (x * other.x) + (y * other.y) + (z * other.z); }
	Vector3 Cross(const Vector3& other) const { return Vector3((y * other.z) - (other.y * z), (z * other.x) - (other.z * x), (x * other.y) - (other.x * y)); }

	// In-place vector math functions
	void Add(const Vector3& rhs);
	void Subtract(const Vector3& rhs);

	void NormalizeInPlace();
	Vector3 GetNormalized() const;

	float GetLength() const;
	float GetLengthSquared() const;
	float GetDistance(const Vector3& point) const;
	float GetDistanceSquared(const Vector3& point) const;

	// Other vector functions
	Vector3 PerElementMultiply(const Vector3& rhs) const;
	Vector3 PerElementDivide(const Vector3& rhs) const;

	//void RotateAroundAxis(const Vector3& axis, const float angle);

	void RotateAroundXAxis(const float angle);
	void RotateAroundYAxis(const float angle);
	void RotateAroundZAxis(const float angle);
	void RotateYawPitchRoll(const float yaw, const float pitch, const float roll);

	// Vector overloaded operators
	Vector3 operator - () const;						// Unary negation operator
	Vector3 operator + (const Vector3& rhs) const;
	void operator += (const Vector3& rhs);
	Vector3 operator - (const Vector3& rhs) const;
	void operator -= (const Vector3& rhs);
	// Component-wise multiply
	Vector3 operator * (const Vector3& rhs) const { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
	void operator *= (const Vector3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; }
	// Component-wise divide
	Vector3 operator / (const Vector3& rhs) const { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
	void operator /= (const Vector3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; }
	// Scalar Multiply
	Vector3 operator * (const float scale) const { return Vector3(x * scale, y * scale, z * scale); }
	void operator *= (const float scale) { x *= scale; y *= scale; z *= scale; }
	// Scalar Divide
	Vector3 operator / (const float d) const { return Vector3(x / d, y / d, z / d); }
	void operator /= (const float scale) { x /= scale; y /= scale; z /= scale; }

	// Returns true if any component is NaN
	bool IsNan() const { return Math::IsNan(x) || Math::IsNan(y) || Math::IsNan(z); }
	// Returns true if any component is Infinite
	bool IsInfinite() const { return Math::IsInfinite(x) || Math::IsInfinite(y) || Math::IsInfinite(z); }
	// Returns true if _all_ components are Finite
	bool IsFinite() const { return Math::IsFinite(x) && Math::IsFinite(y) && Math::IsFinite(z); }

	// Static utility functions
	static Vector3 Lerp(const Vector3& start, const Vector3& end, const float t);
};

struct Vector4
{
public:
	static const Vector4 Zero;		// (0, 0, 0, 0)
	static const Vector4 One;		// (1, 1, 1, 1)
	static const Vector4 UnitX;		// (1, 0, 0, 0)
	static const Vector4 UnitY;		// (0, 1, 0, 0)
	static const Vector4 UnitZ;		// (0, 0, 1, 0)
	static const Vector4 UnitW;		// (0, 0, 0, 1)

public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;	// Note: 'w' comes after x,y,z

	// Default constructor doesn't do any initialization
	Vector4() {}
	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	// Somewhat hacky functions to get a 'float' pointer to the start of the Vector
	const float* Ptr() const { return &x; }
	float* Ptr() { return &x; }

	// Conversions
	// Returns a copy of the Vector4 in Vector3 format, omitting the 'w' component
	Vector3 ToVector3() const { return Vector3(x, y, z); }
// 	// Returns a copy of the Vector4 in Quaternion format
// 	Quaternion ToQuaternion() const { return Quaternion(x, y, z, w); }
	// Returns a reference to this Vector4 in Vector3 format
	Vector3& AsVector3() { return *reinterpret_cast<Vector3*>(this); }
	const Vector3& AsVector3() const { return *reinterpret_cast<const Vector3*>(this); }
// 	// Returns a reference to this Vector4 in Quaternion format
// 	Quaternion& AsQuaternion() { return *reinterpret_cast<Quaternion*>(this); }
// 	const Quaternion& AsQuaternion() const { return *reinterpret_cast<const Quaternion*>(this); }

	bool operator == (const Vector4& other) const { return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w); }
	bool operator != (const Vector4& other) const { return !(operator==(other)); }

	bool Equals(const Vector4& other, const float tolerance = k_floatSmallNumber) const
	{
		return Math::Abs(x - other.x) <= tolerance && Math::Abs(y - other.y) <= tolerance && Math::Abs(z - other.z) <= tolerance && Math::Abs(w - other.w) <= tolerance;
	}

	float Dot(const Vector4& other) const { return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w); }

	// In-place vector math functions
	void Add(const Vector4& rhs);
	void Subtract(const Vector4& rhs);

	void NormalizeInPlace();
	Vector4 GetNormalized() const;

	// Other vector functions
	Vector4 PerElementMultiply(const Vector4& rhs) const;
	Vector4 PerElementDivide(const Vector4& rhs) const;

	// Vector overloaded operators
	Vector4 operator + (const Vector4& rhs) const;
	void operator += (const Vector4& rhs);
	Vector4 operator - (const Vector4& rhs) const;
	void operator -= (const Vector4& rhs);
	// Component-wise multiply
	Vector4 operator * (const Vector4& rhs) const { return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
	void operator *= (const Vector4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; }
	// Component-wise divide
	Vector4 operator / (const Vector4& rhs) const { return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
	void operator /= (const Vector4& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; }
	// Scalar Multiply
	Vector4 operator * (const float scale) const { return Vector4(x * scale, y * scale, z * scale, w * scale); }
	void operator *= (const float scale) { x *= scale; y *= scale; z *= scale; w *= scale; }
	// Scalar Divide
	Vector4 operator / (const float scale) const { return Vector4(x / scale, y / scale, z / scale, w / scale); }
	void operator /= (const float scale) { x /= scale; y /= scale; z /= scale; w /= scale; }

	// Returns true if any component is NaN
	bool IsNan() const {
		return Math::IsNan(x) || Math::IsNan(y) || Math::IsNan(z) || Math::IsNan(w);
	}
	// Returns true if any component is Infinite
	bool IsInfinite() const {
		return Math::IsInfinite(x) || Math::IsInfinite(y) || Math::IsInfinite(z) || Math::IsInfinite(w);
	}
	// Returns true if _all_ components are Finite
	bool IsFinite() const {
		return Math::IsFinite(x) && Math::IsFinite(y) && Math::IsFinite(z) && Math::IsFinite(w);
	}
};


//=============================================================================
// Point2 - Integer equivalent of Vector2
//-----------------------------------------------------------------------------
struct Point2
{
public:
	static const Point2 Zero;		// (0, 0)
	static const Point2 One;		// (1, 1)
	static const Point2 UnitX;		// (1, 0)
	static const Point2 UnitY;		// (0, 1)

public:
	int x = 0;
	int y = 0;

	// Default constructor doesn't do any initialization
	Point2() {}
	Point2(int _x, int _y) : x(_x), y(_y) {}
	Point2(const Point2& other) : x(other.x), y(other.y) {}

	// Somewhat hacky functions to get an 'int' pointer to the start of the Point
	const int* Ptr() const { return &x; }
	int* Ptr() { return &x; }

	bool operator == (const Point2& other) const { return (x == other.x) && (y == other.y); }
	bool operator != (const Point2& other) const { return !(operator==(other)); }

	// In-place Point math functions
	void Add(const Point2& rhs) { x += rhs.x; y += rhs.y; }
	void Subtract(const Point2& rhs) { x -= rhs.x; y -= rhs.y; }

	int GetLengthSquared() const { return (x * x) + (y * y); }

	int GetManhattanDistance(const Point2& point) const { return Math::Abs(x - point.x) + Math::Abs(y - point.y); }
	int GetDistanceSquared(const Point2& point) const { return (x - point.x) * (x - point.x) + (y - point.y) * (y - point.y); }

	// Other Point functions
	Point2 PerElementMultiply(const Point2& rhs) const { return Point2(x * rhs.x, y * rhs.y); }
	Point2 PerElementDivide(const Point2& rhs) const { return Point2(x / rhs.x, y / rhs.y); }

	Vector2 ToVector2() const { return Vector2(static_cast<float>(x), static_cast<float>(y)); }

	// Point overloaded operators
	Point2 operator + (const Point2& rhs) const { return Point2(x + rhs.x, y + rhs.y); }
	void operator += (const Point2& rhs) { x += rhs.x; y += rhs.y; }
	Point2 operator - (const Point2& rhs) const { return Point2(x - rhs.x, y - rhs.y); }
	void operator -= (const Point2& rhs) { x -= rhs.x; y -= rhs.y; }
	Point2 operator * (const int scale) const { return Point2(x * scale, y * scale); }
	void operator *= (const int scale) { x *= scale; y *= scale; }
	Point2 operator / (const int divisor) const { return Point2(x / divisor, y / divisor); }
	void operator /= (const int divisor) { x /= divisor; y /= divisor; }
};


//=============================================================================
// Inline function implementations
//-----------------------------------------------------------------------------
inline Point2 Vector2::ToPoint2() const
{
	return Point2(static_cast<int>(x), static_cast<int>(y));
}

inline Vector3 Vector2::ToVector3(float z) const
{
	return Vector3(x, y, z);
}
