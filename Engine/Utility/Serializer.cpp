/*********************************************************************
 * \file		Serializer.cpp
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
#include "Serializer.hpp"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>

#include "../Components/Name.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Renderer.hpp"
#include "../Components/Collider2D.hpp"
#include "../Components/Rigidbody2D.hpp"
#include "../Components/Animation.hpp"
#include "../Components/AudioSource.hpp"
#include "../Components/ScriptComponent.hpp"
#include "../Components/UI.hpp"
#include "../Components/Textbox.hpp"
#include "../Components/VideoPlayer.hpp"

#include "JSONParser.hpp"
#include "ComponentIDGenerator.hpp"
#include "MetadataHandler.hpp"
#include "../Tools/PrefabManager.hpp"

#include "./EngineConfig.hpp"
#include "../ECS/ECSManager.hpp"

#include "../Tools/EditorPanel.hpp"

#include "../Engine/Tools/Scripting/ScriptEngine.hpp"
#include "../Components/Camera.hpp"
#include "../Scene/SceneManager.hpp"
#include "../Layers/LayerManager.hpp"

Serializer& Serializer::GetInstance()
{
	static Serializer serializer;
	return serializer;
}

void Serializer::SerializeScene(const std::string& scenePath) {
#ifndef INSTALLER
	rapidjson::Document document;
	document.SetObject();
	auto& allocator = document.GetAllocator();

	rapidjson::Value entitiesArray(rapidjson::kArrayType);

	for (auto& entt : EditorPanel::sceneEntities) {
		Entity entity = entt.id;

		rapidjson::Value entityData(rapidjson::kObjectType);

		// Active, tags and layers
		entityData.AddMember("Active", ECSManager::GetInstance().GetEntityManager().GetActive(entity), allocator);
		entityData.AddMember("Tag", "", allocator);
		entityData.AddMember("Layer", ECSManager::GetInstance().GetEntityManager().GetLayer(entity), allocator);

		rapidjson::Value componentsData(rapidjson::kObjectType);

		if (ECSManager::GetInstance().TryGetComponent<Name>(entity)) {
			//const auto& nameComponent = ECSManager::GetInstance().GetComponent<Name>(entity);
			rapidjson::Value nameData(rapidjson::kObjectType);
			SerializeName(ECSManager::GetInstance().GetComponent<Name>(entity), nameData, allocator);
			componentsData.AddMember("Name", nameData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<Transform>(entity)) {
			rapidjson::Value transformData(rapidjson::kObjectType);
			SerializeTransform(ECSManager::GetInstance().GetComponent<Transform>(entity), transformData, allocator);
			componentsData.AddMember("Transform", transformData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<Renderer>(entity)) {
			const auto& rendererComponent = ECSManager::GetInstance().GetComponent<Renderer>(entity);
			rapidjson::Value rendererData(rapidjson::kObjectType);

			rendererData.AddMember("mesh", rendererComponent.mesh, allocator);
			rendererData.AddMember("isAnimated", rendererComponent.isAnimated, allocator);
			rendererData.AddMember("sortingLayer", rendererComponent.sortingLayer, allocator);

			rapidjson::Value uuidStr(rapidjson::kStringType);
			JSONSerializer::StringToJSON(rendererComponent.uuid, allocator, uuidStr);
			rendererData.AddMember("textureFile", uuidStr, allocator);

			componentsData.AddMember("Renderer", rendererData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity)) {
			const auto& colliderComponent = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity);
			rapidjson::Value colliderData(rapidjson::kObjectType);

			colliderData.AddMember("bounciness", colliderComponent.bounciness, allocator);

			rapidjson::Value minColData(rapidjson::kObjectType);
			JSONSerializer::Vec2ToJSON(colliderComponent.min, allocator, minColData);
			colliderData.AddMember("min", minColData, allocator);

			rapidjson::Value maxColData(rapidjson::kObjectType);
			JSONSerializer::Vec2ToJSON(colliderComponent.max, allocator, maxColData);
			colliderData.AddMember("max", maxColData, allocator);

			colliderData.AddMember("isTrigger", colliderComponent.isTrigger, allocator);

			componentsData.AddMember("AABBCollider2D", colliderData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<Rigidbody2D>(entity)) {
			const auto& rbComponent = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
			rapidjson::Value rbData(rapidjson::kObjectType);

			rbData.AddMember("mass", rbComponent.mass, allocator);
			//rbData.AddMember("inverseMass", rbComponent.mass, allocator);
			rbData.AddMember("drag", rbComponent.drag, allocator);
			rbData.AddMember("gravity", rbComponent.gravityScale, allocator);
			rbData.AddMember("static", rbComponent.isStatic, allocator);
			rbData.AddMember("kinematic", rbComponent.isKinematic, allocator);
			rbData.AddMember("grounded", rbComponent.isGrounded, allocator);

			rapidjson::Value rbPos(rapidjson::kObjectType);
			JSONSerializer::Vec2ToJSON(rbComponent.position, allocator, rbPos);
			rbData.AddMember("pos", rbPos, allocator);

			rapidjson::Value rbVel(rapidjson::kObjectType);
			JSONSerializer::Vec2ToJSON(rbComponent.velocity, allocator, rbVel);
			rbData.AddMember("vel", rbVel, allocator);

			componentsData.AddMember("Rigidbody2D", rbData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<Animation>(entity)) {
			rapidjson::Value animData(rapidjson::kObjectType);
			SerializeAnimation(ECSManager::GetInstance().GetComponent<Animation>(entity), animData, allocator);
			componentsData.AddMember("Animation", animData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<AudioSource>(entity)) {
			rapidjson::Value asData(rapidjson::kObjectType);
			SerializeAudioSource(ECSManager::GetInstance().GetComponent<AudioSource>(entity), asData, allocator);
			componentsData.AddMember("AudioSource", asData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<ScriptComponent>(entity)) {
			const auto& scriptComponent = ECSManager::GetInstance().GetComponent<ScriptComponent>(entity);
			rapidjson::Value scriptData(rapidjson::kObjectType);
			SerializeScriptComponent(scriptComponent, entity, scriptData, allocator);
			componentsData.AddMember("ScriptComponent", scriptData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<UI>(entity)) {
			const auto& uiComponent = ECSManager::GetInstance().GetComponent<UI>(entity);
			rapidjson::Value uiData(rapidjson::kObjectType);
			SerializeUI(uiComponent, uiData, allocator);
			componentsData.AddMember("UI", uiData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<VideoPlayer>(entity)) {
			rapidjson::Value vpData(rapidjson::kObjectType);
			SerializeVideoPlayer(ECSManager::GetInstance().GetComponent<VideoPlayer>(entity), vpData, allocator);
			componentsData.AddMember("VideoPlayer", vpData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<Textbox>(entity)) {
			rapidjson::Value tbData(rapidjson::kObjectType);
			SerializeTextbox(ECSManager::GetInstance().GetComponent<Textbox>(entity), tbData, allocator);
			componentsData.AddMember("Textbox", tbData, allocator);
		}

		if (ECSManager::GetInstance().TryGetComponent<Camera>(entity)) {
			rapidjson::Value cameraData(rapidjson::kObjectType);
			SerializeCamera(ECSManager::GetInstance().GetComponent<Camera>(entity), cameraData, allocator);
			componentsData.AddMember("Camera", cameraData, allocator);
		}

		// Add components to entity data
		entityData.AddMember("Components", componentsData, allocator);

		// Add entity data to the entities array
		entitiesArray.PushBack(entityData, allocator);
	}

	// Add entities array to the document
	document.AddMember("Entities", entitiesArray, allocator);

	// Add collision matrix to the document
	rapidjson::Value collisionMatrix(rapidjson::kObjectType);
	for (size_t i = 0; i < LayerManager::GetInstance().collisionMatrix.size(); ++i) {
		rapidjson::Value memberName(rapidjson::kStringType);
		memberName.SetString(std::to_string(i).c_str(), allocator);
		collisionMatrix.AddMember(memberName, LayerManager::GetInstance().collisionMatrix[i], allocator);
	}

	document.AddMember("Collision Matrix", collisionMatrix, allocator);

	// Write JSON to a file
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	std::ofstream ofs(scenePath);
	ofs << buffer.GetString();
	ofs.close();
#else
	scenePath;
#endif
}

void Serializer::DeserializeScene(const std::string& scenePath)
{
	std::ifstream ifs(scenePath);
	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	rapidjson::Document document;
	document.Parse(jsonContent.c_str());

	const auto& entities = document["Entities"];

	for (rapidjson::SizeType i = 0; i < entities.Size(); ++i) {
		const auto& entityData = entities[i];

		Entity newEntity = ECSManager::GetInstance().CreateEntity();

		// Active, tags and layers
		ECSManager::GetInstance().GetEntityManager().SetActive(newEntity, JSONDeserializer::JSONToBool(entityData, "Active"));
		JSONDeserializer::JSONToString(entityData, "Tag");
		if (entityData.HasMember("Layer"))
			ECSManager::GetInstance().GetEntityManager().SetLayer(newEntity, static_cast<Layer>(entityData["Layer"].GetInt()));
		//JSONDeserializer::JSONToUint(entityData, "Active");

		//entityData.AddMember("Active", true, allocator);
		//entityData.AddMember("Tag", "", allocator);
		//entityData.AddMember("Layer", ECSManager::GetInstance().GetEntityManager().GetLayer(entity), allocator);


		if (entityData.HasMember("Components")) {
			const auto& components = entityData["Components"];

			if (components.HasMember("Name")) {
				const auto& name = components["Name"];
				Name& n = ECSManager::GetInstance().GetComponent<Name>(newEntity);
				DeserializeName(n, name);
				if (n.prefabID != "")
					PrefabManager::GetInstance().prefabsMap[n.prefabID].push_back(newEntity);
			}
			if (components.HasMember("Transform")) {
				const auto& transformVal = components["Transform"];
				Transform& t = ECSManager::GetInstance().GetComponent<Transform>(newEntity);
				DeserializeTransform(t, transformVal);
				TransformSystem::uuidToTransformMap[t.uuid] = newEntity;
			}
			if (components.HasMember("Renderer")) {
				const auto& renderer = components["Renderer"];

				Renderer r;
				r.mesh = renderer["mesh"].GetInt();
				if (renderer.HasMember("isAnimated"))
					r.isAnimated = renderer["isAnimated"].GetBool();
				//r.currentTextureID = renderer["textureFile"].GetUint64();
				r.uuid = JSONDeserializer::JSONToString(renderer, "textureFile");
				if (renderer.HasMember("sortingLayer"))
					r.sortingLayer = static_cast<SortingLayer>(renderer["sortingLayer"].GetInt());

				ECSManager::GetInstance().AddComponent(newEntity, r);
			}
			if (components.HasMember("AABBCollider2D")) {
				const auto& collider = components["AABBCollider2D"];

				ECSManager::GetInstance().physicsSystem->AddAABBColliderComponent(
					newEntity,
					collider["bounciness"].GetFloat(),
					JSONDeserializer::JSONToVec2(collider, "min"),
					JSONDeserializer::JSONToVec2(collider, "max"),
					collider["isTrigger"].GetBool()
				);
			}
			if (components.HasMember("Rigidbody2D")) {
				const auto& rb = components["Rigidbody2D"];

				Rigidbody2D rigidbody;
				rigidbody.position = JSONDeserializer::JSONToVec2(rb, "pos");
				rigidbody.velocity = JSONDeserializer::JSONToVec2(rb, "vel");
				rigidbody.mass = JSONDeserializer::JSONToFloat(rb, "mass");
				rigidbody.drag = JSONDeserializer::JSONToFloat(rb, "drag");
				rigidbody.gravityScale = JSONDeserializer::JSONToFloat(rb, "gravity");
				rigidbody.isStatic = JSONDeserializer::JSONToBool(rb, "static");
				rigidbody.isKinematic = JSONDeserializer::JSONToBool(rb, "kinematic");
				rigidbody.isGrounded = JSONDeserializer::JSONToBool(rb, "grounded");
				ECSManager::GetInstance().AddComponent(newEntity, rigidbody);
				ECSManager::GetInstance().physicsSystem->AddRigidbodyComponent(newEntity, rigidbody);
			}
			if (components.HasMember("Animation")) {
				const auto& animationVal = components["Animation"];
				Animation animation;
				DeserializeAnimation(animation, animationVal);
				ECSManager::GetInstance().AddComponent(newEntity, animation);
			}
			if (components.HasMember("AudioSource")) {
				const auto& as = components["AudioSource"];
				AudioSource audioSource;
				DeserializeAudioSource(audioSource, as);
				ECSManager::GetInstance().AddComponent(newEntity, audioSource);
			}
			if (components.HasMember("ScriptComponent")) {
				const auto& scriptVal = components["ScriptComponent"];
				ScriptComponent script;
				DeserializeScriptComponent(script, newEntity, scriptVal);
				ECSManager::GetInstance().AddComponent(newEntity, script);
			}
			if (components.HasMember("UI")) {
				const auto& uiVal = components["UI"];
				UI ui;
				DeserializeUI(ui, uiVal);
				ECSManager::GetInstance().AddComponent(newEntity, ui);
			}
			if (components.HasMember("VideoPlayer")) {
				const auto& as = components["VideoPlayer"];
				VideoPlayer videoPlayer;
				DeserializeVideoPlayer(videoPlayer, as);
				ECSManager::GetInstance().AddComponent(newEntity, videoPlayer);
			}
			if (components.HasMember("Textbox")) {
				const auto& tb = components["Textbox"];
				Textbox textbox;
				DeserializeTextbox(textbox, tb);
				ECSManager::GetInstance().AddComponent(newEntity, textbox);
			}
			if (components.HasMember("Camera")) {
				const auto& camData = components["Camera"];
				Camera camera;
				DeserializeCamera(camera, camData);
				ECSManager::GetInstance().AddComponent(newEntity, camera);
			}
		}
	}

	if (document.HasMember("Collision Matrix")) {
		const auto& collisionMatrix = document["Collision Matrix"];
		for (size_t i = 0; i < LayerManager::GetInstance().collisionMatrix.size(); ++i) {
			LayerManager::GetInstance().collisionMatrix[i] = JSONDeserializer::JSONToBool(collisionMatrix, std::to_string(i).c_str());
		}
	}
	else {
		for (size_t i = 0; i < LayerManager::GetInstance().collisionMatrix.size(); ++i) {
			LayerManager::GetInstance().collisionMatrix[i] = true;
		}
	}
}

void Serializer::ReloadScene(const std::string& scenePath)
{
	std::ifstream ifs(scenePath);
	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	rapidjson::Document document;
	document.Parse(jsonContent.c_str());

	const auto& entities = document["Entities"];

	int i = 0;
	for (auto& pair : EditorPanel::sceneEntities) {
		Entity entity = pair.id;
		const auto& entityData = entities[i];

		if (entityData.HasMember("Components")) {
			const auto& components = entityData["Components"];

			if (components.HasMember("Name")) {
				const auto& name = components["Name"];

				auto& n = ECSManager::GetInstance().GetComponent<Name>(entity);
				n.name = JSONDeserializer::JSONToString(name, "name");
			}
			if (components.HasMember("Transform")) {
				const auto& transform = components["Transform"];

				Transform& t = ECSManager::GetInstance().GetComponent<Transform>(entity);
				DeserializeTransform(t, transform);
				t.updated = true;
			}
			if (components.HasMember("Renderer")) {
				const auto& renderer = components["Renderer"];

				Renderer& r = ECSManager::GetInstance().GetComponent<Renderer>(entity);
				r.mesh = renderer["mesh"].GetInt();
				if (renderer.HasMember("isAnimated"))
					r.isAnimated = renderer["isAnimated"].GetBool();
				//r.currentTextureID = renderer["textureFile"].GetUint64();
				r.uuid = JSONDeserializer::JSONToString(renderer, "textureFile");

				//ECSManager::GetInstance().AddComponent(entity, r);
			}
			if (components.HasMember("AABBCollider2D")) {
				const auto& colliderVal = components["AABBCollider2D"];

				AABBCollider2D& collider = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity);
				DeserializeAABBCollider2D(collider, colliderVal);
				collider.isUpdated = false;
			}
			if (components.HasMember("Rigidbody2D")) {
				const auto& rb = components["Rigidbody2D"];

				Rigidbody2D& rigidbody = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
				rigidbody.position = JSONDeserializer::JSONToVec2(rb, "pos");
				rigidbody.velocity = JSONDeserializer::JSONToVec2(rb, "vel");
				rigidbody.mass = JSONDeserializer::JSONToFloat(rb, "mass");
				rigidbody.drag = JSONDeserializer::JSONToFloat(rb, "drag");
				rigidbody.gravityScale = JSONDeserializer::JSONToFloat(rb, "gravity");
				rigidbody.isStatic = JSONDeserializer::JSONToBool(rb, "static");
				rigidbody.isKinematic = JSONDeserializer::JSONToBool(rb, "kinematic");
				rigidbody.isGrounded = JSONDeserializer::JSONToBool(rb, "grounded");
			}
			if (components.HasMember("Animation")) {
				const auto& anim = components["Animation"];

				Animation& animation = ECSManager::GetInstance().GetComponent<Animation>(entity);
				DeserializeAnimation(animation, anim);
			}
			if (components.HasMember("AudioSource")) {
				const auto& as = components["AudioSource"];
				AudioSource& audioSource = ECSManager::GetInstance().GetComponent<AudioSource>(entity);
				DeserializeAudioSource(audioSource, as);
			}
			if (components.HasMember("ScriptComponent")) {
				const auto& scriptVal = components["ScriptComponent"];
				ScriptComponent& script = ECSManager::GetInstance().GetComponent<ScriptComponent>(entity);
				DeserializeScriptComponent(script, entity, scriptVal);
			}
			if (components.HasMember("VideoPlayer")) {
				const auto& vp = components["VideoPlayer"];
				VideoPlayer& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);
				DeserializeVideoPlayer(videoPlayer, vp);
			}
		}
		++i;
	}
}

void Serializer::SerializePrefab(const std::string& location, Entity entity) {
	std::string filePath = location;

	rapidjson::Document document;
	document.SetObject();
	auto& allocator = document.GetAllocator();

	rapidjson::Value componentsData(rapidjson::kObjectType);

	const auto& nameComponent = ECSManager::GetInstance().GetComponent<Name>(entity);
	rapidjson::Value nameData(rapidjson::kObjectType);

	SerializeName(nameComponent, nameData, allocator);

	//DeserializeName(nameComponent, nameData);

	//rapidjson::Value nameStr(rapidjson::kStringType);
	//JSONSerializer::StringToJSON(nameComponent.name, allocator, nameStr);
	//nameData.AddMember("name", nameStr, allocator);

	componentsData.AddMember("Name", nameData, allocator);

	//if (ECSManager::GetInstance().TryGetComponent<Name>(entity)) {
	//	const auto& nameComponent = ECSManager::GetInstance().GetComponent<Name>(entity);
	//	rapidjson::Value nameData(rapidjson::kObjectType);

	//	rapidjson::Value nameStr(rapidjson::kStringType);
	//	JSONSerializer::StringToJSON(nameComponent.name, allocator, nameStr);
	//	nameData.AddMember("name", nameStr, allocator);

	//	componentsData.AddMember("Name", nameData, allocator);
	//}

	//if (ECSManager::GetInstance().TryGetComponent<Transform>(entity)) {
	rapidjson::Value transformData(rapidjson::kObjectType);
	SerializeTransform(ECSManager::GetInstance().GetComponent<Transform>(entity), transformData, allocator, true);
	componentsData.AddMember("Transform", transformData, allocator);
	//}

	if (ECSManager::GetInstance().TryGetComponent<Renderer>(entity)) {
		const auto& rendererComponent = ECSManager::GetInstance().GetComponent<Renderer>(entity);
		rapidjson::Value rendererData(rapidjson::kObjectType);

		rendererData.AddMember("mesh", rendererComponent.mesh, allocator);
		rendererData.AddMember("isAnimated", rendererComponent.isAnimated, allocator);
		rendererData.AddMember("sortingLayer", rendererComponent.sortingLayer, allocator);

		rapidjson::Value uuidStr(rapidjson::kStringType);
		JSONSerializer::StringToJSON(rendererComponent.uuid, allocator, uuidStr);
		rendererData.AddMember("textureFile", uuidStr, allocator);

		componentsData.AddMember("Renderer", rendererData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<AABBCollider2D>(entity)) {
		const auto& colliderComponent = ECSManager::GetInstance().GetComponent<AABBCollider2D>(entity);
		rapidjson::Value colliderData(rapidjson::kObjectType);

		colliderData.AddMember("bounciness", colliderComponent.bounciness, allocator);

		rapidjson::Value minColData(rapidjson::kObjectType);
		JSONSerializer::Vec2ToJSON(colliderComponent.min, allocator, minColData);
		colliderData.AddMember("min", minColData, allocator);

		rapidjson::Value maxColData(rapidjson::kObjectType);
		JSONSerializer::Vec2ToJSON(colliderComponent.max, allocator, maxColData);
		colliderData.AddMember("max", maxColData, allocator);

		colliderData.AddMember("isTrigger", colliderComponent.isTrigger, allocator);

		componentsData.AddMember("AABBCollider2D", colliderData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<Rigidbody2D>(entity)) {
		const auto& rbComponent = ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity);
		rapidjson::Value rbData(rapidjson::kObjectType);

		rbData.AddMember("mass", rbComponent.mass, allocator);
		//rbData.AddMember("inverseMass", rbComponent.mass, allocator);
		rbData.AddMember("drag", rbComponent.drag, allocator);
		rbData.AddMember("gravity", rbComponent.gravityScale, allocator);
		rbData.AddMember("static", rbComponent.isStatic, allocator);
		rbData.AddMember("kinematic", rbComponent.isKinematic, allocator);
		rbData.AddMember("grounded", rbComponent.isGrounded, allocator);

		rapidjson::Value rbPos(rapidjson::kObjectType);
		JSONSerializer::Vec2ToJSON(rbComponent.position, allocator, rbPos);
		rbData.AddMember("pos", rbPos, allocator);

		rapidjson::Value rbVel(rapidjson::kObjectType);
		JSONSerializer::Vec2ToJSON(rbComponent.velocity, allocator, rbVel);
		rbData.AddMember("vel", rbVel, allocator);

		componentsData.AddMember("Rigidbody2D", rbData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<Animation>(entity)) {
		rapidjson::Value animData(rapidjson::kObjectType);
		SerializeAnimation(ECSManager::GetInstance().GetComponent<Animation>(entity), animData, allocator);
		componentsData.AddMember("Animation", animData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<AudioSource>(entity)) {
		rapidjson::Value asData(rapidjson::kObjectType);
		SerializeAudioSource(ECSManager::GetInstance().GetComponent<AudioSource>(entity), asData, allocator);
		componentsData.AddMember("AudioSource", asData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<ScriptComponent>(entity)) {
		const auto& scriptComponent = ECSManager::GetInstance().GetComponent<ScriptComponent>(entity);
		rapidjson::Value scriptData(rapidjson::kObjectType);
		SerializeScriptComponent(scriptComponent, entity, scriptData, allocator);
		componentsData.AddMember("ScriptComponent", scriptData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<UI>(entity)) {
		const auto& uiComponent = ECSManager::GetInstance().GetComponent<UI>(entity);
		rapidjson::Value uiData(rapidjson::kObjectType);
		SerializeUI(uiComponent, uiData, allocator);
		componentsData.AddMember("UI", uiData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<VideoPlayer>(entity)) {
		const auto& vpComponent = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);
		rapidjson::Value vpData(rapidjson::kObjectType);
		SerializeVideoPlayer(vpComponent, vpData, allocator);
		componentsData.AddMember("VideoPlayer", vpData, allocator);
	}

	if (ECSManager::GetInstance().TryGetComponent<Camera>(entity)) {
		const auto& camComponent = ECSManager::GetInstance().GetComponent<Camera>(entity);
		rapidjson::Value cameraData(rapidjson::kObjectType);
		SerializeCamera(camComponent, cameraData, allocator);
		componentsData.AddMember("Camera", cameraData, allocator);
	}

	// Add entities array to the document
	document.AddMember("Components", componentsData, allocator);

	// Write JSON to a file
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	std::ofstream ofs(filePath);
	ofs << buffer.GetString();
	ofs.close();
}

Entity Serializer::DeserializePrefab(const std::string& prefabPath)
{
	std::ifstream ifs(prefabPath);
	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	rapidjson::Document document;
	document.Parse(jsonContent.c_str());

	const auto& components = document["Components"];

	Entity newEntity = ECSManager::GetInstance().CreateEntity();

	if (components.HasMember("Name")) {
		const auto& name = components["Name"];
		auto& n = ECSManager::GetInstance().GetComponent<Name>(newEntity);
		DeserializeName(n, name);
		std::string prefabMeta = prefabPath + ".meta";
		n.prefabID = MetadataHandler::ParseUUIDFromMeta(prefabMeta);
		n.prefabPath = prefabPath;
	}
	if (components.HasMember("Transform")) {
		const auto& transformVal = components["Transform"];
		Transform& t = ECSManager::GetInstance().GetComponent<Transform>(newEntity);
		t.parentUUID = 0;
		DeserializeTransform(t, transformVal);
	}
	if (components.HasMember("Renderer")) {
		const auto& renderer = components["Renderer"];

		Renderer r;
		r.mesh = renderer["mesh"].GetInt();
		if (renderer.HasMember("isAnimated"))
			r.isAnimated = renderer["isAnimated"].GetBool();
		//r.currentTextureID = renderer["textureFile"].GetUint64();
		r.uuid = JSONDeserializer::JSONToString(renderer, "textureFile");
		if (renderer.HasMember("sortingLayer"))
			r.sortingLayer = static_cast<SortingLayer>(renderer["sortingLayer"].GetInt());

		ECSManager::GetInstance().AddComponent(newEntity, r);
	}
	if (components.HasMember("AABBCollider2D")) {
		const auto& collider = components["AABBCollider2D"];

		ECSManager::GetInstance().physicsSystem->AddAABBColliderComponent(
			newEntity,
			collider["bounciness"].GetFloat(),
			JSONDeserializer::JSONToVec2(collider, "min"),
			JSONDeserializer::JSONToVec2(collider, "max"),
			collider["isTrigger"].GetBool()
		);
	}
	if (components.HasMember("Rigidbody2D")) {
		const auto& rb = components["Rigidbody2D"];

		Rigidbody2D rigidbody;
		rigidbody.position = JSONDeserializer::JSONToVec2(rb, "pos");
		rigidbody.velocity = JSONDeserializer::JSONToVec2(rb, "vel");
		rigidbody.mass = JSONDeserializer::JSONToFloat(rb, "mass");
		rigidbody.drag = JSONDeserializer::JSONToFloat(rb, "drag");
		rigidbody.gravityScale = JSONDeserializer::JSONToFloat(rb, "gravity");
		rigidbody.isStatic = JSONDeserializer::JSONToBool(rb, "static");
		rigidbody.isKinematic = JSONDeserializer::JSONToBool(rb, "kinematic");
		rigidbody.isGrounded = JSONDeserializer::JSONToBool(rb, "grounded");
		ECSManager::GetInstance().AddComponent(newEntity, rigidbody);
		ECSManager::GetInstance().physicsSystem->AddRigidbodyComponent(newEntity, rigidbody);
	}
	if (components.HasMember("Animation")) {
		const auto& animationVal = components["Animation"];
		Animation animation;
		DeserializeAnimation(animation, animationVal);
		ECSManager::GetInstance().AddComponent(newEntity, animation);
	}
	if (components.HasMember("AudioSource")) {
		const auto& as = components["AudioSource"];
		AudioSource audioSource;
		DeserializeAudioSource(audioSource, as);
		ECSManager::GetInstance().AddComponent(newEntity, audioSource);
	}
	if (components.HasMember("ScriptComponent")) {
		const auto& scriptVal = components["ScriptComponent"];
		ScriptComponent script;
		DeserializeScriptComponent(script, newEntity, scriptVal);
		ECSManager::GetInstance().AddComponent(newEntity, script);
	}
	if (components.HasMember("UI")) {
		const auto& uiVal = components["UI"];
		UI ui;
		DeserializeUI(ui, uiVal);
		ECSManager::GetInstance().AddComponent(newEntity, ui);
	}
	if (components.HasMember("VideoPlayer")) {
		const auto& as = components["VideoPlayer"];
		VideoPlayer videoPlayer;
		DeserializeVideoPlayer(videoPlayer, as);
		ECSManager::GetInstance().AddComponent(newEntity, videoPlayer);
	}
	if (components.HasMember("Camera")) {
		const auto& camData = components["Camera"];
		Camera camera;
		DeserializeCamera(camera, camData);
		ECSManager::GetInstance().AddComponent(newEntity, camera);
	}

	return newEntity;
}

void Serializer::LoadEngineConfig(EngineConfig& config)
{
	std::ifstream ifs("../application.config");
	if (!ifs.is_open()) {
		//std::cerr << "Error: Cannot open file '../engine.config'" << std::endl;
		return;
	}
	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	rapidjson::Document document;
	document.Parse(jsonContent.c_str());

	if (document.HasParseError()) {
		// Handle error
		return;
	}

	config.windowTitle = document["Title"].GetString();
	config.windowWidth = document["Width"].GetInt();
	config.windowHeight = document["Height"].GetInt();
	config.sceneName = document["Scene"].GetString();
	config.isFullscreen = document["Fullscreen"].GetBool();
	config.graphicsQuality = document["Graphics Quality"].GetString();
}

void Serializer::DeserializeName(Name& name, const rapidjson::Value& value) {
	name.name = JSONDeserializer::JSONToString(value, "name");
	name.prefabID = JSONDeserializer::JSONToString(value, "prefabID");
	name.prefabPath = JSONDeserializer::JSONToString(value, "prefabPath");
}

void Serializer::DeserializeTransform(Transform& transform, const rapidjson::Value& value) {
	transform.uuid = JSONDeserializer::JSONtoUInt32(value, "uuid");
	if (transform.uuid == 0) transform.uuid = ComponentIDGenerator::GenerateID('t');
	transform.parentUUID = JSONDeserializer::JSONtoUInt32(value, "parentUUID");
	transform.position = JSONDeserializer::JSONToVec3(value, "position");
	transform.scale = JSONDeserializer::JSONToVec3(value, "scale");
	transform.rotation = JSONDeserializer::JSONToVec3(value, "rotation");
	transform.localPosition = JSONDeserializer::JSONToVec3(value, "localPosition");
	transform.localScale = JSONDeserializer::JSONToVec3(value, "localScale");
	transform.localRotation = JSONDeserializer::JSONToVec3(value, "localRotation");
}

void Serializer::DeserializeRenderer(Renderer& renderer, const rapidjson::Value& value)
{
	renderer; value;
}

void Serializer::DeserializeAnimation(Animation& animation, const rapidjson::Value& value) {
	animation.spritesPerRow = value["spritesPerRow"].GetUint();
	animation.spritesPerCol = value["spritesPerCol"].GetUint();
	animation.numFrames = value["numFrames"].GetUint();

	animation.startFrame = value["startFrame"].GetUint();
	animation.endFrame = value["endFrame"].GetUint();

	animation.currentFrame = animation.startFrame;

	animation.timePerFrame = value["timePerFrame"].GetDouble();

	animation.isLooping = value["isLooping"].GetBool();

	animation.playOnce = JSONDeserializer::JSONToBool(value, "playOnce");

	animation.spriteWidth = 1.0f / animation.spritesPerRow;
	animation.spriteHeight = 1.0f / animation.spritesPerCol;
}

void Serializer::DeserializeAudioSource(AudioSource& audioSource, const rapidjson::Value& value) {
	audioSource.audioClipUUID = value["audioClipUUID"].GetString();
	audioSource.isPlaying = value["isPlaying"].GetBool();
	audioSource.isLooping = value["isLooping"].GetBool();
}

void Serializer::DeserializeAABBCollider2D(AABBCollider2D& collider, const rapidjson::Value& value) {
	collider.bounciness = value["bounciness"].GetFloat();
	collider.min = JSONDeserializer::JSONToVec2(value, "min");
	collider.max = JSONDeserializer::JSONToVec2(value, "max");
	collider.isTrigger = value["isTrigger"].GetBool();
}

void Serializer::DeserializeScriptComponent(ScriptComponent& script, const Entity& entity, const rapidjson::Value& value) {
	// Deserialize class name
	script.className = JSONDeserializer::JSONToString(value, "className");

	auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

	if (value.HasMember("parameters") && value["parameters"].IsObject()) {
		for (auto it = value["parameters"].MemberBegin(); it != value["parameters"].MemberEnd(); ++it) {
			std::string key = it->name.GetString();
			const auto& paramObject = it->value;

			if (paramObject.IsObject() && paramObject.HasMember("type") && paramObject.HasMember("value")) {
				std::string type = paramObject["type"].GetString();
				
				ScriptFieldType fieldType = Utils::ScriptFieldTypeFromString(type);

				ScriptFieldInstance scriptField;

				switch (fieldType) {
				case ScriptFieldType::None:
					break;
				case ScriptFieldType::Float:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Float;
					scriptField.SetValue(paramObject["value"].GetFloat());
					break;
				case ScriptFieldType::Double:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Double;
					scriptField.SetValue(paramObject["value"].GetDouble());
					break;
				case ScriptFieldType::Bool:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Bool;
					scriptField.SetValue(paramObject["value"].GetBool());
					break;
				case ScriptFieldType::Short:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Short;
					scriptField.SetValue<int64_t>(paramObject["value"].GetInt());
					break;
				case ScriptFieldType::Int:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Int;
					scriptField.SetValue(paramObject["value"].GetInt());
					break;
				case ScriptFieldType::Long:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Long;
					scriptField.SetValue(paramObject["value"].GetInt64());
					break;
				case ScriptFieldType::UShort:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::UShort;
					scriptField.SetValue<uint16_t>(static_cast<uint16_t>(paramObject["value"].GetUint()));
					break;
				case ScriptFieldType::UInt:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::UInt;
					scriptField.SetValue(paramObject["value"].GetUint());
					break;
				case ScriptFieldType::ULong:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::ULong;
					scriptField.SetValue(paramObject["value"].GetUint64());
					break;
				case ScriptFieldType::Entity:
					scriptField.Field.Name = key;
					scriptField.Field.Type = ScriptFieldType::Entity;
					scriptField.SetValue(paramObject["value"].GetUint());
					break;
				}

				entityFields[key] = scriptField;
			}
		}
	}
}

void Serializer::DeserializeUI(UI& ui, const rapidjson::Value& value) {
	ui.position = JSONDeserializer::JSONToVec3(value, "position");
	ui.scale = JSONDeserializer::JSONToVec2(value, "scale");
	ui.size = JSONDeserializer::JSONToVec2(value, "size");
	ui.rotation = JSONDeserializer::JSONToFloat(value, "rotation");
}

void Serializer::DeserializeTextbox(Textbox& tb, const rapidjson::Value& value) {
	tb.color = JSONDeserializer::JSONToVec3(value, "color");
	tb.text = JSONDeserializer::JSONToString(value, "text");
	tb.fontUUID = JSONDeserializer::JSONToString(value, "fontUUID");
	tb.centerAligned = JSONDeserializer::JSONToBool(value, "centerAligned");
}

void Serializer::DeserializeVideoPlayer(VideoPlayer& vp, const rapidjson::Value& value) {
	vp.videoClipUUID = value["videoClipUUID"].GetString();
	vp.isPlaying = value["isPlaying"].GetBool();
	vp.playOnAwake = value["playOnAwake"].GetBool();
	vp.isLooping = value["isLooping"].GetBool();
}

void Serializer::DeserializeCamera(Camera& cam, const rapidjson::Value& value) {
	cam.zoom = JSONDeserializer::JSONToFloat(value, "zoom");
	cam.width = JSONDeserializer::JSONToFloat(value, "width");
	cam.height = JSONDeserializer::JSONToFloat(value, "height");

	cam.isMainCamera = JSONDeserializer::JSONToBool(value, "isMainCamera");
	cam.isActive = JSONDeserializer::JSONToBool(value, "isActive");
	
	cam.bloomIntensity = JSONDeserializer::JSONToFloat(value, "bloomIntensity");

	cam.vignetteStrength = JSONDeserializer::JSONToFloat(value, "vignetteStrength");
	cam.vignetteSoftness = JSONDeserializer::JSONToFloat(value, "vignetteSoftness");
	cam.vignetteCenter = JSONDeserializer::JSONToVec2(value, "vignetteCenter");
}

void Serializer::SerializeName(const Name& name, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value nameStr(rapidjson::kStringType);
	JSONSerializer::StringToJSON(name.name, allocator, nameStr);
	value.AddMember("name", nameStr, allocator);

	rapidjson::Value prefabID(rapidjson::kStringType);
	JSONSerializer::StringToJSON(name.prefabID, allocator, prefabID);
	value.AddMember("prefabID", prefabID, allocator);

	rapidjson::Value prefabPath(rapidjson::kStringType);
	JSONSerializer::StringToJSON(name.prefabPath, allocator, prefabPath);
	value.AddMember("prefabPath", prefabPath, allocator);
}

void Serializer::SerializeTransform(const Transform& transform, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator, bool isPrefab) {
	if (!isPrefab)
		value.AddMember("uuid", transform.uuid, allocator);
	else
		value.AddMember("uuid", 0, allocator);

	value.AddMember("parentUUID", transform.parentUUID, allocator);

	rapidjson::Value positionData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(transform.position, allocator, positionData);
	value.AddMember("position", positionData, allocator);

	rapidjson::Value scaleData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(transform.scale, allocator, scaleData);
	value.AddMember("scale", scaleData, allocator);

	rapidjson::Value rotationData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(transform.rotation, allocator, rotationData);
	value.AddMember("rotation", rotationData, allocator);

	rapidjson::Value lpositionData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(transform.localPosition, allocator, lpositionData);
	value.AddMember("localPosition", lpositionData, allocator);

	rapidjson::Value lscaleData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(transform.localScale, allocator, lscaleData);
	value.AddMember("localScale", lscaleData, allocator);

	rapidjson::Value lrotationData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(transform.localRotation, allocator, lrotationData);
	value.AddMember("localRotation", lrotationData, allocator);
}

void Serializer::SerializeAnimation(const Animation& animation, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	value.AddMember("spritesPerRow", animation.spritesPerRow, allocator);
	value.AddMember("spritesPerCol", animation.spritesPerCol, allocator);
	value.AddMember("numFrames", animation.numFrames, allocator);

	value.AddMember("startFrame", animation.startFrame, allocator);
	value.AddMember("endFrame", animation.endFrame, allocator);

	value.AddMember("timePerFrame", animation.timePerFrame, allocator);

	value.AddMember("isLooping", animation.isLooping, allocator);

	value.AddMember("playOnce", animation.playOnce, allocator);
}

void Serializer::SerializeAudioSource(const AudioSource& audioSource, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value uuidStr(rapidjson::kStringType);
	JSONSerializer::StringToJSON(audioSource.audioClipUUID, allocator, uuidStr);
	value.AddMember("audioClipUUID", uuidStr, allocator);

	value.AddMember("isPlaying", audioSource.isPlaying, allocator);
	value.AddMember("isLooping", audioSource.isLooping, allocator);
}

void Serializer::SerializeScriptComponent(const ScriptComponent& script, const Entity& entity, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	value.SetObject();

	//std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(script.className);
	//const auto& fields = entityClass->GetFields();
	auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

	// Serialize class name
	rapidjson::Value classNameValue(rapidjson::kStringType);
	classNameValue.SetString(script.className.c_str(), allocator);
	value.AddMember("className", classNameValue, allocator);

	// Serialize parameters with type information
	rapidjson::Value parametersValue(rapidjson::kObjectType);
	for (const auto& [fieldname, field] : entityFields) {
		rapidjson::Value parameterValue(rapidjson::kObjectType);


		rapidjson::Value fieldValue(rapidjson::kStringType);
		JSONSerializer::StringToJSON(Utils::ScriptFieldTypeToString(field.Field.Type), allocator, fieldValue);
		parameterValue.AddMember("type", fieldValue, allocator);

		ScriptFieldInstance& scriptField = entityFields.at(fieldname);

		switch (field.Field.Type) {
		case ScriptFieldType::None:
			break;
		case ScriptFieldType::Float:
			parameterValue.AddMember("value", scriptField.GetValue<float>(), allocator);
			break;
		case ScriptFieldType::Double:
			parameterValue.AddMember("value", scriptField.GetValue<double>(), allocator);
			break;
		case ScriptFieldType::Bool:
			parameterValue.AddMember("value", scriptField.GetValue<bool>(), allocator);
			break;
		case ScriptFieldType::Short:
			parameterValue.AddMember("value", scriptField.GetValue<int16_t>(), allocator);
			break;
		case ScriptFieldType::Int:
			parameterValue.AddMember("value", scriptField.GetValue<int32_t>(), allocator);
			break;
		case ScriptFieldType::Long:
			parameterValue.AddMember("value", scriptField.GetValue<int64_t>(), allocator);
			break;
		case ScriptFieldType::UShort:
			parameterValue.AddMember("value", scriptField.GetValue<uint16_t>(), allocator);
			break;
		case ScriptFieldType::UInt:
			parameterValue.AddMember("value", scriptField.GetValue<uint32_t>(), allocator);
			break;
		case ScriptFieldType::ULong:
			parameterValue.AddMember("value", scriptField.GetValue<uint64_t>(), allocator);
			break;
		case ScriptFieldType::Entity:
			parameterValue.AddMember("value", scriptField.GetValue<uint32_t>(), allocator);
			break;
		}

		rapidjson::Value keyValue(rapidjson::kStringType);
		keyValue.SetString(fieldname.c_str(), allocator);
		parametersValue.AddMember(keyValue, parameterValue , allocator);
	}
	value.AddMember("parameters", parametersValue, allocator);
}

void Serializer::SerializeUI(const UI& ui, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value positionData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(ui.position, allocator, positionData);
	value.AddMember("position", positionData, allocator);

	rapidjson::Value scaleData(rapidjson::kObjectType);
	JSONSerializer::Vec2ToJSON(ui.scale, allocator, scaleData);
	value.AddMember("scale", scaleData, allocator);

	rapidjson::Value sizeData(rapidjson::kObjectType);
	JSONSerializer::Vec2ToJSON(ui.size, allocator, sizeData);
	value.AddMember("size", sizeData, allocator);

	value.AddMember("rotation", ui.rotation, allocator);
}

void Serializer::SerializeVideoPlayer(const VideoPlayer& vp, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value uuidStr(rapidjson::kStringType);
	JSONSerializer::StringToJSON(vp.videoClipUUID, allocator, uuidStr);
	value.AddMember("videoClipUUID", uuidStr, allocator);

	value.AddMember("isPlaying", vp.isPlaying, allocator);
	value.AddMember("playOnAwake", vp.playOnAwake, allocator);
	value.AddMember("isLooping", vp.isLooping, allocator);
}

void Serializer::SerializeTextbox(const Textbox& tb, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value colorData(rapidjson::kObjectType);
	JSONSerializer::Vec3ToJSON(tb.color, allocator, colorData);
	value.AddMember("color", colorData, allocator);

	rapidjson::Value textData(rapidjson::kStringType);
	JSONSerializer::StringToJSON(tb.text, allocator, textData);
	value.AddMember("text", textData, allocator);

	rapidjson::Value fontUUID(rapidjson::kStringType);
	JSONSerializer::StringToJSON(tb.fontUUID, allocator, fontUUID);
	value.AddMember("fontUUID", fontUUID, allocator);

	value.AddMember("centerAligned", tb.centerAligned, allocator);
}

void Serializer::SerializeCamera(const Camera& cam, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {
	value.AddMember("zoom", cam.zoom, allocator);
	value.AddMember("width", cam.width, allocator);
	value.AddMember("height", cam.height, allocator);

	value.AddMember("isMainCamera", cam.isMainCamera, allocator);
	value.AddMember("isActive", cam.isActive, allocator);

	value.AddMember("bloomIntensity", cam.bloomIntensity, allocator);

	value.AddMember("vignetteStrength", cam.vignetteStrength, allocator);
	value.AddMember("vignetteSoftness", cam.vignetteSoftness, allocator);

	rapidjson::Value vignetteCenter(rapidjson::kObjectType);
	JSONSerializer::Vec2ToJSON(cam.vignetteCenter, allocator, vignetteCenter);
	value.AddMember("vignetteCenter", vignetteCenter, allocator);
}
