/*********************************************************************
 * \file		JSONParser.hpp
 * \brief		Scene to showcase de-serialization as well as
 *				a guide on how to use the ECS system
 *
 * \author		Yap Zi Yang Irwen, y.ziyangirwen, 2301345
 * \email		y.ziyangirwen@digipen.edu
 * \date		8 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its contents without the
 *				prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <rapidjson/document.h>
#include <string>

struct Vec2;
struct Vec3;

/// @namespace JSONSerializer
/// @brief Namespace containing functions for serializing various data types to JSON format.
namespace JSONSerializer {

    /**
     * @brief Serializes an integer to JSON.
     * @param[in] value The integer to serialize.
     * @param[in] allocator The allocator for the JSON document.
     * @param[out] outValue The JSON value where the integer will be stored.
     */
    void IntToJSON(int& value, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue);

    /**
     * @brief Serializes a float to JSON.
     * @param[in] value The float to serialize.
     * @param[in] allocator The allocator for the JSON document.
     * @param[out] outValue The JSON value where the float will be stored.
     */
    void FloatToJSON(float& value, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue);

    /**
     * @brief Serializes a string to JSON.
     * @param[in] value The string to serialize.
     * @param[in] allocator The allocator for the JSON document.
     * @param[out] outValue The JSON value where the string will be stored.
     */
    void StringToJSON(const std::string& value, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue);

    /**
     * @brief Serializes a 3D vector (Vec3) to JSON.
     * @param[in] vec The Vec3 object to serialize.
     * @param[in] allocator The allocator for the JSON document.
     * @param[out] outValue The JSON value where the Vec3 will be stored.
     */
    void Vec3ToJSON(const Vec3& vec, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue);

    /**
     * @brief Serializes a 2D vector (Vec2) to JSON.
     * @param[in] vec The Vec2 object to serialize.
     * @param[in] allocator The allocator for the JSON document.
     * @param[out] outValue The JSON value where the Vec2 will be stored.
     */
    void Vec2ToJSON(const Vec2& vec, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& outValue);
}

/// @namespace JSONDeserializer
/// @brief Namespace containing functions for deserializing various data types from JSON format.
namespace JSONDeserializer {

    /**
     * @brief Deserializes an integer from a JSON value.
     * @param[in] value The JSON value containing the integer.
     * @param[in] fieldName The field name for error reporting.
     * @return The deserialized integer.
     */
    int JSONToInt(const rapidjson::Value& value, const char* fieldName);

    uint32_t JSONtoUInt32(const rapidjson::Value& value, const char* fieldName);

    /**
     * @brief Deserializes a float from a JSON value.
     * @param[in] value The JSON value containing the float.
     * @param[in] fieldName The field name for error reporting.
     * @return The deserialized float.
     */
    float JSONToFloat(const rapidjson::Value& value, const char* fieldName);

    /**
     * @brief Deserializes a boolean from a JSON value.
     * @param[in] value The JSON value containing the boolean.
     * @param[in] fieldName The field name for error reporting.
     * @return The deserialized boolean.
     */
    bool JSONToBool(const rapidjson::Value& value, const char* fieldName);

    /**
     * @brief Deserializes a string from a JSON value.
     * @param[in] value The JSON value containing the string.
     * @param[in] fieldName The field name for error reporting.
     * @return The deserialized string.
     */
    std::string JSONToString(const rapidjson::Value& value, const char* fieldName);

    /**
     * @brief Deserializes a 3D vector (Vec3) from a JSON value.
     * @param[in] value The JSON value containing the Vec3 data.
     * @param[in] fieldName The field name for error reporting.
     * @return The deserialized Vec3.
     */
    Vec3 JSONToVec3(const rapidjson::Value& value, const char* fieldName);

    /**
     * @brief Deserializes a 2D vector (Vec2) from a JSON value.
     * @param[in] value The JSON value containing the Vec2 data.
     * @param[in] fieldName The field name for error reporting.
     * @return The deserialized Vec2.
     */
    Vec2 JSONToVec2(const rapidjson::Value& value, const char* fieldName);
}


#endif // !JSON_PARSER_HPP