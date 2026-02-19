/*********************************************************************
 * \file	ScriptComponent.hpp
 * \brief	Defines the Script component.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	27 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#ifndef SCRIPT_COMP
#define SCRIPT_COMP

#include <string>
#include <unordered_map>
#include <variant>

struct ScriptComponent
{
	std::string className;
	//using ParameterType = std::variant<float, double, bool, uint16_t, uint32_t, uint64_t, int16_t, int32_t, int64_t>;
	//std::unordered_map<std::string, ParameterType> parameters;

	ScriptComponent() = default;
	ScriptComponent(const ScriptComponent&) = default;
};

#endif // !SCRIPT_COMP


