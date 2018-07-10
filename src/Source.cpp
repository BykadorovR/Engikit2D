#include "General.h"
#include "World.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"


class PositionComponent : Component<PositionComponent> {
public:
	double x;
	double y;
};

class MovementComponent : Component<MovementComponent> {
public:
	double dx;
	double dy;
};

class InputComponent : Component<InputComponent> {
public:
	enum keyCodes {
		W_KEY,
		S_KEY,
		A_KEY,
		D_KEY
	};

	keyCodes pressedKey;
};

class CollisionComponent : Component<CollisionComponent> {
public:
	//Size of bounding box for collision
	double wx;
	double wy;
};


//should be template with variadic number of parameters for component set
class MovementSystem : public System {
public:
	MovementSystem() {
		systemSignature.addComponent<PositionComponent>();
		systemSignature.addComponent<MovementComponent>();
	}

	void update(int dt) {
		for (auto entity : registeredEntities) {
			PositionComponent& position = parentWorld->getComponent<PositionComponent>(entity);
			MovementComponent& movement = parentWorld->getComponent<MovementComponent>(entity);
			position.x += movement.dx;
			position.y += movement.dy;
		}
	}
};

//need to separate to cpp and h due to a lot of dependencies between classes
int main() {
	cout << "ECS" << endl;
	World world;
	EntityHandle player = world.createEntity();
	EntityHandle object = world.createEntity();
	printf("ID: %d\n", player.getEntity().getID());
	printf("ID: %d\n", object.getEntity().getID());

	PositionComponent position;
	position.x = 30;
	position.y = 40;
	player.addComponent(position);

	MovementComponent movement;
	movement.dx = 1;
	movement.dy = 1;
	player.addComponent(movement);

	CollisionComponent collision;
	collision.wx = 10;
	collision.wx = 1;
	player.addComponent(collision);
	object.addComponent(collision);

	PositionComponent c = player.getComponent<PositionComponent>();
	printf("Position %f, %f\n", c.x, c.y);
	
	MovementSystem mvSystem;
	//TODO: check what all entities were correctly added
	world.addSystem(&mvSystem);

	//TODO: create more entity and check that addComponent correctly register in system entity 

	//TODO: check what all systems now update all entities with the same component masks
	world.getSystem<MovementSystem>().update(1);
	c = player.getComponent<PositionComponent>();
	printf("Player Position %f, %f\n", c.x, c.y);

	object.addComponent(position);
	object.addComponent(movement);
	c = object.getComponent<PositionComponent>();
	printf("Object Position %f, %f\n", c.x, c.y);
	world.getSystem<MovementSystem>().update(1);
	c = object.getComponent<PositionComponent>();
	printf("Object Position %f, %f\n", c.x, c.y);
	return 0;
}
