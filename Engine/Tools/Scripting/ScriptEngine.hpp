/*********************************************************************
 * \file	ScriptEngine.hpp
 * \brief	Defines a Script classes that contains the logic to setup the Scripting Interface of the game engine.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include <filesystem>
#include <map>
#include <unordered_map>
#include <memory>

#include "ScriptField.hpp"
#include "../../ECS/ECSManager.hpp"
#include "../../Physics/PhysicsSystem.hpp"

extern bool appIsRunning;

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoString MonoString;
}

struct ScriptFieldInstance
{
	ScriptField Field;

	/**
	 * @brief Default constructor for the `ScriptFieldInstance` class.
	 *
	 * This constructor initializes the internal buffer `mBuffer` to zero, ensuring that any instance of
	 * `ScriptFieldInstance` starts with a clean state. It is used to set up an instance of the class to
	 * store field data.
	 *
	 * @note This constructor does not take any parameters. The internal buffer is initialized using `memset`
	 *       to avoid uninitialized memory issues.
	 */
	ScriptFieldInstance()
	{
		memset(mBuffer, 0, sizeof(mBuffer));
	}

	/**
	 * @brief Retrieves the value stored in the internal buffer as a specified type.
	 *
	 * This template function retrieves the value stored in the internal buffer `mBuffer` and returns it
	 * as the specified type `T`. The function assumes that the type of `T` matches the size of the data stored
	 * in the buffer.
	 *
	 * @tparam T The type to which the stored value will be cast. The size of `T` must not exceed the size of
	 *           the internal buffer.
	 *
	 * @return The value of type `T` that was stored in the internal buffer.
	 *
	 * @throws std::invalid_argument if the size of `T` exceeds the internal buffer size (16 bytes).
	 */
	template<typename T>
	T GetValue()
	{
		static_assert(sizeof(T) <= 16, "Type too large!");
		return *(T*)mBuffer;
	}

	/**
	 * @brief Stores a value of type `T` into the internal buffer.
	 *
	 * This template function stores a value of type `T` into the internal buffer `mBuffer`. It ensures that the
	 * size of `T` does not exceed the size of the internal buffer (16 bytes) and performs a memory copy to
	 * store the value in the buffer.
	 *
	 * @tparam T The type of value to store. The size of `T` must not exceed the size of the internal buffer.
	 * @param value The value of type `T` to be stored in the internal buffer.
	 *
	 * @throws std::invalid_argument if the size of `T` exceeds the internal buffer size (16 bytes).
	 */
	template<typename T>
	void SetValue(T value)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");
		memcpy(mBuffer, &value, sizeof(T));
	}
private:
	uint8_t mBuffer[16];

	friend class ScriptEngine;
};

using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

class ScriptClass
{
public:
	ScriptClass() = default;

	/**
		* @brief Constructs a ScriptClass object representing a Mono class.
		*
		* This constructor initializes a `ScriptClass` instance by loading the specified Mono class
		* from the appropriate assembly. The class can be loaded from either the core assembly or
		* the application assembly, depending on the `isCore` flag.
		*
		* @param classNamespace The namespace of the Mono class to load.
		* @param className The name of the Mono class to load.
		* @param isCore A boolean flag indicating whether to load the class from the core assembly
		*               (`true`) or the application assembly (`false`).
		*/
	ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

	/**
		* @brief Instantiates an object of the Mono class represented by this ScriptClass.
		*
		* This method creates an instance of the Mono class associated with the `ScriptClass`
		* object by invoking Mono's object creation and initialization process.
		*
		* @return MonoObject* A pointer to the newly instantiated Mono object.
		*/
	MonoObject* Instantiate();

	/**
		* @brief Retrieves a method from the Mono class represented by this ScriptClass.
		*
		* This method searches for a method in the Mono class with the given name and the specified
		* number of parameters. It uses the Mono API to locate the method in the class's metadata.
		*
		* @param name The name of the method to retrieve.
		* @param paramCount The number of parameters the method takes.
		*
		* @return MonoMethod* A pointer to the MonoMethod representing the method, or nullptr if the method
		*         is not found.
		*/
	MonoMethod* GetMethod(const std::string& name, int paramCount);

	/**
	* @brief Invokes a method on a given Mono object instance.
	*
	* This function invokes the specified Mono method on the provided instance and passes
	* any given parameters to the method. If an exception occurs during invocation,
	* it is captured and can be handled accordingly.
	*
	* @param instance The Mono object instance on which the method should be invoked.
	* @param method The Mono method to invoke.
	* @param params An array of pointers to the parameters to pass to the method. The array
	*               should match the expected parameters of the method.
	*
	* @return The result of the method invocation, or `nullptr` if an exception occurred.
	*/
	MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	/**
	* @brief Retrieves the collection of script fields.
	*
	* This function provides access to the `mFields` member, which contains
	* a mapping of field names to their corresponding `ScriptField` objects.
	*
	* @return A constant reference to a map where keys are field names
	*         (as `std::string`) and values are `ScriptField` objects.
	*/
	const std::map<std::string, ScriptField>& GetFields() const { return mFields; }
private:
	std::string mClassNamespace;
	std::string mClassName;

	std::map<std::string, ScriptField> mFields;

	MonoClass* mMonoClass = nullptr;

	friend class ScriptEngine;
};

class ScriptInstance
{
public:

	/**
		* @brief Constructs a new ScriptInstance, instantiating the corresponding script class.
		*
		* This constructor initializes a new instance of the specified script class, binds the
		* relevant methods for initialization and updating, and invokes the class constructor
		* to associate it with the provided entity.
		*
		* @param scriptClass A shared pointer to the ScriptClass representing the script to instantiate.
		* @param entity The Entity to associate with this ScriptInstance.
		*/
	ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity);

	/**
	 * @brief Destructor for the ScriptInstance class.
	 *
	 * Cleans up any resources or references held by the ScriptInstance.
	 * Ensures proper destruction of managed objects associated with the instance.
	 */
	~ScriptInstance();

	/**
		* @brief Invokes the OnInit method of the associated script class.
		*
		* This method attempts to invoke the "OnInit" method of the script class associated with
		* this instance. If the method is defined, it will be executed. This is typically used
		* to initialize the script instance after it has been created.
		*/
	void InvokeOnCreate();

	/**
		* @brief Invokes the OnUpdate method of the associated script class.
		*
		* This method attempts to invoke the "OnUpdate" method of the script class associated with
		* this instance, passing the given time step as a parameter. The method is typically used
		* to update the script instance during each frame.
		*
		* @param ts The time step (delta time) to pass to the "OnUpdate" method.
		*/
	void InvokeOnUpdate(float ts);

	/**
	* @brief Invokes the OnExit method of the script, if available.
	*
	* This method triggers the `OnExit` callback defined in the script class,
	* allowing the script to perform cleanup or exit operations.
	*/
	void InvokeOnExit();

	/**
	* @brief Invokes the OnCollisionEnter method of the script, if available.
	*
	* This method is triggered when a collision begins and passes the collision
	* data to the script.
	*
	* @param collision A `CollisionCS` object containing details about the collision.
	*/
	void InvokeOnCollisionEnter(CollisionCS collision);

	/**
	 * @brief Invokes the OnCollisionStay method of the script, if available.
	 *
	 * This method is triggered during an ongoing collision and passes the collision
	 * data to the script.
	 *
	 * @param collision A `CollisionCS` object containing details about the collision.
	 */
	void InvokeOnCollisionStay(CollisionCS collision);

	/**
	 * @brief Invokes the OnCollisionExit method of the script, if available.
	 *
	 * This method is triggered when a collision ends and passes the collision
	 * data to the script.
	 *
	 * @param collision A `CollisionCS` object containing details about the collision.
	 */
	void InvokeOnCollisionExit(CollisionCS collision);

	/**
	 * @brief Invokes the OnTriggerEnter method of the script, if available.
	 *
	 * This method is triggered when a trigger is entered and passes the collider
	 * data to the script.
	 *
	 * @param collider A `ColliderCS` object containing details about the triggering collider.
	 */
	void InvokeOnTriggerEnter(ColliderCS collider);

	/**
	 * @brief Invokes the OnTriggerStay method of the script, if available.
	 *
	 * This method is triggered during an ongoing trigger interaction and passes
	 * the collider data to the script.
	 *
	 * @param collider A `ColliderCS` object containing details about the triggering collider.
	 */
	void InvokeOnTriggerStay(ColliderCS collider);

	/**
	 * @brief Invokes the OnTriggerExit method of the script, if available.
	 *
	 * This method is triggered when a trigger interaction ends and passes the
	 * collider data to the script.
	 *
	 * @param collider A `ColliderCS` object containing details about the triggering collider.
	 */
	void InvokeOnTriggerExit(ColliderCS collider);

	/**
		* @brief Retrieves the ScriptClass associated with this instance.
		*
		* @return std::shared_ptr<ScriptClass> A shared pointer to the `ScriptClass` associated
		*         with this instance.
		*/
	std::shared_ptr<ScriptClass> GetScriptClass() { return mScriptClass; }

	/**
	 * @brief Retrieves the value of a script field by its name and converts it to the specified type.
	 *
	 * This method retrieves the value of a field in the script instance based on the provided field name,
	 * and attempts to cast it to the type `T`. If the retrieval fails or the type does not match, it returns
	 * the default value of `T`.
	 *
	 * @tparam T The type to which the field value will be cast.
	 * @param name The name of the field to retrieve.
	 * @return The field value of type `T`. If the retrieval fails, a default-initialized object of type `T` is returned.
	 *
	 * @note This function uses a static buffer (`sFieldValueBuffer`) to temporarily store the field's value.
	 * @note There is a size constraint on the type `T`. The size of `T` must be less than or equal to 16 bytes.
	 * @see GetFieldValueInternal for the underlying field retrieval logic.
	 */
	template<typename T>
	T GetFieldValue(const std::string& name)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");

		bool success = GetFieldValueInternal(name, sFieldValueBuffer);
		if (!success)
			return T();

		return *(T*)sFieldValueBuffer;
	}

	/**
	 * @brief Sets the value of a script field by its name.
	 *
	 * This method sets the value of a field in the script instance based on the provided field name,
	 * by assigning the provided value of type `T` to the field. If the field is not found, the operation fails.
	 *
	 * @tparam T The type of the value to set.
	 * @param name The name of the field to modify.
	 * @param value The value to set for the field.
	 *
	 * @note This function uses a static buffer (`sFieldValueBuffer`) to store the field's value before setting it.
	 * @note There is a size constraint on the type `T`. The size of `T` must be less than or equal to 16 bytes.
	 * @see SetFieldValueInternal for the underlying field setting logic.
	 */
	template<typename T>
	void SetFieldValue(const std::string& name, T value)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");

		SetFieldValueInternal(name, &value);
	}

	/**
		* @brief Retrieves the managed MonoObject instance associated with this script instance.
		*
		* @return MonoObject* A pointer to the managed MonoObject associated with this instance.
		*/
	MonoObject* GetManagedObject() { return mInstance; }
private:
	/**
	 * @brief Retrieves the value of a script field by its name.
	 *
	 * Searches the fields of the script class for the specified field name
	 * and copies its value into the provided buffer.
	 *
	 * @param name The name of the field to retrieve.
	 * @param buffer A pointer to the buffer where the field value will be stored.
	 * @return `true` if the field was found and the value retrieved; otherwise, `false`.
	 */
	bool GetFieldValueInternal(const std::string& name, void* buffer);

	/**
	 * @brief Sets the value of a script field by its name.
	 *
	 * Searches the fields of the script class for the specified field name
	 * and assigns the provided value to the field.
	 *
	 * @param name The name of the field to modify.
	 * @param value A pointer to the new value to set for the field.
	 * @return `true` if the field was found and the value was set; otherwise, `false`.
	 */
	bool SetFieldValueInternal(const std::string& name, const void* value);
private:
	std::shared_ptr<ScriptClass> mScriptClass;

	MonoObject* mInstance = nullptr;
	MonoMethod* mConstructor = nullptr;
	MonoMethod* mOnCreateMethod = nullptr;
	MonoMethod* mOnUpdateMethod = nullptr;
	MonoMethod* mOnExitMethod = nullptr;

	MonoMethod* mOnCollisionEnterMethod = nullptr;
	MonoMethod* mOnCollisionStayMethod = nullptr;
	MonoMethod* mOnCollisionExitMethod = nullptr;

	MonoMethod* mOnTriggerEnterMethod = nullptr;
	MonoMethod* mOnTriggerStayMethod = nullptr;
	MonoMethod* mOnTriggerExitMethod = nullptr;

	inline static char sFieldValueBuffer[16];

	uint32_t mHandle = NULL;

	friend class ScriptEngine;
	//friend struct ScriptFieldInstance;
};

class ScriptEngine
{
public:

	/**
		* @brief Initializes the scripting engine and prepares it to load and execute managed code.
		*
		* This function sets up the scripting environment by initializing Mono, registering native
		* functions to be callable from scripts, loading core assemblies, and setting up the
		* initial ScriptClass instances used within the engine.
		*
		*/
	static void Init();

	/**
		* @brief Shuts down the scripting engine and releases allocated resources.
		*
		* This function should be called when the scripting engine is no longer needed, typically
		* during application shutdown, to ensure proper resource deallocation.
		*
		* @see ShutdownMono
		*/
	static void Shutdown();

	/**
		* @brief Initializes the Mono runtime environment for the scripting engine.
		*
		* This function sets up the Mono environment by:
		* - Specifying the assemblies path for Mono using `mono_set_assemblies_path()`.
		* - Initializing the Mono JIT runtime with `mono_jit_init()` and assigning it to the `RootDomain`.
		*
		* If the Mono JIT fails to initialize, the function returns early, allowing for potential error handling.
		* The initialized Mono domain (`RootDomain`) is stored in `s_Data` for future use in managing scripts.
		*
		*/
	static void LoadAssembly(const std::filesystem::path& filePath);

	/**
	 * @brief Loads the specified application assembly into the Mono runtime.
	 *
	 * This method loads a C# assembly (such as the application-specific code) into the Mono runtime by
	 * providing the file path to the assembly. It also sets up a file watcher to monitor changes to the
	 * assembly file and triggers reloading when necessary.
	 *
	 * @param filepath The file path of the assembly to load.
	 *
	 * @note The `AppAssembly` is loaded using the `Utils::LoadMonoAssembly` utility function.
	 * @note The file watcher (`FileWatch`) is set up to monitor the assembly file for changes.
	 * @see `OnAppAssemblyFileSystemEvent` for handling file system events.
	 */
	static void LoadAppAssembly(const std::filesystem::path& filepath);

	/**
	 * @brief Reloads the currently loaded assemblies and refreshes the script environment.
	 *
	 * This method unloads the current Mono domain and reinitializes all assemblies and classes,
	 * including reloading both the core and application assemblies, re-registering components,
	 * and re-instantiating entities with script components.
	 *
	 * @note This function effectively restarts the Mono runtime for the script engine, ensuring
	 *       that any changes in the assemblies are reflected in the running environment.
	 * @note The `ScriptGlue::RegisterComponents` method is called to re-register components.
	 * @note Entities with the `ScriptComponent` are re-initialized using the `ScriptEngine::OnCreateEntity` method.
	 */
	static void ReloadAssembly();

	/**
		* @brief Initializes the script engine runtime with the specified scene and systems.
		*
		* This function sets the scene context and provides references to the
		* physics and render systems, allowing script interactions with these systems.
		*
		* @param scene Pointer to the `ECSManager` representing the scene context.
		* @param pSystem Shared pointer to the `PhysicsSystem`, enabling script
		*        access to physics operations.
		* @param rSystem Shared pointer to the `RenderSystem`, enabling script
		*        access to rendering operations.
		*
		* @note This function must be called before running scripts that interact
		*       with the scene, physics, or rendering systems.
		*/
	static void OnRuntimeStart(ECSManager* scene, 
		std::shared_ptr<PhysicsSystem> physicsSystem, 
		std::shared_ptr<RenderSystem> renderSystem,
		std::shared_ptr<UISystem> uiSystem,
		std::shared_ptr<TransformSystem> transformSystem,
		std::shared_ptr<AnimationSystem> animationSystem,
		std::shared_ptr<CameraSystem> cameraSystem
	);

	/**
		* @brief Stops the script engine runtime and clears the scene context.
		*
		* This function clears the current scene context and cleans up any entity instances
		* that were created during the runtime. It is typically called when the runtime
		* is stopped or when transitioning between scenes.
		*/
	static void OnRuntimeStop();

	/**
		* @brief Checks if a script class exists for the given entity class name.
		*
		* This function searches for the specified `fullClassName` in the registered
		* entity classes and returns `true` if the class is found, otherwise `false`.
		*
		* @param fullClassName The full name of the script class to search for, in the form
		*                      "Namespace.ClassName".
		*
		* @return `true` if the class exists, `false` otherwise.
		*/
	static bool EntityClassExists(const std::string& fullClassName);

	/**
	 * @brief Initializes script instances for all entities in the current scene that have script components.
	 *
	 * This function iterates through all entities in the scene, checking for the presence of a `ScriptComponent`.
	 * For entities with script components, it creates a new `ScriptInstance` using the associated script class.
	 * It also ensures that script field values are restored for each script instance.
	 */
	static void PopulateEntityInstance();

	/**
	* @brief Handles the creation of an entity and initializes its associated script instance.
	*
	* This function checks if a valid script class exists for the entity based on its
	* `ScriptComponent`. If the class exists, a new `ScriptInstance` is created and
	* associated with the entity. It also invokes the `OnCreate` method for the script.
	*
	* @param entity The entity to be created and initialized with a script instance.
	*/
	static void OnCreateEntity(Entity entity);

	/**
	* @brief Handles the creation of an entity and initializes its associated script instance.
	*
	* This function checks if a valid script class exists for the entity based on its
	* `ScriptComponent`. If the class exists, invokes the `OnCreate` method for the script.
	*
	* @param entity The entity to be created and initialized with a script instance.
	*/
	static void OnStartEntity(Entity entity);

	/**
		* @brief Updates the script instance associated with the specified entity.
		*
		* This function checks if the given entity has an associated script instance. If the
		* script instance exists, it calls the `OnUpdate` method of the instance, passing the
		* delta time (`dt`) as an argument. This allows the script to update its behavior
		* during each frame.
		*
		* @param entity The entity whose associated script instance is to be updated.
		* @param dt The delta time for the current frame, used to update the script logic.
		*/
	static void OnUpdateEntity(Entity entity, float dt);

	/**
	* @brief Invokes the `OnCollisionEnter` method in the script instance for the specified entity.
	*
	* This function is called when a collision event occurs between two entities, triggering the corresponding
	* method in the entity's script instance (if one exists). The `OnCollisionEnter` method is typically used
	* for responding to initial collisions, such as applying damage or playing a sound effect.
	*
	* @param entity The entity that has collided with another entity.
	* @param collision The collision data associated with the collision event.
	*
	* @note This function will only invoke the method if the entity has an associated script instance.
	*/
	static void OnEntityCollisionEnter(Entity entity, CollisionCS collision);

	/**
	 * @brief Invokes the `OnCollisionStay` method in the script instance for the specified entity.
	 *
	 * This function is called during each frame that two entities remain in contact after a collision. The
	 * `OnCollisionStay` method can be used to handle continuous effects or checks while two entities stay in contact.
	 *
	 * @param entity The entity that is in contact with another entity.
	 * @param collision The collision data associated with the ongoing contact.
	 *
	 * @note This function will only invoke the method if the entity has an associated script instance.
	 */
	static void OnEntityCollisionStay(Entity entity, CollisionCS collision);

	/**
	 * @brief Invokes the `OnCollisionExit` method in the script instance for the specified entity.
	 *
	 * This function is called when two entities stop colliding, triggering the corresponding method in the entity's
	 * script instance (if one exists). The `OnCollisionExit` method is useful for handling actions when collisions
	 * end, such as stopping sound effects or animations.
	 *
	 * @param entity The entity that has stopped colliding with another entity.
	 * @param collision The collision data associated with the end of the collision.
	 *
	 * @note This function will only invoke the method if the entity has an associated script instance.
	 */
	static void OnEntityCollisionExit(Entity entity, CollisionCS collision);

	/**
	 * @brief Invokes the `OnTriggerEnter` method in the script instance for the specified entity.
	 *
	 * This function is called when an entity enters a trigger zone, invoking the `OnTriggerEnter` method in the
	 * associated script instance. Triggers are often used for detecting when entities enter certain areas,
	 * like doors or zones of interest.
	 *
	 * @param entity The entity that entered the trigger zone.
	 * @param collider The collider data associated with the trigger zone.
	 *
	 * @note This function will only invoke the method if the entity has an associated script instance.
	 */
	static void OnEntityTriggerEnter(Entity entity, ColliderCS collider);

	/**
	 * @brief Invokes the `OnTriggerStay` method in the script instance for the specified entity.
	 *
	 * This function is called during each frame that an entity remains inside a trigger zone. The `OnTriggerStay`
	 * method can be used to handle ongoing effects or checks for entities within trigger zones.
	 *
	 * @param entity The entity that is inside the trigger zone.
	 * @param collider The collider data associated with the trigger zone.
	 *
	 * @note This function will only invoke the method if the entity has an associated script instance.
	 */
	static void OnEntityTriggerStay(Entity entity, ColliderCS collider);

	/**
	 * @brief Invokes the `OnTriggerExit` method in the script instance for the specified entity.
	 *
	 * This function is called when an entity exits a trigger zone, triggering the corresponding `OnTriggerExit`
	 * method in the entity's script instance. This is often used to handle actions when entities leave a trigger zone.
	 *
	 * @param entity The entity that exited the trigger zone.
	 * @param collider The collider data associated with the trigger zone.
	 *
	 * @note This function will only invoke the method if the entity has an associated script instance.
	 */
	static void OnEntityTriggerExit(Entity entity, ColliderCS collider);

	/**
		* @brief Retrieves the current ECS (Entity-Component-System) scene context.
		*
		* This function returns a pointer to the `ECSManager` instance that represents
		* the current scene context, which manages the entities and their associated
		* components within the scene.
		*
		* @return ECSManager* A pointer to the current `ECSManager` instance, or `nullptr`
		*         if no scene context has been set.
		*/
	static ECSManager* GetSceneContext();

	/**
	 * @brief Retrieves the `ScriptInstance` associated with the specified entity.
	 *
	 * This function looks up the script instance for the given entity ID. If the entity has an associated
	 * script instance, it is returned as a `std::shared_ptr<ScriptInstance>`. If no script instance is
	 * found for the entity, `nullptr` is returned.
	 *
	 * @param entityID The ID of the entity whose script instance is to be retrieved.
	 *
	 * @return A `std::shared_ptr<ScriptInstance>` to the entity's script instance, or `nullptr` if no script instance is found.
	 *
	 * @note This function provides a way to access the script instance that is responsible for the entity's behavior
	 * in the game. If no script has been attached to the entity, the returned pointer will be `nullptr`.
	 */
	static std::shared_ptr<ScriptInstance> GetEntityScriptInstance(Entity entityID);

	/**
		* @brief Retrieves the map of registered entity classes.
		*
		* This function returns a reference to the `std::unordered_map` that stores
		* the registered entity classes within the script engine. Each entry in the map
		* associates a string (the full class name) with a `std::shared_ptr` to the
		* corresponding `ScriptClass` instance.
		*
		* @return std::unordered_map<std::string, std::shared_ptr<ScriptClass>> A map
		*         where the key is the full class name and the value is a shared pointer
		*         to the associated `ScriptClass` instance.
		*/
	static std::unordered_map<std::string, std::shared_ptr<ScriptClass>> GetEntityClasses();
	
	/**
	 * @brief Retrieves the `ScriptClass` associated with the given class name.
	 *
	 * This function searches for a `ScriptClass` by its name. If the class is found in the `EntityClasses`
	 * map, it is returned as a `std::shared_ptr<ScriptClass>`. If not, `nullptr` is returned.
	 *
	 * @param name The name of the entity class to be retrieved.
	 *
	 * @return A `std::shared_ptr<ScriptClass>` to the entity class, or `nullptr` if the class is not found.
	 *
	 * @note This function allows access to the `ScriptClass` corresponding to a specific entity. It is useful
	 * for operations that need to manipulate or interact with the entity's class, such as reflection or dynamic method calls.
	 */
	static std::shared_ptr<ScriptClass> GetEntityClass(const std::string& name);

	/**
	 * @brief Retrieves the script field map for the specified entity.
	 *
	 * This function returns the map of script fields associated with the specified entity. It looks up the
	 * entity in the `EntityScriptFields` map and retrieves the associated fields for scripting. The map contains
	 * the field names and their corresponding metadata.
	 *
	 * @param entity The entity whose script fields are to be retrieved.
	 *
	 * @return A reference to the `ScriptFieldMap` for the specified entity.
	 *
	 * @note This function provides access to the script fields that are available for an entity, allowing
	 * interaction with the fields dynamically via reflection.
	 */
	static ScriptFieldMap& GetScriptFieldMap(Entity entity);

	/**
		* @brief Retrieves the Mono image associated with the Core Assembly.
		*
		* This function returns the `MonoImage` pointer for the Core Assembly,
		* providing access to the underlying image representation of the assembly's
		* metadata and types.
		*
		* @return A pointer to the `MonoImage` representing the Core Assembly image.
		*
		* @note The returned image is managed by the `ScriptEngine` and should not
		*       be manually deleted.
		*/
	static MonoImage* GetCoreAssemblyImage();

	/**
	 * @brief Retrieves the managed instance of the script attached to the specified entity.
	 *
	 * This function returns a pointer to the `MonoObject` that represents the managed instance of the script
	 * attached to the given entity. The `MonoObject` is a reference to the actual managed C# object running within
	 * the Mono runtime.
	 *
	 * @param id The entity ID for which to retrieve the managed instance.
	 *
	 * @return A `MonoObject*` representing the managed instance of the script, or `nullptr` if no instance is
	 * found for the specified entity.
	 *
	 * @note This is typically used to interact with the underlying managed script (e.g., invoke methods or access
	 * fields) via the Mono runtime.
	 */
	static MonoObject* GetManagedInstance(Entity id);

	/**
	* @brief Returns a list of available script names.
	*
	* Extracts and returns script names from the `EntityClasses` map.
	*/
	static std::vector<std::string> GetAvailableScripts();
	
private:
	/**
		* @brief Initializes the Mono runtime for the script engine.
		*
		* This function sets the path for Mono assemblies and initializes the JIT (Just-In-Time)
		* compiler for the Mono runtime, which is used to execute managed code. It also stores
		* the root domain pointer, which is required for interacting with Mono's runtime.
		*/
	static void InitMono();

	/**
		* @brief Shuts down the Mono runtime and cleans up the associated resources.
		*
		* This function unloads the currently active Mono domain, sets the root domain as active,
		* and then cleans up the JIT (Just-In-Time) compilation environment. After shutting down
		* the Mono runtime, the related domain and root domain pointers are cleared to release
		* the allocated memory.
		*
		* This is typically called during the shutdown process of the application to properly
		* dispose of any resources used by the Mono runtime.
		*/
	static void ShutdownMono();

	/**
		* @brief Instantiates a new object of the specified Mono class.
		*
		* This function creates a new instance of the Mono class specified by the provided
		* `monoClass` argument. It first allocates memory for the object, and then initializes
		* it using the Mono runtime.
		*
		* @param monoClass The Mono class to instantiate. This class must be a valid Mono class.
		* @return MonoObject* A pointer to the newly created Mono object instance.
		*/
	static MonoObject* InstantiateClass(MonoClass* monoClass);

	/**
		* @brief Loads all classes within the Core Assembly that derive from the `Entity` class or represent enums.
		*
		* This function iterates over the types defined in the Core Assembly, checking if each class is a subclass
		* of `ScriptCore.Entity` or is an enum type. For each qualifying class, a `ScriptClass` instance is created
		* and stored in the `EntityClasses` map, with the class's fully qualified name as the key.
		*
		* @note This function clears `EntityClasses` at the start to ensure no stale data is present.
		*
		*/
	static void LoadAssemblyClasses();


	friend class ScriptClass;
	//friend class ScriptGlue;
};

namespace Utils {

	/**
	 * @brief Converts a `ScriptFieldType` enum value to its corresponding string representation.
	 *
	 * This function takes a `ScriptFieldType` enum value and returns a C-string representing the name of the
	 * field type. The string representation is used primarily for logging or debugging purposes.
	 *
	 * @param fieldType The `ScriptFieldType` value to be converted to a string.
	 *
	 * @return A C-string that represents the field type (e.g., "Float", "Int", "Vector3", etc.).
	 *
	 * @note If the provided `fieldType` is not recognized, the function returns "None".
	 */
	inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
	{
		switch (fieldType)
		{
		case ScriptFieldType::None:    return "None";
		case ScriptFieldType::Float:   return "Float";
		case ScriptFieldType::Double:  return "Double";
		case ScriptFieldType::Bool:    return "Bool";
		case ScriptFieldType::Char:    return "Char";
		case ScriptFieldType::Byte:    return "Byte";
		case ScriptFieldType::Short:   return "Short";
		case ScriptFieldType::Int:     return "Int";
		case ScriptFieldType::Long:    return "Long";
		case ScriptFieldType::UByte:   return "UByte";
		case ScriptFieldType::UShort:  return "UShort";
		case ScriptFieldType::UInt:    return "UInt";
		case ScriptFieldType::ULong:   return "ULong";
		case ScriptFieldType::Vector2: return "Vector2";
		case ScriptFieldType::Vector3: return "Vector3";
		case ScriptFieldType::Vector4: return "Vector4";
		case ScriptFieldType::Entity:  return "Entity";
		}
			
		return "None";
	}

	/**
	 * @brief Converts a string representation of a field type to the corresponding `ScriptFieldType` enum value.
	 *
	 * This function takes a string that represents a field type (e.g., "Float", "Vector3", "Int", etc.) and returns
	 * the corresponding `ScriptFieldType` enum value. This is useful for deserializing or converting user input into
	 * the appropriate enum type.
	 *
	 * @param fieldType The string representing the field type to be converted.
	 *
	 * @return The corresponding `ScriptFieldType` enum value, or `ScriptFieldType::None` if the string does not
	 *         match any known field type.
	 *
	 * @note This function performs case-sensitive matching. If the string does not match a known field type,
	 *       `ScriptFieldType::None` is returned.
	 */
	inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
	{
		if (fieldType == "None")    return ScriptFieldType::None;
		if (fieldType == "Float")   return ScriptFieldType::Float;
		if (fieldType == "Double")  return ScriptFieldType::Double;
		if (fieldType == "Bool")    return ScriptFieldType::Bool;
		if (fieldType == "Char")    return ScriptFieldType::Char;
		if (fieldType == "Byte")    return ScriptFieldType::Byte;
		if (fieldType == "Short")   return ScriptFieldType::Short;
		if (fieldType == "Int")     return ScriptFieldType::Int;
		if (fieldType == "Long")    return ScriptFieldType::Long;
		if (fieldType == "UByte")   return ScriptFieldType::UByte;
		if (fieldType == "UShort")  return ScriptFieldType::UShort;
		if (fieldType == "UInt")    return ScriptFieldType::UInt;
		if (fieldType == "ULong")   return ScriptFieldType::ULong;
		if (fieldType == "Vector2") return ScriptFieldType::Vector2;
		if (fieldType == "Vector3") return ScriptFieldType::Vector3;
		if (fieldType == "Vector4") return ScriptFieldType::Vector4;
		if (fieldType == "Entity")  return ScriptFieldType::Entity;

			
		return ScriptFieldType::None;
	}

	/**
	 * @brief Compiles the C# script assembly for the sandbox project using .NET.
	 *
	 * This function invokes the .NET CLI to build the C# project (`SandBox.csproj`)
	 * and copies the compiled `SandBox.dll` into the appropriate assets directory.
	 *
	 * @throws std::runtime_error If the compilation process fails or cannot be launched.
	 *
	 * @details
	 * - The function constructs a build command using `dotnet build` with specific options:
	 *   - `-c Debug`: Builds in Debug configuration.
	 *   - `--no-self-contained`: Ensures only the necessary runtime dependencies are used.
	 *   - `-o "./tmp_build/"`: Outputs the build files into a temporary directory.
	 *   - `-r "win-x64"`: Targets the Windows x64 runtime.
	 * - A new process is created using `CreateProcessW` to execute the command.
	 * - The function waits for the process to complete and checks the exit code:
	 *   - If successful (`exitCode == 0`), the compiled DLL is copied to the assets directory.
	 *   - Otherwise, an exception is thrown indicating build failure.
	 *
	 * @note
	 * - This function is Windows-specific due to its use of `CreateProcessW` and `GetLastError`.
	 * - Ensure that the .NET SDK is installed and accessible at `C:\Program Files\dotnet\dotnet.exe`.
	 * - The function relies on `std::filesystem` to manage paths and copy files.
	 */
	void compileScriptAssembly();

}
