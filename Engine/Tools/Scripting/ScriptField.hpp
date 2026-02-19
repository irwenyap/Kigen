/*********************************************************************
 * \file	ScriptField.hpp
 * \brief	Defines ScriptFieldType to help with type management between C# and C++.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	27 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#ifndef SCRIPT_FIELD_HPP
#define SCRIPT_FIELD_HPP

#include <string>

typedef struct _MonoClassField MonoClassField;

enum class ScriptFieldType {
	None = 0,
	Float, Double,
	Bool, Char, Byte, Short, Int, Long,
	UByte, UShort, UInt, ULong,
	Vector2, Vector3, Vector4,
	Entity
};

struct ScriptField {
	ScriptFieldType Type;
	std::string Name;

	MonoClassField* ClassField{};
};


#endif // !SCRIPT_FIELD_HPP
