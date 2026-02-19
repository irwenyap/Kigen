/*********************************************************************
 * \file	ScriptGlue.cpp
 * \brief	Defines a ScriptGlue class that links the C# Engine interface with the C++ Engine functionality.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "ScriptGlue.hpp"
#include "ScriptEngine.hpp"
#include "../../Systems/TransformSystem.hpp"
#include "../../Audio/AudioManager.hpp"
#include "../../Utility/MetadataHandler.hpp"
#include "../../Graphics/EngineCamera.hpp"
#include "../../Scene/SceneManager.hpp"
#include "../../Core/Logger.hpp"
#include "../../Engine/StateMachine/CustomState.hpp"

#include "../Engine/Input/InputManager.hpp"
#include "../../Utility/EngineState.hpp"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include <iostream>
#include <optional>

#include "Vec3.hpp" //test
#include <functional>
#include <Timer.hpp>

extern bool appIsRunning;
extern bool onSwitch;
extern Timer TIMER;

static std::unordered_map<MonoType*, std::function<bool(Entity)>> entityComponentMap;

std::shared_ptr<PhysicsSystem> ScriptGlue::ps{};
std::shared_ptr<RenderSystem> ScriptGlue::rs{};
std::shared_ptr<UISystem> ScriptGlue::uis{};
std::shared_ptr<TransformSystem> ScriptGlue::ts{};
std::shared_ptr<AnimationSystem> ScriptGlue::as{};
std::shared_ptr<CameraSystem> ScriptGlue::cs{};
std::shared_ptr<StateMachineSystem> ScriptGlue::sms{};

#define ADD_INTERNAL_CALL(Name) mono_add_internal_call("ScriptCore.InternalCalls::" #Name, Name)

//static void testLog(MonoString* string, int param){
//	char* cstr = mono_string_to_utf8(string);
//	std::string str(cstr);
//
//	mono_free(cstr); //remember to free
//
//	std::cout << str << ", " << param << std::endl;
//}

/**
 * @brief Changes the currently active scene to the specified one.
 *
 * This function loads a new scene specified by its path, effectively replacing the current scene.
 * The scene path is provided as a MonoString, which is converted into a standard C++ string for processing.
 *
 * @param scenePath A MonoString representing the path or name of the scene to load.
 */
static void ChangeScene(MonoString* scenePath) {

	onSwitch = true;

	char* nameCStr = mono_string_to_utf8(scenePath);

	std::string scene(nameCStr);

	mono_free(nameCStr);

	SceneManager::GetInstance().LoadScene(scene.c_str());
}

/**
 * @brief Quits from Application
 */
static void QuitGame() {
	appIsRunning = false;
}

static void HideCursor() {
	Application::GetInstance().HideCursor();
}

static void UnhideCursor() {
	Application::GetInstance().UnhideCursor();
}

static int GetWindowWidth() {
	return Application::GetWindowSize().first;
}

static int GetWindowHeight() {
	return Application::GetWindowSize().second;
}

static double GetFPS() {
	return TIMER.GetFPS();
}

/**
 * @brief Retrieves the managed script instance associated with the given entity.
 *
 * @param entityID The ID of the entity whose script instance is being retrieved.
 * @return A pointer to the MonoObject representing the script instance.
 */
static MonoObject* GetScriptInstance(Entity entityID)
{
	return ScriptEngine::GetManagedInstance(entityID);
}

/**
 * @brief Checks whether the specified entity is currently active.
 *
 * This function queries the active state of the given entity from the entity manager.
 *
 * @param id The entity whose active state is to be checked.
 * @return `true` if the entity is active; otherwise, `false`.
 */
static bool Entity_GetActive(Entity id) {
	return ECSManager::GetInstance().GetEntityManager().GetActive(id);
}

/**
 * @brief Sets the active state of the specified entity.
 *
 * This function updates the active state of the entity in the entity manager
 * and also modifies its visibility in the rendering system accordingly.
 *
 * @param id The entity whose active state is to be updated.
 * @param b The new active state. Pass `true` to activate the entity or `false` to deactivate it.
 */
static void Entity_SetActive(Entity id, bool b) {
	ECSManager::GetInstance().GetEntityManager().SetActive(id,b);
	ECSManager::GetInstance().renderSystem->SetVisibility(id, b);
}

/**
 * @brief Checks if a given entity has a specific component.
 *
 * This function verifies whether an entity has a component of the type
 * specified by the MonoReflectionType object.
 *
 * @param id The ID of the entity to check.
 * @param componentType A reflection type object representing the component type.
 * @return `true` if the entity has the component, `false` otherwise.
 */
static bool Entity_HasComponent(Entity id, MonoReflectionType* componentType)
{
	MonoType* managedType = mono_reflection_type_get_type(componentType);
	if (entityComponentMap.find(managedType) == entityComponentMap.end()) {
		std::cout << "Can't find component in entity map\n";
		return false; //Error out WIP
	}
	return entityComponentMap.at(managedType)(id);
}

/**
 * @brief Finds an entity by its name.
 *
 * Searches the entity manager for an entity with a `Name` component matching
 * the specified string.
 *
 * @param name A MonoString containing the name of the entity to find.
 * @return The ID of the entity if found, or `-1` (as `uint32_t`) if not found.
 *
 * @note The function uses `mono_string_to_utf8` to convert the MonoString to a C-style string.
 *       The memory allocated by `mono_string_to_utf8` is freed using `mono_free`.
 */
static uint32_t Entity_FindEntityByName(MonoString* name)
{
	char* nameCStr = mono_string_to_utf8(name);

	std::string cname(nameCStr);

	mono_free(nameCStr);
	// find out the names of entities
	auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		const auto& enttName = ECSManager::GetInstance().TryGetComponent<Name>(i);
		if (enttName.has_value() && enttName->get().name == cname) {
			return i;
		}
	}

	return static_cast<uint32_t>(-1);
}

static uint32_t Entity_FindEntityByID(uint32_t transformID)
{
	// find out the uuid of the transform component of the Entity
	auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		const auto& entTrans = ECSManager::GetInstance().TryGetComponent<Transform>(i);
		if (entTrans.has_value() && entTrans->get().uuid == transformID) {
			return i;
		}
	}

	return static_cast<uint32_t>(-1);
}

/**
 * @brief Sets the position of the camera.
 *
 * This function updates the camera's position to the specified coordinates.
 * It serves as a temporary implementation until the camera is fully integrated as an entity.
 *
 * @param pos A pointer to a `Vec3` structure containing the new camera position.
 *            - `pos->x`: The x-coordinate of the position.
 *            - `pos->y`: The y-coordinate of the position.
 *            - The z-coordinate (`pos->z`) is currently unused.
 *
 * @note This is a placeholder function and may be replaced in the future
 *       when the camera is managed as an entity.
 */
static void CameraSystem_SetPosition(Vec3* pos){ //tmp while cam not fully entity
	GraphicsManager::GetInstance().camera.SetPosition(pos->x,pos->y);
	//GraphicsManager::GetInstance().camera.SetPosition(glm::vec3(pos->x, pos->y,pos->z));
	//GraphicsManager::GetInstance().camera.SetZoom(pos->z); //jank af
}

/**
 * @brief Gets the translation (position) of a 2D rigidbody component.
 *
 * This function retrieves the translation (position) of a Rigidbody2D component associated
 * with the specified entity and writes it into the provided Vec3 output vector.
 *
 * @param id The entity ID to get the translation for.
 * @param outVec A pointer to the Vec3 structure where the translation will be written.
 *        The z-component will be set to 1 as a default value (for 2D physics).
 */
static void TransformComponent_GetTranslation(Entity id, Vec3* outVec){ //if need to return pointers

	/*Vec2 tmp = ScriptEngine::GetSceneContext()->GetComponent<Rigidbody2D>(id).velocity;

	*outVec = { tmp.x,tmp.y,1 };*/

	*outVec = ScriptEngine::GetSceneContext()->GetComponent<Transform>(id).position;

}

/**
 * @brief Sets the translation (position) of a 2D rigidbody component.
 *
 * This function sets the translation (position) of a Rigidbody2D component associated
 * with the specified entity. The values from the provided Vec3 structure are used to
 * update the 2D position, with the z-component ignored.
 *
 * @param id The entity ID to set the translation for.
 * @param inVec A pointer to the Vec3 structure containing the new translation values.
 *        Only the x and y components are used to set the position.
 */
static void TransformComponent_SetTranslation(Entity id, Vec3* inVec) { //if returning const values
	ScriptGlue::ts->SetPosition(id, *inVec);
}

/**
 * @brief Retrieves the rotation of the Transform component for a given entity.
 *
 * This function accesses the `Transform` component of the specified entity
 * and retrieves its current rotation, storing the result in the provided `Vec3` structure.
 *
 * @param id The ID of the entity whose rotation is to be retrieved.
 * @param rotation A pointer to a `Vec3` structure where the rotation data will be stored.
 *                 - `rotation->x`: The x-component of the rotation.
 *                 - `rotation->y`: The y-component of the rotation.
 *                 - `rotation->z`: The z-component of the rotation.
 */
static void TransformComponent_GetRotation(Entity id, Vec3* rotation) {
	*rotation = ScriptEngine::GetSceneContext()->GetComponent<Transform>(id).rotation;
}

/**
 * @brief Sets the rotation of the Transform component for a given entity.
 *
 * This function updates the rotation of the `Transform` component of the specified entity
 * using the values provided in the `Vec3` structure.
 *
 * @param id The ID of the entity whose rotation is to be updated.
 * @param rotation A pointer to a `Vec3` structure containing the new rotation values.
 *                 - `rotation->x`: The x-component of the rotation to set.
 *                 - `rotation->y`: The y-component of the rotation to set.
 *                 - `rotation->z`: The z-component of the rotation to set.
 */
static void TransformComponent_SetRotation(Entity id, Vec3* rotation) {
	ScriptEngine::GetSceneContext()->transformSystem->SetRotation(id, *rotation);
}

/**
 * @brief Retrieves the scale of a Transform component associated with the specified entity.
 *
 * This function fetches the scale vector of the Transform component of a given entity.
 *
 * @param id The entity whose Transform component's scale is to be retrieved.
 * @param scale A pointer to a `Vec3` structure where the scale vector will be stored.
 */
static void TransformComponent_GetScale(Entity id, Vec3* scale) {
	*scale = ScriptEngine::GetSceneContext()->GetComponent<Transform>(id).scale;
}

/**
 * @brief Sets the scale of a Transform component associated with the specified entity.
 *
 * This function updates the scale vector of the Transform component of a given entity.
 *
 * @param id The entity whose Transform component's scale is to be updated.
 * @param scale A pointer to a `Vec3` structure containing the new scale vector.
 */
static void TransformComponent_SetScale(Entity id, Vec3* scale) {
	ScriptGlue::ts->SetScale(id, *scale);
}


/**
 * @brief Checks if a specific key is currently being held down.
 *
 * This function checks if a key specified by its keycode is being held down in the current frame.
 *
 * @param keycode The keycode of the key to check.
 * @return true if the key is being held down, false otherwise.
 */
static bool Input_IsKeyDown(int keycode) {
	return InputManager::GetInstance().GetKey(keycode);
}

/**
 * @brief Checks if a specific key has been pressed in the current frame.
 *
 * This function checks if a key specified by its keycode was pressed in the current frame.
 *
 * @param keycode The keycode of the key to check.
 * @return true if the key was pressed, false otherwise.
 */
static bool Input_IsKeyPressed(int keycode) {
	return InputManager::GetInstance().GetKeyDown(keycode);
}

/**
 * @brief Checks if a specified mouse button is currently pressed down.
 *
 * This function queries the input system to determine if the specified mouse button
 * is being pressed during the current frame.
 *
 * @param button The mouse button index to check.
 * @return bool `true` if the specified mouse button is currently pressed; otherwise, `false`.
 */
static bool Input_GetMouseDown(int button) {
	return InputManager::GetInstance().GetMouseDown(button);
}

static bool Input_GetMouseHold(int button) {
	return InputManager::GetInstance().GetMouse(button);
}

static void Input_GetMouseX(float* x) {
	*x = static_cast<float>(InputManager::GetInstance().GetMouseX());
}

static void Input_GetMouseY(float* y) {
	*y = static_cast<float>(InputManager::GetInstance().GetMouseY());
}

/**
 * @brief Checks if the connected gamepad's left joystick is being moved.
 *
 * This function queries the input system to determine if the connected gamepad's left joystick
 * is being moved during the current frame.
 *
 * @return Vec2 representing the x and y move direction of the joystick (-1.0f to 1.0f inclusive).
 */
static void Input_GetGamepadLeftStick(Vec2* direction) {
	*direction = InputManager::GetInstance().GetGamepadLeftStick();
}

/**
 * @brief Checks if a specific button on the connected gamepad is currently held down.
 *
 * This function queries the input system to determine if the specified gamepad button
 * is being held down during the current frame.
 *
 * @return bool `true` if the specified button is currently held; otherwise, `false`.
 */
static bool Input_IsGamepadButtonDown(int button) {
	return InputManager::GetInstance().IsGamepadButtonDown(button);
}

/**
 * @brief Checks if a specific button on the connected gamepad is currently pressed.
 *
 * This function queries the input system to determine if the specified gamepad button
 * is being pressed during the current frame.
 *
 * @return bool `true` if the specified button is currently pressed; otherwise, `false`.
 */
static bool Input_IsGamepadButtonPressed(int button) {
	return InputManager::GetInstance().IsGamepadButtonPressed(button);
}

/**
 * @brief Checks if a gamepad is connected.
 *
 * This function queries the input system to determine if a gamepad is connected
 *
 * @return bool `true` if there is a gamepad connected; otherwise, `false`.
 */
static bool Input_IsGamepadConnected() {
	return InputManager::GetInstance().IsGamepadConnected();
}

static bool Input_GetAnyKeyDown() {
	return InputManager::GetInstance().GetAnyKeyDown();
}

// PhysicsSystem
/*________________________________________________________________________________________________________________*/

/**
 * @brief Retrieves the position of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param outPos Pointer to a `Vec2` where the position will be stored.
 */
static void Rigidbody2DComponent_GetRBPosition(Entity entity, Vec2* outPos){
	*outPos = ScriptGlue::ps->GetRBPosition(entity);
}

/**
 * @brief Sets the position of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param pos The new position as a `Vec2`.
 */
static void Rigidbody2DComponent_SetRBPosition(Entity entity, Vec2 pos) {
	ScriptGlue::ps->SetRBPosition(entity, pos);
}

/**
 * @brief Retrieves the velocity of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param outVel Pointer to a `Vec2` where the velocity will be stored.
 */
static void Rigidbody2DComponent_GetRBVelocity(Entity entity, Vec2* outVel) {
	*outVel = ScriptGlue::ps->GetRBVelocity(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity));
}

/**
 * @brief Sets the velocity of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param vel The new velocity as a `Vec2`.
 */
static void Rigidbody2DComponent_SetRBVelocity(Entity entity, Vec2 vel) {
	ScriptGlue::ps->SetRBVelocity(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), vel);
}

/**
 * @brief Retrieves the mass of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @return The mass as a `float`.
 */
static float Rigidbody2DComponent_GetRBMass(Entity entity) {
	return ScriptGlue::ps->GetRBMass(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity));
}

/**
 * @brief Sets the mass of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param newMass The new mass as a `float`.
 */
static void Rigidbody2DComponent_SetRBMass(Entity entity, float newMass) {
	ScriptGlue::ps->SetRBMass(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), newMass);
}

/**
 * @brief Retrieves the drag of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @return The drag as a `float`.
 */
static float Rigidbody2DComponent_GetRBDrag(Entity entity) {
	return ScriptGlue::ps->GetRBDrag(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity));
}

/**
 * @brief Sets the drag of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param newMass The new drag value as a `float`.
 */
static void Rigidbody2DComponent_SetRBDrag(Entity entity, float newMass) {
	ScriptGlue::ps->SetRBDrag(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), newMass);
}

/**
 * @brief Retrieves the gravity scale of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @return The gravity scale as a `float`.
 */
static float Rigidbody2DComponent_GetRBGravityScale(Entity entity) {
	return ScriptGlue::ps->GetRBGravityScale(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity));
}

/**
 * @brief Sets the gravity scale of a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param newScale The new gravity scale as a `float`.
 */
static void Rigidbody2DComponent_SetGravityScale(Entity entity, float newScale) {
	ScriptGlue::ps->SetRBGravityScale(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), newScale);
}

/**
 * @brief Checks if a 2D Rigidbody is static.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @return `true` if static, `false` otherwise.
 */
static bool Rigidbody2DComponent_IsRBStatic(Entity entity) {
	return ScriptGlue::ps->IsRBStatic(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity));
}

/**
 * @brief Sets a 2D Rigidbody as static or non-static.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param value `true` to set as static, `false` otherwise.
 */
static void Rigidbody2DComponent_SetRBStatic(Entity entity, bool value) {
	ScriptGlue::ps->SetRBStatic(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), value);
}

/**
 * @brief Checks if a 2D Rigidbody is kinematic.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @return `true` if kinematic, `false` otherwise.
 */
static bool Rigidbody2DComponent_IsRBKinematic(Entity entity) {
	return ScriptGlue::ps->IsRBKinematic(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity));
}

/**
 * @brief Sets a 2D Rigidbody as kinematic or non-kinematic.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param value `true` to set as kinematic, `false` otherwise.
 */
static void Rigidbody2DComponent_SetRBKinematic(Entity entity, bool value) {
	ScriptGlue::ps->SetRBKinematic(ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), value);
}

/**
 * @brief Adds an impulse force to a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param direction The direction of the force as a `Vec2`.
 * @param magnitude The magnitude of the force.
 * @return A unique ID for the force added.
 */
static size_t Rigidbody2DComponent_AddImpulseForce(Entity entity, Vec2 direction, float magnitude) {
	return ScriptGlue::ps->AddImpulseForce(
		ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), direction, magnitude);
}

/**
 * @brief Adds a force over a specified duration to a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param direction The direction of the force as a `Vec2`.
 * @param magnitude The magnitude of the force.
 * @param duration The duration over which the force is applied.
 * @return A unique ID for the force added.
 */
static size_t Rigidbody2DComponent_AddForceOverTime(Entity entity, Vec2 direction, float magnitude, float duration) {
	return ScriptGlue::ps->AddForceOverTime(
		ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), direction, magnitude, duration);
}

/**
 * @brief Adds an always-active force to a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param forceID The unique ID for the force.
 * @param direction The direction of the force as a `Vec2`.
 * @param magnitude The magnitude of the force.
 * @return A unique ID for the force added.
 */
static size_t Rigidbody2DComponent_AddAlwaysActiveForce(Entity entity, size_t forceID, Vec2 direction, float magnitude) {
	return ScriptGlue::ps->AddAlwaysActiveForce(
		ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), forceID, direction, magnitude);
}

/**
 * @brief Adds a forever impulse force to a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param direction The direction of the force as a `Vec2`.
 * @param magnitude The magnitude of the force.
 * @param startActive Whether the force is active upon creation.
 * @param forceID The unique ID for the force. Defaults to `-1`.
 * @return A unique ID for the force added.
 */
static size_t Rigidbody2DComponent_AddForeverImpulseForce(Entity entity, Vec2 direction, float magnitude, bool startActive, size_t forceID = static_cast<size_t>(-1)) {
	return ScriptGlue::ps->AddForeverImpulseForce(
		ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), direction, magnitude, startActive, forceID);
}

/**
 * @brief Activates or deactivates a specific force on a 2D Rigidbody component.
 *
 * @param entity The ID of the entity owning the Rigidbody.
 * @param forceID The unique ID for the force.
 * @param activate `true` to activate, `false` to deactivate.
 */
static void Rigidbody2DComponent_ActivateForce(Entity entity, size_t forceID, bool activate) {
	ScriptGlue::ps->ActivateForce(
		ECSManager::GetInstance().GetComponent<Rigidbody2D>(entity), forceID, activate);
}

static void VideoPlayerComponent_GetIsPlaying(Entity entity, bool* b) {
	auto& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);

	*b = videoPlayer.isPlaying;
}

static void VideoPlayerComponent_SetIsPlaying(Entity entity, bool* b) {
	auto& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);

	videoPlayer.isPlaying = *b;
}

static void VideoPlayerComponent_Play(Entity entity) {
	auto& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);

	videoPlayer.isPlaying = true;
}

static void VideoPlayerComponent_Stop(Entity entity) {
	auto& videoPlayer = ECSManager::GetInstance().GetComponent<VideoPlayer>(entity);

	videoPlayer.isPlaying = false;
}

/**
 * @brief Retrieves the bounciness of an AABB collider.
 *
 * @param entity The ID of the entity owning the collider.
 * @return The bounciness value as a `float`.
 */
static float AABBColliderComponent_GetBounciness(Entity entity) {
	return ScriptGlue::ps->GetAABBColliderBounciness(entity);
}

/**
 * @brief Sets the bounciness of an AABB collider.
 *
 * @param entity The ID of the entity owning the collider.
 * @param val The new bounciness value as a `float`.
 */
static void AABBColliderComponent_SetBounciness(Entity entity, float val) {
	ScriptGlue::ps->SetAABBColliderBounciness(entity, val);
}

/**
 * @brief Retrieves the minimum bounds of an AABB collider.
 *
 * @param entity The ID of the entity owning the collider.
 * @param min Pointer to a `Vec2` where the minimum bounds will be stored.
 */
static void AABBColliderComponent_GetMin(Entity entity, Vec2* min) {
	*min = ScriptGlue::ps->GetAABBColliderMin(entity);
}


/**
 * @brief Sets the minimum bounds of an AABB collider.
 *
 * @param entity The ID of the entity owning the collider.
 * @param newMin The new minimum bounds as a `Vec2`.
 */
static void AABBColliderComponent_SetMin(Entity entity, Vec2 newMin) {
	ScriptGlue::ps->SetAABBColliderMin(entity, newMin);
}

/**
 * @brief Retrieves the maximum bounds of an AABB collider.
 *
 * @param entity The ID of the entity owning the collider.
 * @param max Pointer to a `Vec2` where the maximum bounds will be stored.
 */
static void AABBColliderComponent_GetMax(Entity entity, Vec2* max) {
	*max = ScriptGlue::ps->GetAABBColliderMax(entity);
}

/**
 * @brief Sets the maximum bounds of an AABB collider.
 *
 * @param entity The ID of the entity owning the collider.
 * @param newMax The new maximum bounds as a `Vec2`.
 */
static void AABBColliderComponent_SetMax(Entity entity, Vec2 newMax) {
	ScriptGlue::ps->SetAABBColliderMax(entity, newMax);
}

/**
 * @brief Retrieves the size of an AABB collider in the x-axis.
 *
 * @param entity The ID of the entity owning the collider.
 * @return The size along the x-axis as a `float`.
 */
static float AABBColliderComponent_GetSizeX(Entity entity) {
	return ScriptGlue::ps->GetAABBColliderSizeX(entity);
}

/**
 * @brief Retrieves the size of an AABB collider in the y-axis.
 *
 * @param entity The ID of the entity owning the collider.
 * @return The size along the y-axis as a `float`.
 */
static float AABBColliderComponent_GetSizeY(Entity entity) {
	return ScriptGlue::ps->GetAABBColliderSizeY(entity);
}

/**
 * @brief Checks if the AABB collider component of an entity is set as a trigger.
 *
 * This function queries the physics system to determine whether the specified entity's
 * axis-aligned bounding box (AABB) collider is configured as a trigger. A trigger collider
 * does not physically collide but detects overlap events.
 *
 * @param entity The ID of the entity whose collider is being queried.
 * @return bool `true` if the AABB collider is set as a trigger; otherwise, `false`.
 */
static bool AABBColliderComponent_GetIsTrigger(Entity entity) {
	return ScriptGlue::ps->IsAABBColliderTrigger(entity);
}

/**
 * @brief Sets the trigger state for the AABB collider component of an entity.
 *
 * This function modifies the physics system to configure the specified entity's
 * axis-aligned bounding box (AABB) collider as a trigger or a physical collider.
 *
 * @param entity The ID of the entity whose collider is being modified.
 * @param b `true` to set the AABB collider as a trigger, or `false` to disable trigger behavior.
 */
static void AABBColliderComponent_SetIsTrigger(Entity entity, bool b) {
	ScriptGlue::ps->SetAABBColliderTrigger(entity, b);
}

/**
 * @brief Plays a sound clip at the specified volume.
 *
 * This function allows you to play a sound file by providing its file path and volume.
 * It ensures the sound metadata is loaded, retrieves the sound's unique identifier (UUID),
 * and then plays the clip at a default position of (0, 0, 0).
 *
 * @param filePath The file path to the sound file as a MonoString.
 * @param vol The volume at which the sound should be played, as a float.
 */
static void AudioSystem_PlaySound(uint32_t entityID, MonoString* filePath, float vol, bool loop) {
	
	std::filesystem::path p = mono_string_to_utf8(filePath);

	MetadataHandler::GenerateMetaFile(p.string()); //check if "loaded"

	// Get UUID
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	// Get or create an entity for audio
	//ECSManager& ecs = ECSManager::GetInstance();
	//Entity entity = ecs.GetEntityManager().CreateEntity();  // Create a new entity for the sound

	AudioManager::GetInstance().PlayClip(entityID, uuid, Vec3(0,0,0), vol, loop);
}
/**
 * @brief Sets the panning value for a specific entity's audio clip.
 *
 * @param entityID The unique ID of the entity playing the sound.
 * @param filePath The file path of the audio clip.
 * @param panning A float value representing the panning (-1.0 = left, 0.0 = center, 1.0 = right).
 */
static void AudioSystem_SetPanning(uint32_t entityID, MonoString* filePath, float panning) {
	std::filesystem::path p = mono_string_to_utf8(filePath);

	// Get UUID from metadata
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	// Update panning for the given entity sound
	AudioManager::GetInstance().SetPanning(entityID, uuid, panning);
}

/**
 * @brief Plays an audio clip for a specific entity with optional panning.
 *
 * @param entityID The unique ID of the entity playing the sound.
 * @param filePath The file path of the audio clip.
 * @param volume The volume of the sound (range: 0.0 - 1.0).
 * @param isPanning If true, enables panning based on entity position.
 */
static void AudioSystem_PlaySoundWithPanning(uint32_t entityID, MonoString* filePath, float volume,bool loop, bool isPanning) {
	std::filesystem::path p = mono_string_to_utf8(filePath);
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");
	AudioManager::GetInstance().PlayClip(entityID, uuid, Vec3(0, 0, 0), volume, loop, isPanning);
}


/**
 * @brief Plays a spatial sound at a given position.
 *
 * Converts the file path to a UUID, ensures the metadata file exists,
 * and plays the corresponding audio clip.
 *
 * @param filePath The path to the audio file.
 * @param pos The 3D position where the sound should be played.
 * @param vol The playback volume (0.0 - 1.0).
 */

static void AudioSystem_SpatialSound(Entity entity, MonoString* filePath, Vec3 pos, float vol)
{
	std::filesystem::path p = mono_string_to_utf8(filePath);

	MetadataHandler::GenerateMetaFile(p.string()); // Ensure metadata is loaded

	// Get UUID
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	std::cout << "[DEBUG] Using Existing Entity: " << entity << " for sound: " << uuid << std::endl;

	// Play the sound on the existing entity instead of creating a new one
	AudioManager::GetInstance().PlayClip(entity, uuid, pos, vol);
}

/**
 * @brief Checks if an audio clip is currently playing.
 *
 * @param filePath The file path of the audio clip.
 * @return true If the audio clip is currently playing.
 * @return false If the audio clip is not playing.
 */
static bool AudioSystem_ClipIsPlaying(MonoString* filePath) {
	std::filesystem::path p = mono_string_to_utf8(filePath);
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	return AudioManager::GetInstance().ClipIsPlaying(uuid);
}
/**
 * @brief Updates the volume of a playing sound.
 *
 * Retrieves the UUID from the file path and adjusts its playback volume.
 *
 * @param filePath The path to the audio file.
 * @param volume The new volume level (0.0 - 1.0).
 */

static void AudioSystem_UpdateSound(uint32_t entityID, MonoString* filePath, float volume) {
	if (!filePath) {
		printf("[ERROR] C++: filePath is null!\n");
		return;
	}

	std::filesystem::path p = mono_string_to_utf8(filePath);
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	if (uuid.empty()) {
		printf("[ERROR] C++: Could not retrieve UUID from %s\n", p.string().c_str());
		return;
	}

	// Directly update the volume
	AudioManager::GetInstance().UpdateSoundVolume(entityID, volume);
}

/**
 * @brief Updates the listener's position and orientation.
 *
 * Sets the listener's spatial attributes for 3D audio positioning.
 *
 * @param position The listener's position in 3D space.
 * @param velocity The listener's velocity for Doppler effects.
 * @param forward The listener's forward direction.
 * @param up The listener's up direction.
 */
static void AudioSystem_SetListener(const Vec3& position, const Vec3& velocity, const Vec3& forward, const Vec3& up) 
{
	AudioManager::GetInstance().UpdateListener(position, velocity, forward, up);
}


/**
 * @brief Stops a sound clip
 *
 * @param filePath The file path to the sound file as a MonoString.
 */
static void AudioSystem_StopSound(MonoString* filePath) {
	std::filesystem::path p = mono_string_to_utf8(filePath);

	MetadataHandler::GenerateMetaFile(p.string()); //check if "loaded"

	// Get UUID
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	AudioManager::GetInstance().StopClip(uuid);
}

static void AudioSystem_StopClip(uint32_t entityID, MonoString* filePath)
{
	std::filesystem::path p = mono_string_to_utf8(filePath);
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	AudioManager::GetInstance().StopClip(entityID, uuid);
}

static void AudioSystem_DuckAllExcept(uint32_t entityID, MonoString* filePath, float duckVolume)
{
	std::filesystem::path p = mono_string_to_utf8(filePath);
	std::string uuid = MetadataHandler::ParseUUIDFromMeta(p.string() + ".meta");

	AudioManager::GetInstance().DuckAllExcept(entityID, uuid, duckVolume);
}

static void AudioSystem_RestoreAllVolumes()
{
	AudioManager::GetInstance().RestoreAllVolumes();
}

// Global volume setter exposed to C#
static void AudioSystem_SetGlobalVolume(float volume)
{
	AudioManager::GetInstance().SetGlobalVolume(volume);
}

static void AudioSystem_ResumeAll() {
	AudioManager::GetInstance().ResumeAll();
}

static void AudioSystem_PauseAll()
{
	AudioManager::GetInstance().PauseAll();
}

static void AudioSystem_StopAll() {
	AudioManager::GetInstance().StopAll();
}


// RenderSystem
/*________________________________________________________________________________________________________________*/

/**
 * @brief Sets the visibility of a renderable entity.
 *
 * @param id The entity ID.
 * @param visible A boolean indicating visibility (true = visible, false = hidden).
 */
static void RenderSystem_SetVisibility(Entity id, bool visible) {
	ScriptGlue::rs->SetVisibility(id, visible);
}

/**
 * @brief Enables or disables debug rendering mode for the render system.
 *
 * @param val A boolean indicating debug mode state (true = enabled, false = disabled).
 */
static void RenderSystem_SetDebugMode(bool val) {
	ScriptGlue::rs->SetDebugMode(val);
}

/**
 * @brief Assigns a texture to a renderable entity.
 *
 * @param entity The entity ID.
 * @param texID The texture identifier as a string.
 */
static void RenderSystem_SetTextureToEntity(Entity entity, MonoString* texID) {
	char* texIDCStr = mono_string_to_utf8(texID);
	std::string uuid(texIDCStr);
	mono_free(texIDCStr);
	ScriptGlue::rs->SetTextureToEntity(entity, uuid);
}

/**
 * @brief Retrieves the entity that was clicked by the user.
 *
 * This function queries the rendering system to determine which entity was clicked based on
 * user input, such as a mouse click. It uses the `ScriptGlue::rs` interface to access the
 * rendering system's functionality.
 *
 * @return Entity The ID of the clicked entity.
 *         - Returns a valid `Entity` ID if an entity was clicked.
 *         - Returns an invalid or default `Entity` if no entity was clicked.
 *
 * @note Ensure the rendering system is properly initialized and integrated with input handling
 *       mechanisms to provide accurate click detection.
 */
static Entity RenderSystem_GetClickedEntity(int fbo) {
	return ScriptGlue::rs->GetClickedEntity(fbo);
}

/*________________________________________________________________________________________________________________*/
/**
 * @brief Sets the position of a UI entity.
 *
 * @param entity The UI entity ID.
 * @param position The position as a Vec2.
 */
static void UISystem_SetPosition(Entity entity, Vec3 position) {
	ScriptGlue::uis->SetPosition(entity, position);
}

/**
 * @brief Sets the scale of a UI entity.
 *
 * @param entity The UI entity ID.
 * @param scale The scale as a Vec2.
 */
static void UISystem_SetScale(Entity entity, Vec2 scale) {
	ScriptGlue::uis->SetScale(entity, scale);
}

/**
 * @brief Sets the size of a UI entity.
 *
 * @param entity The UI entity ID.
 * @param size The size as a Vec2.
 */
static void UISystem_SetSize(Entity entity, Vec2 size) {
	ScriptGlue::uis->SetSize(entity, size);
}
	
/**
 * @brief Sets the text content of a UI entity.
 *
 * @param entity The UI entity ID.
 * @param text The text content as a string.
 */
static void UISystem_SetText(Entity entity, MonoString* mtext) {
	char* textCStr = mono_string_to_utf8(mtext);
	std::string text(textCStr);
	mono_free(textCStr);
	ScriptGlue::uis->SetText(entity, text);
}

/**
 * @brief Sets the font of a UI entity.
 *
 * @param entity The UI entity ID.
 * @param fontUUID The font identifier.
 */
static void UISystem_SetFont(Entity entity, MonoString* fontUUID) {
	char* fontIDCStr = mono_string_to_utf8(fontUUID);
	std::string fontID(fontIDCStr);
	mono_free(fontIDCStr);
	ScriptGlue::uis->SetFont(entity, fontID);
}


/**
 * @brief Sets the color of a UI entity.
 *
 * @param entity The UI entity ID.
 * @param color The color as a Vec3 (RGB values).
 */
static void UISystem_SetColor(Entity entity, Vec3 color) {
	ScriptGlue::uis->SetColor(entity, color);
}

/**
 * @brief Sets the visibility of a UI entity.
 *
 * @param id The UI entity ID.
 * @param visible A boolean indicating visibility (true = visible, false = hidden).
 */
static void UISystem_SetVisibility(Entity id, bool visible) {
	ScriptGlue::uis->SetVisibility(id, visible);
}

static void UISystem_SetCenterAlignment(Entity id, bool centerAlign) {
	ScriptGlue::uis->SetCenterAlignment(id, centerAlign);
}

// TransformSystem
/*________________________________________________________________________________________________________________*/

/**
 * @brief Sets the position of an entity in 3D space.
 *
 * @param id The entity ID.
 * @param pos The position as a Vec3.
 */
//static void TransformSystem_SetPosition(Entity id, Vec3 pos) {
//	ScriptGlue::ts->SetPosition(id, pos);
//}

/**
 * @brief Sets the rotation of an entity in 3D space.
 *
 * @param id The entity ID.
 * @param rot The rotation as a Vec3.
 */
//static void TransformSystem_SetRotation(Entity id, Vec3 rot) {
//	ScriptGlue::ts->SetRotation(id, rot);
//}

/**
 * @brief Sets the scale of an entity in 3D space.
 *
 * @param id The entity ID.
 * @param scale The scale as a Vec3.
 */
//static void TransformSystem_SetScale(Entity id, Vec3 scale) {
//	ScriptGlue::ts->SetScale(id, scale);
//}

/**
 * @brief Applies a translation to an entity.
 *
 * @param id The entity ID.
 * @param translation The translation vector as a Vec3.
 */
//static void TransformSystem_Translate(Entity id, Vec3 translation) {
//	ScriptGlue::ts->Translate(id, translation);
//}

/**
 * @brief Rotates an entity by a specified angle.
 *
 * @param id The entity ID.
 * @param rotation The rotation angle in degrees.
 */
//static void TransformSystem_Rotate(Entity id, float rotation) {
//	ScriptGlue::ts->Rotate(id, rotation);
//}

// AnimationSystem
/*________________________________________________________________________________________________________________*/

/**
 * @brief Sets the animation for a specific entity.
 *
 * This function assigns an animation to the given entity by its `Animation` instance.
 *
 * @param id The entity to which the animation should be set.
 * @param animName The `Animation` object specifying the animation to be set.
 *
 * @note This does not automatically play the animation.
 */
static void AnimationSystem_SetAnimation(Entity id, Animation animName) {
	ScriptGlue::as->SetAnimation(id, animName);
}

/**
 * @brief Plays the animation for a specified entity.
 *
 * This function initiates the playback of the animation assigned to the given entity. Optionally, the
 * animation can be reset to the beginning before playback.
 *
 * @param entity The entity for which the animation should be played.
 * @param reset A boolean indicating whether to reset the animation before playing.
 *              - `true`: Resets the animation to the start.
 *              - `false`: Continues from the current position.
 */
static void AnimationSystem_PlayAnimation(Entity entity, bool reset, bool refresh, bool playOnce, bool isLooping) {
	ScriptGlue::as->PlayAnimation(entity, reset, refresh, playOnce, isLooping);
}

/**
 * @brief Pauses the animation for a specified entity.
 *
 * This function pauses the current animation of the given entity. Optionally, the animation can be reset
 * to the beginning after pausing.
 *
 * @param entity The entity whose animation should be paused.
 * @param reset A boolean indicating whether to reset the animation after pausing.
 *              - `true`: Resets the animation to the start.
 *              - `false`: Keeps the animation at the current position.
 */
static void AnimationSystem_PauseAnimation(Entity entity, bool reset) {
	ScriptGlue::as->PauseAnimation(entity, reset);
}

/**
 * @brief Retrieves the animation component data for a specified entity.
 *
 * This function retrieves the `Animation` component data associated with the given entity and stores it
 * in the provided `Animation` object.
 *
 * @param entityID The entity from which to fetch the `Animation` component data.
 * @param anim A pointer to an `Animation` object where the retrieved data will be stored.
 *
 * @note Ensure that the entity has an `Animation` component before calling this function to avoid
 *       undefined behavior.
 */
static void AnimationComponent_GetData(Entity entityID, Animation* anim) {
	*anim = ScriptEngine::GetSceneContext()->GetComponent<Animation>(entityID);
}

// StateMachine System
/*________________________________________________________________________________________________________________*/

/**
 * @brief Adds a custom state to the entity's state machine.
 * @param entity The entity ID.
 * @param stateName The name of the new state.
 */
static void StateMachineComponent_AddState(Entity entity, const char* stateName)
{
	auto& component = ECSManager::GetInstance().GetComponent<StateMachineComponent>(entity);

	// If no state exists, default to IdleState
	if (component.stateMachine->GetStates().empty())
	{
		std::cout << "No states found. Initializing with IdleState.\n";
		component.stateMachine->AddState(std::make_shared<IdleState>());
		return;
	}

	// Otherwise, create a state dynamically
	auto& registry = StateFactory::GetRegistry();
	if (registry.find(stateName) != registry.end()) {
		component.stateMachine->AddState(registry[stateName]());
	}
	else {
		std::cout << "Warning: Attempted to add unknown state: " << stateName << "\n";
	}
}

/**
 * @brief Adds a transition between two states.
 * @param entity The entity ID.
 * @param from The name of the starting state.
 * @param to The name of the target state.
 */
static void StateMachineComponent_AddTransition(Entity entity, const char* from, const char* to)
{
	ScriptGlue::sms->AddAutomatedTransition(entity, std::string(from), std::string(to), [](Entity*) { return true; });
}

/**
 * @brief Triggers a manual transition in the state machine.
 * @param entity The entity ID.
 * @param targetState The target state name.
 */
static void StateMachineComponent_TriggerTransition(Entity entity, const char* targetState)
{
	ScriptGlue::sms->TriggerManualTransition(entity, std::string(targetState));
}

/**
 * @brief Retrieves the current state of the entity's state machine.
 * @param entity The entity ID.
 * @return const char* The name of the current state.
 */
static const char* StateMachineComponent_GetCurrentState(Entity entity)
{
	auto& component = ECSManager::GetInstance().GetComponent<StateMachineComponent>(entity);
	return component.stateMachine->GetCurrentStateName().c_str();
}

// Logger
/*________________________________________________________________________________________________________________*/

/**
 * @brief Logs a message to the engine's console.
 *
 * @param message The message to log.
 * @param level The log level (e.g., Debug, Info, Warn, Error).
 */
static void Logger_Log(MonoString* message, int level) {
	// Convert MonoString to std::string
	char* cstr = mono_string_to_utf8(message);
	if (!cstr) return; // Safety check

	std::string logMessage(cstr);
	mono_free(cstr);

	// Map C# level to Logger::Level
	Logger::Level logLevel;
	switch (level) {
	case 0: logLevel = Logger::Level::DEBUG; break;
	case 1: logLevel = Logger::Level::INFO; break;
	case 2: logLevel = Logger::Level::WARN; break;
	case 3: logLevel = Logger::Level::ERR; break;
	default:
		// If level is invalid, log an error
		Logger::Instance().Log(Logger::Level::ERR, "[C#] Invalid log level: ", level);
		return;
	}

	// Log the message using the existing Logger system
	Logger::Instance().Log(logLevel, "[C#] ", logMessage);
}

// CameraSystem
/*________________________________________________________________________________________________________________*/

/**
 * @brief Sets the zoom level of the camera.
 *
 * @param id The entity ID of the camera.
 * @param zoom The new zoom level.
 */
static void CameraSystem_SetZoom(Entity id, float zoom) {
	ScriptGlue::cs->SetActiveCamera(id);
	ScriptGlue::cs->SetZoom(zoom);
}

static void CameraSystem_SetMain(Entity id) {
	ScriptGlue::cs->SetMainCamera(id);
}

static void CameraSystem_SetBloom(float bloomIntensity) {
	ScriptGlue::cs->SetBloom(bloomIntensity);
}

/**
 * @brief Sets active camera.
 *
 * @param id The entity ID of the camera.
 */
static void CameraSystem_SetActive(Entity id) {
	ScriptGlue::cs->SetActiveCamera(id);
}

/**
 * @brief Disables the camera.
 */
static void CameraSystem_Disable(Entity id) {
	if (ScriptGlue::cs->GetActiveCamera() == id)
	ScriptGlue::cs->DisableActiveCamera();
}

/**
 * @brief Registers C++ component types with the scripting system.
 *
 * This function maps C++ component types to their corresponding managed types
 * in the scripting environment. It uses a variadic template to register multiple
 * components at once. The mapping is stored in the `entityComponentMap`.
 *
 * @tparam Component The component types to be registered.
 */
template<typename... Component>
static void RegisterComponent()
{
	([]()
		{
			std::string typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(' ');
			std::string structName = typeName.substr(pos + 1);
			std::string managedTypename = "ScriptCore." + structName;

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				std::cout << "Could not find component type " << managedTypename << " \n";
				return;
			}
			entityComponentMap[managedType] = [](Entity entity) { return ScriptEngine::GetSceneContext()->HasComponent<Component>(entity); };
		}(), ...);
}

/**
 * @brief Overload of `RegisterComponent` that accepts a `ComponentGroup`.
 *
 * This function is used to register a group of components together
 * by forwarding the types to the main `RegisterComponent` function.
 *
 * @tparam Component The component types contained in the `ComponentGroup`.
 * @param group The group of components to be registered.
 */
template<typename... Component>
static void RegisterComponent(ComponentGroup<Component...>)
{
	RegisterComponent<Component...>();
}

uint32_t ScriptGlue::FindEntityID(uint32_t transformID)
{
	// find out the uuid of the transform component of the Entity
	auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		const auto& entTrans = ECSManager::GetInstance().TryGetComponent<Transform>(i);
		if (entTrans.has_value() && entTrans->get().uuid == transformID) {
			return i;
		}
	}

	return static_cast<uint32_t>(-1);

}

void ScriptGlue::RegisterComponents()
{
	entityComponentMap.clear();
	RegisterComponent(AllComponents{});
}

void ScriptGlue::RegisterFunctions() {
	//adding function to c# context?
	//ADD_INTERNAL_CALL(testLog);

	ADD_INTERNAL_CALL(ChangeScene);
	ADD_INTERNAL_CALL(QuitGame);
	ADD_INTERNAL_CALL(HideCursor);
	ADD_INTERNAL_CALL(UnhideCursor);
	ADD_INTERNAL_CALL(GetWindowWidth);
	ADD_INTERNAL_CALL(GetWindowHeight);
	ADD_INTERNAL_CALL(GetFPS);

	ADD_INTERNAL_CALL(GetScriptInstance);

	ADD_INTERNAL_CALL(Entity_GetActive);
	ADD_INTERNAL_CALL(Entity_SetActive);
	ADD_INTERNAL_CALL(Entity_HasComponent);
	ADD_INTERNAL_CALL(Entity_FindEntityByName);
	ADD_INTERNAL_CALL(Entity_FindEntityByID);

	ADD_INTERNAL_CALL(CameraSystem_SetPosition);//tmp

	//TransformComponent
	ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
	ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
	ADD_INTERNAL_CALL(TransformComponent_GetRotation);
	ADD_INTERNAL_CALL(TransformComponent_SetRotation);
	ADD_INTERNAL_CALL(TransformComponent_GetScale);
	ADD_INTERNAL_CALL(TransformComponent_SetScale);

	// Input
	ADD_INTERNAL_CALL(Input_GetMouseX);
	ADD_INTERNAL_CALL(Input_GetMouseY);
	ADD_INTERNAL_CALL(Input_IsKeyDown);
	ADD_INTERNAL_CALL(Input_IsKeyPressed);
	ADD_INTERNAL_CALL(Input_GetMouseDown);
	ADD_INTERNAL_CALL(Input_GetMouseHold);
	ADD_INTERNAL_CALL(Input_GetGamepadLeftStick);
	ADD_INTERNAL_CALL(Input_IsGamepadButtonDown);
	ADD_INTERNAL_CALL(Input_IsGamepadButtonPressed);
	ADD_INTERNAL_CALL(Input_IsGamepadConnected);
	ADD_INTERNAL_CALL(Input_GetAnyKeyDown);

	// PhysicsSystem
	ADD_INTERNAL_CALL(Rigidbody2DComponent_GetRBPosition);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetRBPosition);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_GetRBVelocity);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetRBVelocity);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_GetRBMass);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetRBMass);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_GetRBDrag);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetRBDrag);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_GetRBGravityScale);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetGravityScale);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_IsRBStatic);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetRBStatic);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_IsRBKinematic);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_SetRBKinematic);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_AddImpulseForce);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_AddForceOverTime);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_AddAlwaysActiveForce);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_AddForeverImpulseForce);
	ADD_INTERNAL_CALL(Rigidbody2DComponent_ActivateForce);

	//VideoPlayerComponent
	ADD_INTERNAL_CALL(VideoPlayerComponent_GetIsPlaying);
	ADD_INTERNAL_CALL(VideoPlayerComponent_SetIsPlaying);
	ADD_INTERNAL_CALL(VideoPlayerComponent_Play);
	ADD_INTERNAL_CALL(VideoPlayerComponent_Stop);

	ADD_INTERNAL_CALL(AABBColliderComponent_GetBounciness);
	ADD_INTERNAL_CALL(AABBColliderComponent_SetBounciness);
	ADD_INTERNAL_CALL(AABBColliderComponent_GetMin);
	ADD_INTERNAL_CALL(AABBColliderComponent_SetMin);
	ADD_INTERNAL_CALL(AABBColliderComponent_GetMax);
	ADD_INTERNAL_CALL(AABBColliderComponent_SetMax);
	ADD_INTERNAL_CALL(AABBColliderComponent_GetSizeX);
	ADD_INTERNAL_CALL(AABBColliderComponent_GetSizeY);
	ADD_INTERNAL_CALL(AABBColliderComponent_GetIsTrigger);
	ADD_INTERNAL_CALL(AABBColliderComponent_SetIsTrigger);

	ADD_INTERNAL_CALL(AudioSystem_PlaySound);
	ADD_INTERNAL_CALL(AudioSystem_SpatialSound);
	ADD_INTERNAL_CALL(AudioSystem_StopSound);
	ADD_INTERNAL_CALL(AudioSystem_SetListener);
	ADD_INTERNAL_CALL(AudioSystem_UpdateSound);
	ADD_INTERNAL_CALL(AudioSystem_ClipIsPlaying);
	ADD_INTERNAL_CALL(AudioSystem_SetPanning);
	ADD_INTERNAL_CALL(AudioSystem_PlaySoundWithPanning);
	ADD_INTERNAL_CALL(AudioSystem_DuckAllExcept);
	ADD_INTERNAL_CALL(AudioSystem_RestoreAllVolumes);
	ADD_INTERNAL_CALL(AudioSystem_SetGlobalVolume);
	ADD_INTERNAL_CALL(AudioSystem_StopClip);
	ADD_INTERNAL_CALL(AudioSystem_ResumeAll);
	ADD_INTERNAL_CALL(AudioSystem_PauseAll);
	ADD_INTERNAL_CALL(AudioSystem_StopAll);
	
	// RenderSystem
	ADD_INTERNAL_CALL(RenderSystem_SetVisibility);
	ADD_INTERNAL_CALL(RenderSystem_SetDebugMode);
	ADD_INTERNAL_CALL(RenderSystem_SetTextureToEntity);
	ADD_INTERNAL_CALL(RenderSystem_GetClickedEntity);

	// UISystem
	ADD_INTERNAL_CALL(UISystem_SetPosition);
	ADD_INTERNAL_CALL(UISystem_SetScale);
	ADD_INTERNAL_CALL(UISystem_SetSize);
	ADD_INTERNAL_CALL(UISystem_SetText);
	ADD_INTERNAL_CALL(UISystem_SetFont);
	ADD_INTERNAL_CALL(UISystem_SetColor);
	ADD_INTERNAL_CALL(UISystem_SetVisibility);
	ADD_INTERNAL_CALL(UISystem_SetCenterAlignment);

	// AnimationSystem
	ADD_INTERNAL_CALL(AnimationSystem_SetAnimation);
	ADD_INTERNAL_CALL(AnimationSystem_PlayAnimation);
	ADD_INTERNAL_CALL(AnimationSystem_PauseAnimation);
	ADD_INTERNAL_CALL(AnimationComponent_GetData);

	// StateMachine
	ADD_INTERNAL_CALL(StateMachineComponent_AddState);
	ADD_INTERNAL_CALL(StateMachineComponent_AddTransition);
	ADD_INTERNAL_CALL(StateMachineComponent_TriggerTransition);
	ADD_INTERNAL_CALL(StateMachineComponent_GetCurrentState);

	// Logger
	ADD_INTERNAL_CALL(Logger_Log);

	// CameraSystem
	ADD_INTERNAL_CALL(CameraSystem_SetZoom);
	ADD_INTERNAL_CALL(CameraSystem_SetMain);
	ADD_INTERNAL_CALL(CameraSystem_SetActive);
	ADD_INTERNAL_CALL(CameraSystem_Disable);
	ADD_INTERNAL_CALL(CameraSystem_SetBloom);
}
