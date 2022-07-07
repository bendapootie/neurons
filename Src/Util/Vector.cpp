#include "Vector.h"


//-----------------------------------------------------------------------------
// Vector2
//-----------------------------------------------------------------------------
const Vector2 Vector2::Zero(0, 0);
const Vector2 Vector2::One(1, 1);
const Vector2 Vector2::UnitX(1, 0);
const Vector2 Vector2::UnitY(0, 1);

void Vector2::Add(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vector2::Subtract(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vector2::NormalizeInPlace()
{
	const float lenSquared = this->Dot(*this);
	const float invLength = Math::InvSqrt(lenSquared);
	(*this) *= invLength;
}

Vector2 Vector2::GetNormalized() const
{
	const float lenSquared = this->Dot(*this);
	const float invLength = Math::InvSqrt(lenSquared);
	return (*this) * invLength;
}

void Vector2::SafeNormalizeInPlace()
{
	const float lenSquared = this->Dot(*this);
	if (lenSquared > Math::FloatSmallNumber)
	{
		const float invLength = Math::InvSqrt(lenSquared);
		(*this) *= invLength;
	}
	else
	{
		(*this) = Vector2::Zero;
	}
}

Vector2 Vector2::GetSafeNormalized() const
{
	const float lenSquared = this->Dot(*this);
	if (lenSquared > Math::FloatSmallNumber)
	{
		const float invLength = Math::InvSqrt(lenSquared);
		return (*this) * invLength;
	}
	return Vector2::Zero;
}

Vector2 Vector2::GetSafeNormalized(__out float& length) const
{
	const float lenSquared = this->Dot(*this);
	if (lenSquared > Math::FloatSmallNumber)
	{
		length = Math::Sqrt(lenSquared);
		return (*this) * (1.0f / length);
	}
	length = 0.0f;
	return Vector2::Zero;
}

float Vector2::GetLength() const
{
	return Math::Sqrt(this->Dot(*this));
}

float Vector2::GetLengthSquared() const
{
	return this->Dot(*this);
}

float Vector2::GetDistance(const Vector2& point) const
{
	return ((*this) - point).GetLength();
}

float Vector2::GetDistanceSquared(const Vector2& point) const
{
	return ((*this) - point).GetLengthSquared();
}

Vector2 Vector2::PerElementMultiply(const Vector2& rhs) const
{
	return Vector2(x * rhs.x, y * rhs.y);
}

Vector2 Vector2::PerElementDivide(const Vector2& rhs) const
{
	return Vector2(x / rhs.x, y / rhs.y);
}

Vector2 Vector2::operator + (const Vector2& rhs) const
{
	return Vector2(x + rhs.x, y + rhs.y);
}

void Vector2::operator += (const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

Vector2 Vector2::operator - (const Vector2& rhs) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

void Vector2::operator -= (const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

Vector2 Vector2::operator - () const
{
	return Vector2(-x, -y);
}

//-----------------------------------------------------------------------------
// Vector3
//-----------------------------------------------------------------------------
const Vector3 Vector3::Zero(0, 0, 0);
const Vector3 Vector3::One(1, 1, 1);
const Vector3 Vector3::UnitX(1, 0, 0);
const Vector3 Vector3::UnitY(0, 1, 0);
const Vector3 Vector3::UnitZ(0, 0, 1);

void Vector3::Add(const Vector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}

void Vector3::Subtract(const Vector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

void Vector3::NormalizeInPlace()
{
	const float lenSquared = this->Dot(*this);
	const float invLength = Math::InvSqrt(lenSquared);
	(*this) *= invLength;
}

Vector3 Vector3::GetNormalized() const
{
	const float lenSquared = this->Dot(*this);
	const float invLengh = Math::InvSqrt(lenSquared);
	return (*this) * invLengh;
}

float Vector3::GetLength() const
{
	return Math::Sqrt(this->Dot(*this));
}

float Vector3::GetLengthSquared() const
{
	return this->Dot(*this);
}

float Vector3::GetDistance(const Vector3& point) const
{
	return ((*this) - point).GetLength();
}

float Vector3::GetDistanceSquared(const Vector3& point) const
{
	return ((*this) - point).GetLengthSquared();
}

Vector3 Vector3::PerElementMultiply(const Vector3& rhs) const
{
	return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 Vector3::PerElementDivide(const Vector3& rhs) const
{
	return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

// void Vector3::RotateAroundAxis(const Vector3& axis, const float angle)
// {
// 	float c, s;
// 	Math::SinCos(angle, s, c);
// 	const float _1_c = 1.0f - c;
// 	const Vector3 unitAxis = axis.GetNormalized();
// 
// 	const float ux = unitAxis.x;
// 	const float uy = unitAxis.y;
// 	const float uz = unitAxis.z;
// 
// 	// Rotation matrix derived from here - http://en.wikipedia.org/wiki/Rotation_matrix
// 	// Optimization: Use a 3x3 matrix when rotating a Vector3 instead of a 4x4
// 	Matrix m = Matrix(
// 		c + (ux * ux * _1_c),
// 		(ux * uy * _1_c) - (uz * s),
// 		(ux * uz * _1_c) + (uy * s),
// 		0.0f,
// 
// 		(uy * ux * _1_c) + (uz * s),
// 		c + (uy * uy * _1_c),
// 		(uy * uz * _1_c) - (ux * s),
// 		0.0f,
// 
// 		(uz * ux * _1_c) - (uy * s),
// 		(uz * uy * _1_c) + (ux * s),
// 		c + (uz * uz * _1_c),
// 		0.0f,
// 
// 		0.0f, 0.0f, 0.0f, 1.0f
// 	);
// 
// 	Vector3 result = (m * Vector4(x, y, z, 1.0f)).ToVector3();
// 	(*this) = result;
// }

void Vector3::RotateAroundXAxis(const float angle)
{
	float cos_value;
	float sin_value;
	Math::SinCos(angle, sin_value, cos_value);

	const float yf = y * cos_value - z * sin_value;
	const float zf = y * sin_value + z * cos_value;

	y = yf;
	z = zf;
}

void Vector3::RotateAroundYAxis(const float angle)
{
	float cos_value;
	float sin_value;
	Math::SinCos(angle, sin_value, cos_value);

	const float xf = x * cos_value + z * sin_value;
	const float zf = x * -sin_value + z * cos_value;

	x = xf;
	z = zf;
}

void Vector3::RotateAroundZAxis(const float angle)
{
	float cos_value;
	float sin_value;
	Math::SinCos(angle, sin_value, cos_value);

	const float xf = x * cos_value - y * sin_value;
	const float yf = x * sin_value + y * cos_value;

	x = xf;
	y = yf;
}

void Vector3::RotateYawPitchRoll(const float yaw, const float pitch, const float roll)
{
	// TODO: Make sure this function acts as expected
	RotateAroundYAxis(roll);
	RotateAroundXAxis(pitch);
	RotateAroundZAxis(yaw);
}

Vector3 Vector3::operator - () const
{
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator + (const Vector3& rhs) const
{
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

void Vector3::operator += (const Vector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}


Vector3 Vector3::operator - (const Vector3& rhs) const
{
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

void Vector3::operator -= (const Vector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, const float t)
{
	return start + (end - start) * t;
}

//-----------------------------------------------------------------------------
// Vector4
//-----------------------------------------------------------------------------
const Vector4 Vector4::Zero(0, 0, 0, 0);
const Vector4 Vector4::One(1, 1, 1, 1);
const Vector4 Vector4::UnitX(1, 0, 0, 0);
const Vector4 Vector4::UnitY(0, 1, 0, 0);
const Vector4 Vector4::UnitZ(0, 0, 1, 0);
const Vector4 Vector4::UnitW(0, 0, 0, 1);

void Vector4::Add(const Vector4& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
}

void Vector4::Subtract(const Vector4& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
}

void Vector4::NormalizeInPlace()
{
	const float lenSquared = this->Dot(*this);
	const float invLength = Math::InvSqrt(lenSquared);
	(*this) *= invLength;
}

Vector4 Vector4::GetNormalized() const
{
	const float lenSquared = this->Dot(*this);
	const float invLengh = Math::InvSqrt(lenSquared);
	return (*this) * invLengh;
}

Vector4 Vector4::PerElementMultiply(const Vector4& rhs) const
{
	return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
}

Vector4 Vector4::PerElementDivide(const Vector4& rhs) const
{
	return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
}

Vector4 Vector4::operator + (const Vector4& rhs) const
{
	return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

void Vector4::operator += (const Vector4& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
}

Vector4 Vector4::operator - (const Vector4& rhs) const
{
	return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

void Vector4::operator -= (const Vector4& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
}

//-----------------------------------------------------------------------------
// Point2
//-----------------------------------------------------------------------------
const Point2 Point2::Zero(0, 0);
const Point2 Point2::One(1, 1);
const Point2 Point2::UnitX(1, 0);
const Point2 Point2::UnitY(0, 1);
