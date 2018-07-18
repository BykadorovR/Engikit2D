#define GMOCK_DEBUG

#include <gtest/gtest.h>
#include "World.h"
#include "Entity.h"

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	//	::testing::GTEST_FLAG(filter) = "Pointer.Callstack";
	return RUN_ALL_TESTS();
	std::getchar(); // keep console window open until Return keystroke
}

TEST(Entity, Create) {
	World world;
	Entity* player = world.createEntity();
	EXPECT_NE(player, nullptr);
}