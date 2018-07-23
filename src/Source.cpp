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
	cout << "ECS" << endl;
	World world;
	auto player = world.createEntity();
	auto object = world.createEntity();

	shared_ptr<PositionComponent> position = make_shared<PositionComponent>();
	position->x = 30;
	position->y = 40;
	player->addComponent<PositionComponent>(position);

	shared_ptr<MovementComponent> movement = make_shared<MovementComponent>();
	movement->dx = 1;
	movement->dy = 1;
	player->addComponent<MovementComponent>(movement);

	shared_ptr<CollisionComponent> collision = make_shared<CollisionComponent>();
	collision->wx = 10;
	collision->wx = 1;
	player->addComponent(collision);
	object->addComponent(collision);

	shared_ptr<PositionComponent> c = player->getComponent<PositionComponent>();
	printf("Position %f, %f\n", c->x, c->y);
	
	shared_ptr<MovementSystem> mvSystem = make_shared<MovementSystem>();
	//TODO: check what all entities were correctly added
	world.registerSystem(mvSystem);

	//TODO: create more entity and check that addComponent correctly register in system entity 

	//TODO: check what all systems now update all entities with the same component masks
	world.getSystem<MovementSystem>()->update(1);
	c = player->getComponent<PositionComponent>();
	printf("Player Position %f, %f\n", c->x, c->y);

	object->addComponent(position);
	object->addComponent(movement);
	c = object->getComponent<PositionComponent>();
	printf("Object Position %f, %f\n", c->x, c->y);
	world.getSystem<MovementSystem>()->update(1);
	//TODO: FIX THE ISSUE WITH POINTERS (PLAYER UPDATED OBJECT(!))
	c = object->getComponent<PositionComponent>();
	printf("Object Position %f, %f\n", c->x, c->y);
	return 0;
}
