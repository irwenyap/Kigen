/*********************************************************************
 * \file	Transform.hpp
 * \brief	Defines the base Transform component
  *
  * \author	t.yongchin (2301359), y.ziyangirwen (2301345)
  * \email	t.yongchin@digipen.edu, y.ziyangirwen@digipen.edu
  * \date	01 November 2024
  *
  * Copyright(C) 2024 DigiPen Institute of Technology.
  * Reproduction or disclosure of this file or its contents without the
  * prior written consent of DigiPen Institute of Technology is prohibited.
  *********************************************************************/
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <vector>
#include "Math.hpp"
#include "../Utility/ReflectionMacros.hpp"

//struct Mat4;

  /*********************************************************************
  * \struct	Transform
  * \brief	Defines the Transform component that is used to store the
  *			position, scale, rotation and the model-to-world matrix.
  *********************************************************************/
struct Transform {

	/*
	* \brief	Constructor for the Transform component
	* \param	position	Position of the object
	* \param	scale		Scale of the object
	* \param	rotation	Rotation of the object
	*/
	Transform(
		Vec3 position = Vec3(0.0f, 0.f, 0.f), 
		Vec3 scale = Vec3(1000.f, 1000.f, 1.f), 
		Vec3 rotation = Vec3(0.0f, 0.f, 0.f)
	);

	uint32_t uuid;
	uint32_t parentUUID = 0;

	//Transform* parent = nullptr;
	uint32_t parent = MAX_ENTITIES;
	std::vector<uint32_t> children = {};

	Vec3 position;
	Vec3 scale;
	Vec3 rotation;

	Vec3 localPosition;
	Vec3 localScale;
	Vec3 localRotation;

	// Model-to-world matrix
	Mat4 modelToWorldMtx;
	Mat4 localTransformMtx;

	bool updated;

	REFLECTABLE(Transform,
		FIELD(position),
		FIELD(scale),
		FIELD(rotation),
		FIELD(localPosition),
		FIELD(localScale),
		FIELD(localRotation)
	)
};

inline Transform::Transform(Vec3 position, Vec3 scale, Vec3 rotation) :
	position(position),
	scale(scale),
	rotation(rotation),
	localPosition(position),
	localScale(scale),
	localRotation(rotation),
	updated(true),
	uuid(0)
{
}


#endif // !TRANSFORM_HPP
