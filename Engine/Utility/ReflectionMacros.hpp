/*********************************************************************
 * \file		ReflectionMacros.hpp
 * \brief		Future use to reduce boilerplate code
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		1 September 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef REFLECTION_MACROS_HPP
#define REFLECTION_MACROS_HPP

#include <tuple>
#include <type_traits>
#include "../ECS/ECSManager.hpp"

//extern ECSManager g_ecsManager;

#define REFLECTABLE(name, ...) \
	constexpr const char* ComponentName() { return #name; } \
	constexpr auto Reflect() { return std::make_tuple(__VA_ARGS__); }

#define FIELD(name) std::make_pair(#name, &std::remove_reference_t<decltype(*this)>::name)

#endif // !REFLECTION_MACROS_HPP
