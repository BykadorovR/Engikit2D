#include "General.h"
#include "World.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"


class PositionComponent : public Component {
public:
	double x;
	double y;
};

class MovementComponent : public Component {
public:
	double dx;
	double dy;
};

class InputComponent : public Component {
public:
	enum keyCodes {
		W_KEY,
		S_KEY,
		A_KEY,
		D_KEY
	};

	keyCodes pressedKey;
};

class CollisionComponent : public Component {
public:
	//Size of bounding box for collision
	double wx;
	double wy;
};


//should be template with variadic number of parameters for component set
class MovementSystem : public System {
public:
	void update(int dt) {
		for (auto entity : entityManager->getEntities()) {
			auto position = entity->getComponent<PositionComponent>();
			auto  movement = entity->getComponent<MovementComponent>();
			if (!position || !movement)
				continue;
			position->x += movement->dx;
			position->y += movement->dy;
		}
	}
};

//need to separate to cpp and h due to a lot of dependencies between classes
int main() {
}
