/*********************************************************************
 * \file	ScriptEngine.cpp
 * \brief	Defines a Script classes that contains the logic to setup the Scripting Interface of the game engine.
 *
 * \author	wengkhong.thang, 2301372
 * \co-author	wilmerjunrong.lee, 2200691
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "FileWatch.hpp"
#include "ScriptEngine.hpp"
#include "ScriptGlue.hpp"
#include "../../Application.hpp"

#include "../../Components/ScriptComponent.hpp"


#include "mono/include/mono/jit/jit.h"
#include "mono/include/mono/metadata/assembly.h"
#include "mono/include/mono/metadata/tabledefs.h"

#include <fstream>

bool appIsRunning;

static std::unordered_map<std::string, ScriptFieldType> scriptFieldTypeMap =
{
	{ "System.Single", ScriptFieldType::Float },
	{ "System.Double", ScriptFieldType::Double },
	{ "System.Boolean", ScriptFieldType::Bool },
	{ "System.Char", ScriptFieldType::Char },
	{ "System.Int16", ScriptFieldType::Short },
	{ "System.Int32", ScriptFieldType::Int },
	{ "System.Int64", ScriptFieldType::Long },
	{ "System.Byte", ScriptFieldType::Byte },
	{ "System.UInt16", ScriptFieldType::UShort },
	{ "System.UInt32", ScriptFieldType::UInt },
	{ "System.UInt64", ScriptFieldType::ULong },

	{ "Core.Vector2", ScriptFieldType::Vector2 },
	{ "Core.Vector3", ScriptFieldType::Vector3 },
	{ "Core.Vector4", ScriptFieldType::Vector4 },

	{ "ScriptCore.Entity", ScriptFieldType::Entity }
};

namespace Utils {

	/**
		* @brief Reads the contents of a file into a dynamically allocated buffer.
		*
		* This function opens a file at the specified path in binary mode, reads its contents,
		* and returns a pointer to a dynamically allocated buffer containing the data. The size
		* of the read data is stored in `outSize`.
		*
		* @param filepath The path to the file to be read.
		* @param[out] outSize Pointer to a uint32_t where the function will store the size of the read data.
		* @return char* Pointer to the buffer containing the file's contents, or `nullptr` if the file cannot be opened or is empty.
		*
		* @note The caller is responsible for deallocating the buffer to avoid memory leaks.
		*/
	static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = static_cast<uint32_t>(end - stream.tellg());

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	/**
		* @brief Loads a Mono assembly from a file path.
		*
		* This function reads a Mono assembly file, creates a Mono image for initial processing,
		* and loads the assembly using the Mono runtime. If the image fails to load, the function
		* returns `nullptr`.
		*
		* @param assemblyPath The file path to the Mono assembly (.dll or .exe).
		* @return MonoAssembly* Pointer to the loaded Mono assembly, or `nullptr` if an error occurs.
		*
		* @note The caller is responsible for ensuring that the Mono runtime is properly initialized
		*       before calling this function. This function will free any allocated file data after use.
		* @warning The Mono image created in this function is only used for loading the assembly and cannot
		*          be referenced directly by other parts of the application.
		*/
	static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			//const char* errorMessage = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		std::string path = assemblyPath.string();
		MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}

	ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
	{
		std::string typeName = mono_type_get_name(monoType);

		auto it = scriptFieldTypeMap.find(typeName);
		if (it == scriptFieldTypeMap.end())
		{
			std::cout << "Unknown Type: " << typeName << "\n";
			return ScriptFieldType::None;
		}

		return it->second;
	}

	/**
		* @brief Prints the namespaces and type names defined within a given Mono assembly.
		*
		* This function iterates over the type definitions table in a Mono assembly, extracting
		* and printing the namespace and name for each type.
		*
		* @param assembly Pointer to the MonoAssembly containing the types to be printed.
		*
		* @note The assembly should be fully loaded, and its `MonoImage` should be accessible
		*       through the `mono_assembly_get_image` function. This function only outputs type
		*       names to the console and does not retain any data.
		* @warning This function assumes that the Mono runtime is initialized and that the
		*          provided assembly is valid and non-null.
		*/
	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			printf("%s.%s\n", nameSpace, name);
		}
	}
}

struct ScriptEngineData
{
	MonoDomain* RootDomain = nullptr;
	MonoDomain* AppDomain = nullptr;

	MonoAssembly* CoreAssembly = nullptr;
	MonoImage* CoreAssemblyImage = nullptr;

	MonoAssembly* AppAssembly = nullptr;
	MonoImage* AppAssemblyImage = nullptr;

	std::filesystem::path CoreAssemblyPath;
	std::filesystem::path AppAssemblyPath;

	ScriptClass EntityClass;

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
	std::unordered_map<Entity, std::shared_ptr<ScriptInstance>> EntityInstances;
	std::unordered_map<Entity, ScriptFieldMap> EntityScriptFields;

	std::unique_ptr<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
	bool AssemblyReloadPending = false;


	ECSManager* SceneContext = nullptr;

	// trial
	std::vector<uint32_t> mHandles;
};

static ScriptEngineData* s_Data = nullptr;
std::string title = "KigenScriptRuntime";

void ScriptEngine::Init()
{
	s_Data = new ScriptEngineData();

	InitMono();
	ScriptGlue::RegisterFunctions();

	LoadAssembly("../Assets/Scripts/ScriptCore.dll");
	LoadAppAssembly("../SandBoxProject/Assets/Scripts/SandBox.dll");
	LoadAssemblyClasses();

	ScriptGlue::RegisterComponents();

	s_Data->EntityClass = ScriptClass("ScriptCore", "Entity", true);

	// Retrieve and instantiate Class
	//s_Data->EntityClass = ScriptClass("ScriptCore", "Main", true);
	//MonoObject* instance = s_Data->EntityClass.Instantiate();

	//// Call Method
	//MonoMethod* m1 = s_Data->EntityClass.GetMethod("PrintMessage", 0);
	//s_Data->EntityClass.InvokeMethod(instance, m1, nullptr);

	//// Call Method with params
	//MonoString* s2 = mono_string_new(s_Data->AppDomain, "wowowow!");
	//MonoMethod* m2 = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
	//void* stringParam = s2;
	//s_Data->EntityClass.InvokeMethod(instance, m2, &stringParam);

}

void ScriptEngine::Shutdown()
{
	// trial
	for (auto& mHandle : s_Data->mHandles) mono_gchandle_free(mHandle);
	
	ShutdownMono();
	delete s_Data;
}	

void ScriptEngine::InitMono()
{
	mono_set_assemblies_path("mono/lib/4.5");

	MonoDomain* rootDomain = mono_jit_init("KigenJITRuntime");
	if (rootDomain == nullptr)
	{
		// Maybe log some error here
		return;
	}

	// Store the root domain pointer
	s_Data->RootDomain = rootDomain;
}
	 
void ScriptEngine::ShutdownMono()
{
	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(s_Data->AppDomain);
	s_Data->AppDomain = nullptr;

	mono_jit_cleanup(s_Data->RootDomain);
	s_Data->RootDomain = nullptr;
}

void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath) {

	// Create an App Domain
	s_Data->AppDomain = mono_domain_create_appdomain(title.data(), nullptr);
	mono_domain_set(s_Data->AppDomain, true);

	//Load C# Assemblies
	s_Data->CoreAssemblyPath = filePath;
	s_Data->CoreAssembly = Utils::LoadMonoAssembly(filePath);
	s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	//PrintAssemblyTypes(s_Data->CoreAssembly);
}

/**
 * @brief Handles file system events for the application assembly, triggering a reload when modified.
 *
 * This function is called when a file system event (such as modification) is detected on the application
 * assembly. It triggers an assembly reload only if a reload is not already pending. The event handler
 * ensures that the assembly is reloaded in the main thread to prevent issues related to thread synchronization
 * in the Mono runtime.
 *
 * @param path The path of the assembly file that triggered the event.
 * @param change_type The type of file system event that occurred (e.g., modified).
 *
 * @note This function checks if an assembly reload is already pending to prevent multiple reload requests.
 * @note The actual reload operation is deferred to the main thread using `Application::SubmitToMainThread`,
 *       ensuring thread safety when interacting with the Mono runtime.
 */
static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
{
	if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
	{
		s_Data->AssemblyReloadPending = true;

		std::cout << path << std::endl;

		Application::GetInstance().SubmitToMainThread([]()
		{
			s_Data->AppAssemblyFileWatcher.reset();
			ScriptEngine::ReloadAssembly();
		});
	}
}

void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
{
	//Load C# Assemblies
	s_Data->AppAssemblyPath = filepath;
	s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
	s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);

	//get script directory
	std::filesystem::path scriptDirec = "../SandBoxProject/SandBox/SandBox";

	if (std::filesystem::exists(scriptDirec)) { // checks if path exists
		s_Data->AppAssemblyFileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(scriptDirec.string(), OnAppAssemblyFileSystemEvent);
		s_Data->AssemblyReloadPending = false;
	}
	
}

void ScriptEngine::ReloadAssembly() {
	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(s_Data->AppDomain);

	LoadAssembly(s_Data->CoreAssemblyPath);

	Utils::compileScriptAssembly();

	LoadAppAssembly(s_Data->AppAssemblyPath);
	LoadAssemblyClasses();

	ScriptGlue::RegisterComponents();

	// Retrieve and instantiate class
	s_Data->EntityClass = ScriptClass("ScriptCore", "Entity", true);

	//reinstantiate all entity with script component
	ScriptEngine::PopulateEntityInstance();
	auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i))
			ScriptEngine::OnCreateEntity(i);
	}
}

/**
	* @brief Retrieves a Mono class by namespace and class name, with support for nested (enum) types.
	*
	* This function first attempts to find a Mono class directly using `mono_class_from_name` with the specified
	* namespace and class name. If the class is not found, it searches within the `ScriptCore` namespace,
	* specifically checking for nested types (e.g., enums) within the `Physics` class.
	*
	* If the specified `className` matches one of the nested types, that type is returned as a `MonoClass`.
	*
	* @param image A pointer to the MonoImage containing the assembly's metadata.
	* @param nameSpace The namespace of the desired class.
	* @param className The name of the class or enum to find.
	* @return A pointer to the `MonoClass` if found, or `nullptr` if the class cannot be located.
	*
	*/
MonoClass* GetMonoClass(MonoImage* image, const std::string& nameSpace, const std::string& className) {
	MonoClass* monoClass = mono_class_from_name(image, nameSpace.c_str(), className.c_str());
	if (monoClass) {
		return monoClass;
	}

	// Try to find the enum type
	std::string enumName = nameSpace + "." + className;
	monoClass = mono_class_from_name(image, "ScriptCore", "Physics");
	void* enumClass = nullptr;
	monoClass = mono_class_get_nested_types(monoClass, &enumClass);
	while (monoClass) {
		const char* enumClassName = mono_class_get_name(monoClass);
		if (strcmp(enumClassName, className.c_str()) == 0) {
			return monoClass;
		}
		monoClass = mono_class_get_nested_types(monoClass,&enumClass);
	}

	return nullptr;
}

void ScriptEngine::LoadAssemblyClasses()
{
	s_Data->EntityClasses.clear();

	const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
	int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
	MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "ScriptCore", "Entity");

	for (int32_t i = 0; i < numTypes; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

		const std::string nameSpace = std::string(mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]));
		const std::string className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
		std::string fullName;
		if (nameSpace.empty()) fullName = className;
		else 
			fullName = std::string(nameSpace) + "." + std::string(className);
				
		//MonoClass* monoClass = mono_class_from_name(image, nameSpace.c_str(), className.c_str());
		MonoClass* monoClass = GetMonoClass(s_Data->AppAssemblyImage, nameSpace, className);

		if (monoClass == entityClass)
			continue;

		bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false) || mono_class_is_enum(monoClass);
		if (!isEntity) continue;
				
			
		std::shared_ptr<ScriptClass> scriptClass = std::make_shared<ScriptClass>(nameSpace, className);
		s_Data->EntityClasses[fullName] = scriptClass;

		// This routine is an iterator routine for retrieving the fields in a class.
		// You must pass a gpointer that points to zero and is treated as an opaque handle
		// to iterate over all of the elements. When no more values are available, the return value is NULL.
		void* iterator = nullptr;
		while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
		{
			const char* fieldName = mono_field_get_name(field);
			uint32_t flags = mono_field_get_flags(field);
			if (flags & FIELD_ATTRIBUTE_PUBLIC)
			{
				MonoType* type = mono_field_get_type(field);
				ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
				std::cout << fieldName << " Type: " << Utils::ScriptFieldTypeToString(fieldType) << "\n"; //debug check type
				scriptClass->mFields[fieldName] = { fieldType, fieldName, field };
			}
		}
	}
	//auto& entityClasses = s_Data->EntityClasses;

		//mono_field_get_value();
			
}

MonoImage* ScriptEngine::GetCoreAssemblyImage()
{
	return s_Data->CoreAssemblyImage;
}

MonoObject* ScriptEngine::GetManagedInstance(Entity id)
{
	if (s_Data->EntityInstances.find(id) == s_Data->EntityInstances.end())
		return nullptr;

	return s_Data->EntityInstances.at(id)->GetManagedObject();
}

void ScriptEngine::OnRuntimeStart(ECSManager* scene,
	std::shared_ptr<PhysicsSystem> physicsSystem,
	std::shared_ptr<RenderSystem> renderSystem,
	std::shared_ptr<UISystem> uiSystem,
	std::shared_ptr<TransformSystem> transformSystem,
	std::shared_ptr<AnimationSystem> animationSystem,
	std::shared_ptr<CameraSystem> cameraSystem
	)
{
	s_Data->SceneContext = scene;

	ScriptGlue::ps = physicsSystem;
	ScriptGlue::rs = renderSystem;
	ScriptGlue::uis = uiSystem;
	ScriptGlue::ts = transformSystem;
	ScriptGlue::as = animationSystem;
	ScriptGlue::cs = cameraSystem;
}

void ScriptEngine::OnRuntimeStop()
{
	s_Data->SceneContext = nullptr;

	s_Data->EntityInstances.clear();
}

std::shared_ptr<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
{
	if (s_Data->EntityClasses.find(name) == s_Data->EntityClasses.end())
		return nullptr;

	return s_Data->EntityClasses.at(name);
}

ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
{
	return s_Data->EntityScriptFields[entity];
}

bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
{
	return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
}

void ScriptEngine::PopulateEntityInstance()
{
	auto view = ECSManager::GetInstance().GetEntityManager();
	for (Entity i{}; i < view.GetEntities(); i++) {
		if (ECSManager::GetInstance().HasComponent<ScriptComponent>(i)) {
			auto& sc = s_Data->SceneContext->GetComponent<ScriptComponent>(i); //should be const

			if (ScriptEngine::EntityClassExists(sc.className))
			{

				std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.className], i);
				s_Data->EntityInstances[i] = instance;

				// trial
				s_Data->mHandles.push_back(instance->mHandle);

				// Copy field values
				if (s_Data->EntityScriptFields.find(i) != s_Data->EntityScriptFields.end())
				{
					const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(i);
					for (const auto& [name, fieldInstance] : fieldMap)
						instance->SetFieldValueInternal(name, fieldInstance.mBuffer);
				}

			}
		}
	}
}

void ScriptEngine::OnCreateEntity(Entity entity)
{

	auto& sc = s_Data->SceneContext->GetComponent<ScriptComponent>(entity); //should be const
		
	if (ScriptEngine::EntityClassExists(sc.className))
	{

		if (s_Data->EntityInstances.find(entity) == s_Data->EntityInstances.end()) {
			std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.className], entity);
			s_Data->EntityInstances[entity] = instance;

			// trial
			s_Data->mHandles.push_back(instance->mHandle);

			// Copy field values
			if (s_Data->EntityScriptFields.find(entity) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(entity);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValueInternal(name, fieldInstance.mBuffer);
			}

			//instance->InvokeOnCreate();
		}
		else {
			//s_Data->EntityInstances.at(entity)->InvokeOnCreate();
		}
	}
}

void ScriptEngine::OnStartEntity(Entity entity) {
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnCreate();
	}
}

void ScriptEngine::OnUpdateEntity(Entity entity, float dt)
{

	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnUpdate(dt);
	}
}

void ScriptEngine::OnEntityCollisionEnter(Entity entity, CollisionCS collision)
{
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnCollisionEnter(collision);
	}
}

void ScriptEngine::OnEntityCollisionStay(Entity entity, CollisionCS collision)
{
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnCollisionStay(collision);
	}
}

void ScriptEngine::OnEntityCollisionExit(Entity entity, CollisionCS collision)
{
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnCollisionExit(collision);
	}
}

void ScriptEngine::OnEntityTriggerEnter(Entity entity, ColliderCS collider)
{
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnTriggerEnter(collider);
	}
}

void ScriptEngine::OnEntityTriggerStay(Entity entity, ColliderCS collider)
{
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnTriggerStay(collider);
	}
}

void ScriptEngine::OnEntityTriggerExit(Entity entity, ColliderCS collider)
{
	if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
	{
		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
		instance->InvokeOnTriggerExit(collider);
	}
}

MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
{
	MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
	mono_runtime_object_init(instance);
	return instance;
}

ECSManager* ScriptEngine::GetSceneContext() {
	return s_Data->SceneContext;
}

std::shared_ptr<ScriptInstance> ScriptEngine::GetEntityScriptInstance(Entity entityID)
{
	auto it = s_Data->EntityInstances.find(entityID);
	if (it == s_Data->EntityInstances.end())
		return nullptr;

	return it->second;
}

std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::GetEntityClasses()
{
	return s_Data->EntityClasses;
}

ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
	: mClassNamespace(classNamespace), mClassName(className)
{
	mMonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
}

MonoObject* ScriptClass::Instantiate()
{
	return ScriptEngine::InstantiateClass(mMonoClass);
}

MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount)
{
	return mono_class_get_method_from_name(mMonoClass, name.c_str(), paramCount);
}

MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
{
	MonoObject* exception = nullptr;

	// Ensure method, instance, and params are valid before invocation
	if (method == nullptr || instance == nullptr) {
		// Handle null cases explicitly
		return exception;
	}

	return mono_runtime_invoke(method, instance, params, &exception);
}

ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity)
	: mScriptClass(scriptClass)
{
	mInstance = scriptClass->Instantiate();

	// Check object lifetime and ensure proper reference handling
	mHandle = mono_gchandle_new(mInstance, true);  // Pin the object to prevent GC movement

	mConstructor = s_Data->EntityClass.GetMethod(".ctor", 1);
	mOnCreateMethod = scriptClass->GetMethod("OnInit", 0);
	mOnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
	mOnExitMethod = scriptClass->GetMethod("OnExit", 0);
	mOnCollisionEnterMethod = s_Data->EntityClass.GetMethod("OnCollisionEnterCPP", 1);
	mOnCollisionStayMethod = s_Data->EntityClass.GetMethod("OnCollisionStayCPP", 1);
	mOnCollisionExitMethod = s_Data->EntityClass.GetMethod("OnCollisionExitCPP", 1);
	mOnTriggerEnterMethod = s_Data->EntityClass.GetMethod("OnTriggerEnterCPP", 1);
	mOnTriggerStayMethod = s_Data->EntityClass.GetMethod("OnTriggerStayCPP", 1);
	mOnTriggerExitMethod = s_Data->EntityClass.GetMethod("OnTriggerExitCPP", 1);

	// Call Entity constructor
	{
		void* param = &entity;
		mScriptClass->InvokeMethod(mInstance, mConstructor, &param); //temporary replace constructor
	}
}

ScriptInstance::~ScriptInstance() {
	// When done, free the handle to allow potential collection
	// trial
	//mono_gchandle_free(mHandle);
}

void ScriptInstance::InvokeOnCreate()
{
	if (mOnCreateMethod)
		mScriptClass->InvokeMethod(mInstance, mOnCreateMethod);
}

void ScriptInstance::InvokeOnUpdate(float dt)
{
	if (mOnUpdateMethod)
	{
		void* param = &dt;
		mScriptClass->InvokeMethod(mInstance, mOnUpdateMethod, &param);
	}
}

void ScriptInstance::InvokeOnExit()
{
	if (mOnExitMethod)
		mScriptClass->InvokeMethod(mInstance, mOnExitMethod);
}

void ScriptInstance::InvokeOnCollisionEnter(CollisionCS collision)
{
	if (mOnCollisionEnterMethod)
	{
		void* param = &collision;
		mScriptClass->InvokeMethod(mInstance, mOnCollisionEnterMethod, &param);
	}
}

void ScriptInstance::InvokeOnCollisionStay(CollisionCS collision)
{
	if (mOnCollisionStayMethod)
	{
		void* param = &collision;
		mScriptClass->InvokeMethod(mInstance, mOnCollisionStayMethod, &param);
	}
}

void ScriptInstance::InvokeOnCollisionExit(CollisionCS collision)
{
	if (mOnCollisionExitMethod)
	{
		void* param = &collision;
		mScriptClass->InvokeMethod(mInstance, mOnCollisionExitMethod, &param);
	}
}

void ScriptInstance::InvokeOnTriggerEnter(ColliderCS collider)
{
	if (mOnTriggerEnterMethod)
	{
		void* param = &collider;
		mScriptClass->InvokeMethod(mInstance, mOnTriggerEnterMethod, &param);
	}
}

void ScriptInstance::InvokeOnTriggerStay(ColliderCS collider)
{
	if (mOnTriggerStayMethod)
	{
		void* param = &collider;
		mScriptClass->InvokeMethod(mInstance, mOnTriggerStayMethod, &param);
	}
}

void ScriptInstance::InvokeOnTriggerExit(ColliderCS collider)
{
	if (mOnTriggerExitMethod)
	{
		void* param = &collider;
		mScriptClass->InvokeMethod(mInstance, mOnTriggerExitMethod, &param);
	}
}

bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
{
	const auto& fields = mScriptClass->GetFields();
	auto it = fields.find(name);
	if (it == fields.end())
		return false;

	const ScriptField& field = it->second;
	mono_field_get_value(mInstance, field.ClassField, buffer);
	return true;
}

bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
{
	const auto& fields = mScriptClass->GetFields();
	auto it = fields.find(name);
	if (it == fields.end())
		return false;

	const ScriptField& field = it->second;
	mono_field_set_value(mInstance, field.ClassField, (void*)value);
	return true;
}

void Utils::compileScriptAssembly() {

	std::filesystem::path rp = "../SandBoxProject/SandBox/SandBox/SandBox.csproj";

	std::filesystem::path ab = std::filesystem::absolute(rp);

	std::cout << ab;

	std::wstring buildCmd = L" build \"" +
		ab.wstring() +
		L"\" -c Debug --no-self-contained " +
		L"-o \"./tmp_build/\" -r \"win-x64\"";


	// Define the struct to config the compiler process call
	STARTUPINFOW startInfo;
	PROCESS_INFORMATION pi;
	ZeroMemory(&startInfo, sizeof(startInfo));
	ZeroMemory(&pi, sizeof(pi));
	startInfo.cb = sizeof(startInfo);

	// Start compiler process
	const auto SUCCESS = CreateProcess
	(
		L"C:\\Program Files\\dotnet\\dotnet.exe", buildCmd.data(),
		nullptr, nullptr, true, NULL, nullptr, nullptr,
		&startInfo, &pi
	);

	// Check that we launched the process
	if (!SUCCESS)
	{
		auto err = GetLastError();
		std::ostringstream oss;
		oss << "Failed to launch compiler. Error code: "
			<< std::hex << err;
		throw std::runtime_error(oss.str());
	}

	// Wait for process to end
	DWORD exitCode{};
	while (true)
	{
		const auto EXEC_SUCCESS =
			GetExitCodeProcess(pi.hProcess, &exitCode);
		if (!EXEC_SUCCESS)
		{
			auto err = GetLastError();
			std::ostringstream oss;
			oss << "Failed to query process. Error code: "
				<< std::hex << err;
			throw std::runtime_error(oss.str());
		}
		if (exitCode != STILL_ACTIVE)
			break;
	}

	// Successful build
	if (exitCode == 0)
	{
		// Copy out files
		std::filesystem::copy_file
		(
			"./tmp_build/SandBox.dll",
			"../SandBoxProject/Assets/Scripts/SandBox.dll",
			std::filesystem::copy_options::overwrite_existing
		);
	}

	// Failed build
	else
	{
		throw std::runtime_error("Failed to build managed scripts!");
	}
}


std::vector<std::string> ScriptEngine::GetAvailableScripts() {
	std::vector<std::string> scriptNames;

	for (const auto& [name, scriptClass] : s_Data->EntityClasses) {
		scriptNames.push_back(name);
	}

	return scriptNames;
}
