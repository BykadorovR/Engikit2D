#pragma once
#include <memory>
#include <vector>

#include "System.h"

class SystemManager {
public:
	template <class ConcreteSystem>
	std::shared_ptr<ConcreteSystem> create() {
		std::shared_ptr<ConcreteSystem> system = get<ConcreteSystem>();
		if (system == nullptr) {
			system = make_shared<ConcreteSystem>();
			_systems.push_back(system);
			return system;
		}
		return nullptr;
	}

	//get existing system or create new one
	template <class ConcreteSystem>
	std::shared_ptr<ConcreteSystem> get() {
		for (auto system : _systems) {
			std::shared_ptr<ConcreteSystem> foundSystem = dynamic_pointer_cast<ConcreteSystem>(system);
			if (foundSystem)
				return foundSystem;
		}
		return nullptr;
	}

private:
	std::vector<std::shared_ptr<System> > _systems;	  //References to all systems to notify about changes
};