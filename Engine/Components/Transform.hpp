#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Vec3.hpp"

struct Transform {
	Vec3 position;
	Vec3 scale;
	Vec3 rotation;

	Vec3 localPosition;
	Vec3 localScale;
	Vec3 localRotation;
};

#endif // !TRANSFORM_HPP
