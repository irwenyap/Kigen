/*****************************************************************//**
 * \file   Vec2.hpp
 * \brief
 *
 * \author irwen
 * \date   April 2024
 *********************************************************************/

#ifndef VEC_2_HPP
#define VEC_2_HPP

#include <ostream>
#include <cmath>

struct Vec2 {
	float x, y;

	Vec2(float a = 0, float b = 0);
	Vec2(const Vec2& rhs) = default;
	~Vec2() = default;



	Vec2 operator+(const Vec2& rhs) const;
	Vec2& operator+=(const Vec2& rhs);

	Vec2 operator-(const Vec2& rhs) const;
	Vec2 operator-=(const Vec2& rhs);

	Vec2 operator-() const;

	Vec2 operator*(float scalar) const;
	Vec2& operator*=(float scalar);

	Vec2 operator/(float scalar) const;
	Vec2& operator/=(float scalar);

	bool operator==(const Vec2& rhs) const;
	bool operator!=(const Vec2& rhs) const;

	Vec2& operator=(const Vec2& rhs);

	float Length() const;
	float LengthSquared() const;

	float Dot(const Vec2& rhs) const;
	float Cross(const Vec2& rhs) const;

	Vec2 Normalized() const; // Maybe add throw divide by zero error

	Vec2& Normalize(); // maybe add throw divide by zero error

	friend std::ostream& operator<<(std::ostream& os, const Vec2& rhs);
	friend Vec2 operator*(float scalar, const Vec2& rhs);
	friend Vec2 operator/(float scalar, const Vec2& rhs);
};

inline Vec2::Vec2(float a, float b) : x(a), y(b)
{
}

inline Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

inline Vec2& Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

inline Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

inline Vec2 Vec2::operator-=(const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

inline Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
}

inline Vec2 Vec2::operator*(float scalar) const
{
	return Vec2(scalar * x, scalar * y);
}

inline Vec2& Vec2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

inline Vec2 Vec2::operator/(float scalar) const
{
	return Vec2(x / scalar, y / scalar);
}

inline Vec2& Vec2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}

inline bool Vec2::operator==(const Vec2& rhs) const
{
	return (x == rhs.x) && (y == rhs.y);
}

inline bool Vec2::operator!=(const Vec2& rhs) const
{
	return !(*this == rhs);
}

inline Vec2& Vec2::operator=(const Vec2& rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

inline float Vec2::Length() const
{
	return sqrt(x * x + y * y);
}

inline float Vec2::LengthSquared() const
{
	return x * x + y * y;
}

inline float Vec2::Dot(const Vec2& rhs) const
{
	return x * rhs.x + y * rhs.y;
}

/**
 * .
 * 
 * This scalar can be thought of as the magnitude of the cross product if the 
 * 2D vectors were extended into 3D space by adding a zero as the third component. 
 * This scalar is useful for determining the orientation of a pair of vectors 
 * (which can tell you if they form a left turn or a right turn) and can be used 
 * to calculate the area of the parallelogram formed by the two vectors.
 * 
 * \param rhs
 * \return 
 * Sign of result indicates relative orientation of vectors:
 * - Positive:	b is counter-clockwise from a
 * - Negative:	b is clockwise from a
 * - Zero:		a and b are collinear (lie on a straight line)
 * Magnitude of result is the area of parallelogram formed by the vectors if they are positioned tail-to-head
 */
inline float Vec2::Cross(const Vec2& rhs) const
{
	return x * rhs.y - y * rhs.x;
}

inline Vec2 Vec2::Normalized() const
{
	float d = Length();
	if (d <= FLT_EPSILON && -d <= FLT_EPSILON) {
		return *this;
	}
	return Vec2(*this / d);
}

inline Vec2& Vec2::Normalize()
{
	float d = Length();
	if (d <= FLT_EPSILON && -d <= FLT_EPSILON) {
		return *this;
	}
	*this /= d;
	return *this;
}

inline std::ostream& operator<<(std::ostream& os, const Vec2& rhs) {
	os << "[ " << rhs.x << ", " << rhs.y << ", " << " ]";
	return os;
}

inline Vec2 operator*(float scalar, const Vec2& rhs)
{
	return rhs * scalar;
}

inline Vec2 operator/(float scalar, const Vec2& rhs)
{
	return rhs / scalar;
}

#endif // !VEC_3_HPP