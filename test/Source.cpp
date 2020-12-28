#include <gtest/gtest.h>
#include "Entity.h"
#include "SceneManager.h"
#include <iostream>

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	//::testing::GTEST_FLAG(filter) = "Python_Tests.Crop_NV12_Downscale_Center";

	//Disable cout output from library
	std::cout.setstate(std::ios_base::failbit);
	return RUN_ALL_TESTS();
}

TEST(Init, Entity) {
	std::shared_ptr<SceneManager> sceneManager = std::make_shared<SceneManager>();
	auto activeScene = sceneManager->createScene("basic");

	std::shared_ptr<Entity> object = activeScene->createEntity("Test");
	EXPECT_NE(object.get(), nullptr);
}