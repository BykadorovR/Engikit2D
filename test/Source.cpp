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

TEST(Components_create, Entity) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	EXPECT_EQ(object1->createComponent<PositionComponent>(), true);
	EXPECT_EQ(object1->createComponent<PositionComponent>(), false);
}

TEST(Components_get, Entity) {
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

TEST(Components_remove, Entity) {
	World world;
	shared_ptr<Entity> object1 = world.createEntity();
	object1->createComponent<MovementComponent>();

	EXPECT_EQ(object1->removeComponent<MovementComponent>(), true);
	EXPECT_EQ(object1->removeComponent<MovementComponent>(), false);
}

TEST(Components_change, Entity) {
}