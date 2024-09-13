#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "Signature.hpp"
#include "System.hpp"

class SystemManager {
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem() {
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) == systems.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();
		systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature) {
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) != systems.end() && "System used before registered.");

		// Set the signature for this system
		signatures.insert({ typeName, signature });
	}

	void EntityDestroyed(Entity entity) {
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : systems) {
			auto const& system = pair.second;

			system->m_entities.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature) {
		// Notify each system that an entity's signature changed
		for (auto const& pair : systems) {
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = signatures[type];

			// Entity signature matches system signature - insert into set
			if ((entitySignature & systemSignature) == systemSignature) {
				system->m_entities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else {
				system->m_entities.erase(entity);
			}
		}
	}

private:
	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> signatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};

#endif // !SYSTEM_MANAGER_H