#define GMOCK_DEBUG

#include <gtest/gtest.h>
#include "World.h"
#include "Entity.h"

/*needed for component testing*/
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

class MovementSystem : public System {
public:
	void update(int dt) {
		for (auto entity : getEntities()) {
			auto position = entity->getComponent<PositionComponent>();
			auto  movement = entity->getComponent<MovementComponent>();
			if (!position || !movement)
				continue;
			position->x += movement->dx;
			position->y += movement->dy;
		}
	}
};

class InteractionSystem : public System {

};

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	//	::testing::GTEST_FLAG(filter) = "Pointer.Callstack";
	return RUN_ALL_TESTS();
	std::getchar(); // keep console window open until Return keystroke
}

TEST(Init, Entity) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	EXPECT_NE(object1.get(), nullptr);
	shared_ptr<Entity> object2 = world.createEntity();
	EXPECT_NE(object1.get(), object2.get());
}

TEST(Unregister, Entity) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	EXPECT_EQ(world.unregisterEntity(object1), true);
	EXPECT_EQ(world.unregisterEntity(object1), false);
}

TEST(Create, Component) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	EXPECT_NE(object1->registerComponent<PositionComponent>(), nullptr);
	EXPECT_EQ(object1->registerComponent<PositionComponent>(), nullptr);
}

TEST(Get, Component) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	shared_ptr<Entity> object2 = world.createEntity();
	object1->registerComponent<PositionComponent>();
	EXPECT_NE(object1->getComponent<PositionComponent>().get(), nullptr);

	object2->registerComponent<PositionComponent>();
	EXPECT_NE(object1->getComponent<PositionComponent>().get(), object2->getComponent<PositionComponent>().get());

	object2->removeComponent<PositionComponent>();
	EXPECT_EQ(object2->getComponent<PositionComponent>().get(), nullptr);
}

TEST(Remove, Component) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	object1->registerComponent<MovementComponent>();

	EXPECT_EQ(object1->removeComponent<MovementComponent>(), true);
	EXPECT_EQ(object1->removeComponent<MovementComponent>(), false);
}

TEST(Change, Component) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	shared_ptr<MovementComponent> movement = object1->registerComponent<MovementComponent>();
	movement->dx = 5;
	movement->dy = 5;
	shared_ptr<MovementComponent> movementFromEntity = object1->getComponent<MovementComponent>();
	EXPECT_EQ(movement->dx, movementFromEntity->dx);
	EXPECT_EQ(movement->dy, movementFromEntity->dy);
}

TEST(Init, System) {
	World world;
	EXPECT_NE(world.registerSystem<MovementSystem>(), nullptr);
	EXPECT_EQ(world.registerSystem<MovementSystem>(), nullptr);
}

TEST(Get, System) {
	World world;
	EXPECT_NE(world.registerSystem<MovementSystem>(), nullptr);
	shared_ptr<MovementSystem> movementSystem = world.getSystem<MovementSystem>();
	EXPECT_NE(movementSystem, nullptr);
	shared_ptr<InteractionSystem> interactionSystem = world.getSystem<InteractionSystem>();
	EXPECT_EQ(interactionSystem, nullptr);
}

TEST(Update, System) {
	World world;
	shared_ptr<MovementSystem> movementSystem = world.registerSystem<MovementSystem>();
	shared_ptr<Entity> object1 = world.createEntity();
	shared_ptr<PositionComponent> position = object1->registerComponent<PositionComponent>();
	position->x = 5;
	position->y = 5;
	movementSystem->update(1);
	EXPECT_EQ(object1->getComponent<PositionComponent>()->x, 5);
	EXPECT_EQ(object1->getComponent<PositionComponent>()->y, 5);
	shared_ptr<MovementComponent> movement = object1->registerComponent<MovementComponent>();
	movement->dx = 5;
	movement->dy = 5;
	movementSystem->update(1);
	EXPECT_EQ(object1->getComponent<PositionComponent>()->x, 10);
	EXPECT_EQ(object1->getComponent<PositionComponent>()->y, 10);
}