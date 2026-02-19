/*********************************************************************
 * \file		Serializer.hpp
 * \brief		Handles all serialization with rapidJson
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		17 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <string>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

using Entity = uint32_t;

/**
 \brief Forward declarations for component structures.
*/
struct EngineConfig;
struct Name;
struct Transform;
struct Renderer;
struct Animation;
struct AudioSource;
struct AABBCollider2D;
struct ScriptComponent;
struct UI;
struct VideoPlayer;
struct Textbox;
struct Camera;

/**
 \class Serializer
 \brief Manages serialization and deserialization of scenes, prefabs, and engine configurations.

 This class provides utility functions for saving and loading scenes, prefabs, and engine configurations
 to and from files in JSON format using RapidJSON. It also provides functionality to reload a scene.
*/
class Serializer {
public:
    /**
     \brief Gets the singleton instance of the Serializer.
     \return Reference to the Serializer instance.
    */
    static Serializer& GetInstance();

    /**
     \brief Serializes the current scene to a JSON file.
     \param scenePath The path to save the serialized scene.
    */
    void SerializeScene(const std::string& scenePath);

    /**
     \brief Deserializes a scene from a JSON file.
     \param scenePath The path of the scene file to load.
    */
    void DeserializeScene(const std::string& scenePath);

    /**
     \brief Reloads a scene by deserializing it from the provided file path.
     \param scenePath The path of the scene file to reload.
    */
    void ReloadScene(const std::string& scenePath);

    /**
     \brief Serializes an entity into a prefab JSON file.
     \param prefabPath The path to save the prefab.
     \param entity The entity to serialize as a prefab.
    */
    void SerializePrefab(const std::string& prefabPath, Entity entity);

    /**
     \brief Deserializes an entity from a prefab JSON file.
     \param prefabPath The path of the prefab file to load.
     \return The deserialized entity.
    */
    Entity DeserializePrefab(const std::string& prefabPath);

    /**
     \brief Loads the engine configuration from a file.
     \param config The reference to an EngineConfig object to populate.
    */
    void LoadEngineConfig(EngineConfig& config);

private:

    /**
     \brief Deserializes a Name component from a JSON object.
     \param name The Name object to populate.
     \param value The JSON object containing the Name data.
    */
    void DeserializeName(Name& name, const rapidjson::Value& value);

    /**
     \brief Deserializes a Transform component from a JSON object.
     \param transform The Transform object to populate.
     \param value The JSON object containing the Transform data.
    */
    void DeserializeTransform(Transform& transform, const rapidjson::Value& value);

    /**
     \brief Deserializes a Renderer component from a JSON object.
     \param renderer The Renderer object to populate.
     \param value The JSON object containing the Renderer data.
    */
    void DeserializeRenderer(Renderer& renderer, const rapidjson::Value& value);

    /**
     \brief Deserializes an Animation component from a JSON object.
     \param animation The Animation object to populate.
     \param value The JSON object containing the Animation data.
    */
    void DeserializeAnimation(Animation& animation, const rapidjson::Value& value);

    /**
     \brief Deserializes an AudioSource component from a JSON object.
     \param audioSource The AudioSource object to populate.
     \param value The JSON object containing the AudioSource data.
    */
    void DeserializeAudioSource(AudioSource& audioSource, const rapidjson::Value& value);

    /**
     \brief Deserializes an AABBCollider2D component from a JSON object.
     \param collider The AABBCollider2D object to populate.
     \param value The JSON object containing the collider data.
    */
    void DeserializeAABBCollider2D(AABBCollider2D& collider, const rapidjson::Value& value);

    /**
     \brief Deserializes a ScriptComponent from a JSON object.
     \param scriptComponent The ScriptComponent object to populate.
     \param entity The entity associated with this script component.
     \param value The JSON object containing the ScriptComponent data.
    */
    void DeserializeScriptComponent(ScriptComponent& scriptComponent, const Entity& entity, const rapidjson::Value& value);

    void DeserializeUI(UI& ui, const rapidjson::Value& value);
    void DeserializeVideoPlayer(VideoPlayer& vp, const rapidjson::Value& value);
    void DeserializeCamera(Camera& cam, const rapidjson::Value& value);
    void DeserializeTextbox(Textbox& ui, const rapidjson::Value& value);

    /**
     \brief Serializes a Name component into a JSON object.
     \param name The Name object to serialize.
     \param value The JSON object to populate with Name data.
     \param allocator The RapidJSON allocator for memory management.
    */
    void SerializeName(const Name& name, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);

    /**
     \brief Serializes a Transform component into a JSON object.
     \param transform The Transform object to serialize.
     \param value The JSON object to populate with Transform data.
     \param allocator The RapidJSON allocator for memory management.
     \param isPrefab Flag indicating whether the serialization is for a prefab.
    */
    void SerializeTransform(const Transform& transform, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator, bool isPrefab = false);

    /**
     \brief Serializes an Animation component into a JSON object.
     \param animation The Animation object to serialize.
     \param value The JSON object to populate with Animation data.
     \param allocator The RapidJSON allocator for memory management.
    */
    void SerializeAnimation(const Animation& animation, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);

    /**
     \brief Serializes an AudioSource component into a JSON object.
     \param audioSource The AudioSource object to serialize.
     \param value The JSON object to populate with AudioSource data.
     \param allocator The RapidJSON allocator for memory management.
    */
    void SerializeAudioSource(const AudioSource& audioSource, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);

    /**
     \brief Serializes a ScriptComponent into a JSON object.
     \param scriptComponent The ScriptComponent object to serialize.
     \param entity The entity associated with this script component.
     \param value The JSON object to populate with ScriptComponent data.
     \param allocator The RapidJSON allocator for memory management.
    */
    void SerializeScriptComponent(const ScriptComponent& scriptComponent, const Entity& entity, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);

    void SerializeUI(const UI& ui, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);
    void SerializeVideoPlayer(const VideoPlayer& vp, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);
    void SerializeCamera(const Camera& cam, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);
    void SerializeTextbox(const Textbox& ui, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);
};


#endif // !SERIALIZER_HPP