/*****************************************************************//**
 * \file   Vec3.hpp
 * \brief  
 * 
 * \author irwen
 * \date   April 2024
 *********************************************************************/

#ifndef VEC_3_HPP
#define VEC_3_HPP

#include <ostream>
#include <cmath>

struct Vec3 {
	float x, y, z;

	Vec3(float a = 0, float b = 0, float c = 0);
	Vec3(const Vec3& rhs) = default;
	~Vec3() = default;

	Vec3 operator+(const Vec3& rhs) const;
	Vec3& operator+=(const Vec3& rhs);

	Vec3 operator-(const Vec3& rhs) const;
	Vec3 operator-=(const Vec3& rhs);

	Vec3 operator-() const;

	Vec3 operator*(float scalar) const;
	Vec3& operator*=(float scalar);

	Vec3 operator/(float scalar) const;
	Vec3& operator/=(float scalar);

	bool operator==(const Vec3& rhs) const;
	bool operator!=(const Vec3& rhs) const;

	Vec3& operator=(const Vec3& rhs);

	float Length() const;
	float LengthSquared() const;

	float Dot(const Vec3& rhs) const;
	Vec3 Cross(const Vec3& rhs) const;

	Vec3 Normalized() const; // Maybe add throw divide by zero error

	Vec3& Normalize(); // maybe add throw divide by zero error

	friend std::ostream& operator<<(std::ostream& os, const Vec3& rhs);
	friend Vec3 operator*(float scalar, const Vec3& rhs);
	friend Vec3 operator/(float scalar, const Vec3& rhs);
};

inline Vec3::Vec3(float a, float b, float c) : x(a), y(b), z(c)
{
}

inline Vec3 Vec3::operator+(const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vec3& Vec3::operator+=(const Vec3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

inline Vec3 Vec3::operator-(const Vec3& rhs) const
{
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vec3 Vec3::operator-=(const Vec3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

inline Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

inline Vec3 Vec3::operator*(float scalar) const
{
	return Vec3(scalar * x, scalar * y, scalar * z);
}

inline Vec3& Vec3::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

inline Vec3 Vec3::operator/(float scalar) const
{
	return Vec3(x / scalar, y / scalar, z / scalar);
}

inline Vec3& Vec3::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

inline bool Vec3::operator==(const Vec3& rhs) const
{
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vec3::operator!=(const Vec3& rhs) const
{
	return !(*this == rhs);
}

inline Vec3& Vec3::operator=(const Vec3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

inline float Vec3::Length() const
{
	return sqrt(x * x + y * y + z * z);
}

inline float Vec3::LengthSquared() const
{
	return x * x + y * y + z * z;
}

inline float Vec3::Dot(const Vec3& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

inline Vec3 Vec3::Cross(const Vec3& rhs) const
{
	return Vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

inline Vec3 Vec3::Normalized() const
{
	float d = Length();
	if (d <= FLT_EPSILON && -d <= FLT_EPSILON) {
		return *this;
	}
	return Vec3(*this / d);
}

inline Vec3& Vec3::Normalize()
{
	float d = Length();
	if (d <= FLT_EPSILON && -d <= FLT_EPSILON) {
		return *this;
	}
	*this /= d;
	return *this;
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& rhs) {
	os << "[ " << rhs.x << ", " << rhs.y << ", " << rhs.z << " ]";
	return os;
}

inline Vec3 operator*(float scalar, const Vec3& rhs)
{
	return rhs * scalar;
}

inline Vec3 operator/(float scalar, const Vec3& rhs)
{
	return rhs / scalar;
}

#endif // !VEC_3_HPP