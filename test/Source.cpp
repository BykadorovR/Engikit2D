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


int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	//	::testing::GTEST_FLAG(filter) = "Pointer.Callstack";
	return RUN_ALL_TESTS();
	std::getchar(); // keep console window open until Return keystroke
}

TEST(Init_different, Entity) {
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

TEST(Create, Components) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	EXPECT_EQ(object1->createComponent<PositionComponent>(), true);
	EXPECT_EQ(object1->createComponent<PositionComponent>(), false);
}

TEST(Get, Components) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	shared_ptr<Entity> object2 = world.createEntity();
	object1->createComponent<PositionComponent>();
	EXPECT_NE(object1->getComponent<PositionComponent>().get(), nullptr);

	object2->createComponent<PositionComponent>();
	EXPECT_NE(object1->getComponent<PositionComponent>().get(), object2->getComponent<PositionComponent>().get());

	object2->removeComponent<PositionComponent>();
	EXPECT_EQ(object2->getComponent<PositionComponent>().get(), nullptr);
}

TEST(Remove, Components) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	object1->createComponent<MovementComponent>();

	EXPECT_EQ(object1->removeComponent<MovementComponent>(), true);
	EXPECT_EQ(object1->removeComponent<MovementComponent>(), false);
}

TEST(Change, Components) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	object1->createComponent<MovementComponent>();
	auto movement = object1->getComponent<MovementComponent>();
	movement->dx = 5;
	movement->dy = 5;
	auto movementFromEntity = object1->getComponent<MovementComponent>();
	EXPECT_EQ(movement->dx, movementFromEntity->dx);
	EXPECT_EQ(movement->dy, movementFromEntity->dy);
}