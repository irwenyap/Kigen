/*********************************************************************
 * \file		JSONParser.cpp
 * \brief		Scene to showcase de-serialization as well as
 *				a guide on how to use the ECS system
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email		y.ziyangirwen@digipen.edu
 * \date		8 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "JSONParser.hpp"
#include "Vec.hpp"


namespace JSONSerializer {
    void IntToJSON(int& value, rapidjson::Document::AllocatorType&, rapidjson::Value& outValue) {
        outValue.SetInt(value);
    }

    void FloatToJSON(float& value, rapidjson::Document::AllocatorType&, rapidjson::Value& outValue) {
        outValue.SetFloat(value);
    }

    void StringToJSON(const std::string& value, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue) {
        outValue.SetString(value.c_str(), allocator);
    }

    void Vec3ToJSON(const Vec3& vec, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue) {
        outValue.SetObject();
        outValue.AddMember("x", vec.x, allocator);
        outValue.AddMember("y", vec.y, allocator);
        outValue.AddMember("z", vec.z, allocator);
    }

    void Vec2ToJSON(const Vec2& vec, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue) {
        outValue.SetObject();
        outValue.AddMember("x", vec.x, allocator);
        outValue.AddMember("y", vec.y, allocator);
    }
}

namespace JSONDeserializer {

    int JSONToInt(const rapidjson::Value& value, const char* fieldName)
    {
        if (!value.HasMember(fieldName)) {
            //throw std::runtime_error(std::string("Field not found: ") + fieldName);
        }
        if (!value[fieldName].IsInt()) {
            //throw std::runtime_error(std::string("Field is not an integer: ") + fieldName);
        }
        return value[fieldName].GetInt();
    }

    uint32_t JSONtoUInt32(const rapidjson::Value& value, const char* fieldName)
    {
        if (!value.HasMember(fieldName)) {
            //throw std::runtime_error(std::string("Field not found: ") + fieldName);
            return 0;
        }
        if (!value[fieldName].IsUint()) {
            //throw std::runtime_error(std::string("Field is not an integer: ") + fieldName);
        }
        return value[fieldName].GetUint();
    }

    float JSONToFloat(const rapidjson::Value& value, const char* fieldName)
    {
        if (!value.HasMember(fieldName)) {
            //throw std::runtime_error(std::string("Field not found: ") + fieldName);
            return 0.f;
        }
        if (!value[fieldName].IsFloat() && !value[fieldName].IsDouble()) {
            throw std::runtime_error(std::string("Field is not a float: ") + fieldName);
        }
        return value[fieldName].GetFloat();
    }

    bool JSONToBool(const rapidjson::Value& value, const char* fieldName)
    {
        if (!value.HasMember(fieldName)) {
            //throw std::runtime_error(std::string("Field not found: ") + fieldName);
            return false;
        }
        if (!value[fieldName].IsBool()) {
            throw std::runtime_error(std::string("Field is not a boolean: ") + fieldName);
        }
        return value[fieldName].GetBool();
    }

    std::string JSONToString(const rapidjson::Value& value, const char* fieldName) {
        if (!value.HasMember(fieldName) || !value[fieldName].IsString()) {
            //throw std::runtime_error(std::string("Invalid or missing Vec3 field: ") + fieldName);
            return "";
        }

        const auto& charValue = value[fieldName];
        return charValue.GetString();
    }

    Vec3 JSONToVec3(const rapidjson::Value& value, const char* fieldName)
    {
        if (!value.HasMember(fieldName) || !value[fieldName].IsObject()) {
            //throw std::runtime_error(std::string("Invalid or missing Vec3 field: ") + fieldName);
            return Vec3(0.f, 0.f, 0.f);
        }

        const auto& vecValue = value[fieldName];
        Vec3 temp;
        temp.x = JSONToFloat(vecValue, "x");
        temp.y = JSONToFloat(vecValue, "y");
        temp.z = JSONToFloat(vecValue, "z");
        return temp;
        //return Vec3(vecValue["x"].GetFloat(), vecValue["y"].GetFloat(), vecValue["z"].GetFloat());
    }

    Vec2 JSONToVec2(const rapidjson::Value& value, const char* fieldName)
    {
        if (!value.HasMember(fieldName) || !value[fieldName].IsObject()) {
            //throw std::runtime_error(std::string("Invalid or missing Vec3 field: ") + fieldName);
            return Vec2(0.f, 0.f);
        }

        const auto& vecValue = value[fieldName];
        return Vec2(vecValue["x"].GetFloat(), vecValue["y"].GetFloat());
    }

}
