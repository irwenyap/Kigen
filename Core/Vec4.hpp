/*****************************************************************//**
 * \file	Vec4.hpp
 * \brief	Vec4 supports unnamed unions for both (x, y, z, w) and color components (r, g, b, a). The class provides operators for vector arithmetic, 
            and it can convert to Vec2 and Vec3. This file also introduces a type alias Color, which is essentially a Vec4​.
 *
 * \author	irwinjun.l, 2301305
 * \email	irwinjun.l@digipen.edu
 * \date	29 September 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#ifndef VEC_4_HPP
#define VEC_4_HPP

#pragma warning(disable : 4201) // Disable warning C4201 for unnamed struct/union

struct Vec2;
struct Vec3;

struct Vec4 {
    union {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
    };
    Vec4(float a = 0, float b = 0, float c = 0, float d = 0);
    Vec4(const Vec4& rhs) = default;
    Vec4(const Vec2& v);
    Vec4(const Vec2& v, float c, float d);
    Vec4(const Vec3& v);
    Vec4(const Vec3& v, float d);
    ~Vec4() = default;

    operator Vec2() const;
    operator Vec3() const;
    inline float& operator[](unsigned int i);
    inline const float& operator[](unsigned int i) const;

    Vec4 operator+(const Vec4& rhs) const;
    Vec4& operator+=(const Vec4& rhs);

    Vec4 operator-(const Vec4& rhs) const;
    Vec4& operator-=(const Vec4& rhs);

    Vec4 operator*(float scalar) const;
    Vec4& operator*=(float scalar);

    Vec4 operator/(float scalar) const;
    Vec4& operator/=(float scalar);
};

using Color = Vec4;


inline Vec4::Vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d)
{
}

inline float& Vec4::operator[](unsigned int i) {
	if (i == 0) return x;
	else if (i == 1) return y;
	else if (i == 2) return z;
	else return w;
}

inline const float& Vec4::operator[](unsigned int i) const {
    if (i == 0) return x;
    else if (i == 1) return y;
    else if (i == 2) return z;
    else return w;
}

inline Vec4 Vec4::operator+(const Vec4& rhs) const {
	return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

inline Vec4& Vec4::operator+=(const Vec4& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

inline Vec4 Vec4::operator-(const Vec4& rhs) const {
	return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

inline Vec4& Vec4::operator-=(const Vec4& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

inline Vec4 Vec4::operator*(float scalar) const {
	return Vec4(scalar * x, scalar * y, scalar * z, scalar * w);
}

inline Vec4& Vec4::operator*=(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

inline Vec4 Vec4::operator/(float scalar) const {
	return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
}

inline Vec4& Vec4::operator/=(float scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}

#endif // !VEC_4_HPP
