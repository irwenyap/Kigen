/*****************************************************************//**
 * \file	Vec.hpp
 * \brief	Added to handle implicit conversions between Vec2, Vec3, and Vec4
 *
 * \author	irwinjun.l, 2301305
 * \email	irwinjun.l@digipen.edu
 * \date	29 September 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"


inline Vec2::Vec2(const Vec3& v) : x(v.x), y(v.y)
{
}

inline Vec2::Vec2(const Vec4& v) : x(v.x), y(v.y)
{
}

inline Vec2::operator Vec3() const
{
	return Vec3(x, y, 1.0f);
}

inline Vec2::operator Vec4() const
{
	return Vec4(x, y, 0.0f, 1.0f);
}

inline Vec3::Vec3(const Vec2& v) : x(v.x), y(v.y), z(0.0f)
{
}

inline Vec3::Vec3(const Vec2& v, float c) : x(v.x), y(v.y), z(c)
{
}

inline Vec3::Vec3(const Vec4& v) : x(v.x), y(v.y), z(v.z)
{
}

inline Vec3::operator Vec2() const
{
	return Vec2(x, y);
}

inline Vec3::operator Vec4() const
{
	return Vec4(x, y, z, 1.0f);
}

inline Vec4::Vec4(const Vec2& v) : x(v.x), y(v.y), z(0.0f), w(1.0f)
{
}

inline Vec4::Vec4(const Vec2& v, float c, float d) : x(v.x), y(v.y), z(c), w(d)
{
}

inline Vec4::Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(1.0f)
{
}

inline Vec4::Vec4(const Vec3& v, float d) : x(v.x), y(v.y), z(v.z), w(d)
{
}

inline Vec4::operator Vec2() const
{
	return Vec2(x, y);
}

inline Vec4::operator Vec3() const
{
	return Vec3(x, y, z);
}