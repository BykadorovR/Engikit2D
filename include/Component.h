#pragma once
#include "General.h"
#include "Entity.h"

/*
Global counter used for component enumeration
*/
struct ComponentCounter {
	static int counter;
};

int ComponentCounter::counter = 0;

/*
Each user created component should be inheritanced from this class.
Implement static family() method which assign UNIQUE ComponentCounter for
every Component.
*/
template<typename ConcreteComponent>
struct Component {
	// Get the family for the component
	static inline int family() {
		static int family = ComponentCounter::counter++;
		return family;
	}
};

/*
Special method which simplify access to Component family ID
*/
template <typename ConcreteComponent>
static int getComponentFamily() {
	return Component<typename std::remove_const<ConcreteComponent>::type>::family();
}

/*
Every component has own ID which is assigned during creation.
To be able identify which components are related to concrete system
ComponentMask can be used. For example, when we initialize movemenet system
we store inside this system ComponentMask with ID's of Movement and Position
components so further when new entities are added we can easily determine
whether this entity with attached component should be updated by the system or not
*/
struct ComponentMask {
	unsigned int mask = 0;
	template<typename ComponentType>
	void addComponent() {
		mask |= (1 << Component<ComponentType>::family());
	}

	template<typename ComponentType>
	void removeComponent() {
		mask ^= (1 << Component<ComponentType>::family());
	}

	bool checkMask(ComponentMask low) {
		unsigned int convolution = (mask & low.mask);
		return (convolution == low.mask);
	}
};

/*
The base component manager class used for storing
different component managers in World
*/
class BaseComponentManager {
};

/*
ComponentManager class is responsible for managing components inside entities.
Each component manager stores ALL entities with, ONLY related to this component manager,
component's data. For example, MovementComponentManager stores ONLY MovementComponent data for
all entities if exist.
*/
template <class ConcreteComponent>
class ComponentManager : BaseComponentManager {
public:
	void add(EntityID e, ConcreteComponent c) {
		unsigned int instance = componentData.last;
		componentData.data[instance] = c;
		entityMap[e] = instance;
		componentData.last++;
	}

	//Look up the component related to an entity
	ConcreteComponent& get(EntityID e) {
		unsigned int instance = entityMap[e];
		return componentData.data[instance];
	}

	//Destroy the component related to an entity
	void destroy(EntityID e) {
		ComponentInstance instance = entityMap[e];
		componentData.data[instance] = componentData.data[componentData.last];
		entityMap.erase(e); //we delete component assigned to entity
							//need update in entityMap second element = last to instance
		for (it = entityMap.begin(); it != entityMap.end(); ++it)
			if (it->second == componentData.last)
				it->second = instance;
		//
		componentData.last--;
	}

private:
	std::map<EntityID, unsigned int> entityMap;
	//want to store components this way due to performance gains
	struct ComponentData {
		unsigned int last = 0;
		std::array<ConcreteComponent, 128> data;
	} componentData;
};