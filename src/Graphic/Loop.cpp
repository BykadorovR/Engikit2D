#include "Loop.h"
#include "Buffer.h"
#include "PlatformGL.h"
#include "Texture.h"
#include "Entity.h"
#include "World.h"
#include "GraphicSystem.h"
#include "UISystem.h"
#include "Events.h"
#include "ComponentFunctors.h"
#include "TextLoader.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

World world;

shared_ptr<Entity> createSprite(int x, int y, int width, int height, std::shared_ptr<Texture> texture) {
	shared_ptr<Entity> sprite;
	Shader shader;
	auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	sprite = world.createEntity();
	sprite->createComponent<ObjectComponent>()->initialize(x, y, width, height, program);
	sprite->createComponent<TextureComponent>()->initialize(texture, program);
	//sprite->createComponent<TransformComponent>()->initialize(program);
	return sprite;
}

shared_ptr<DrawSystem> drawSystem;
shared_ptr<MouseSystem> mouseSystem;
shared_ptr<InteractionAddToSystem> interactionAddToSystem;
shared_ptr<SaveLoadSystem> saveLoadSystem;
shared_ptr<Entity> animatedSprite, staticSprite, newSprite, textureSprite, loadSaveSprite;
shared_ptr<CameraSystem> cameraSystem;
shared_ptr<MoveSystem> moveSystem;
TextLoader textLoader;
void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(4096, 4096);
	std::shared_ptr<Texture> textureRaw = std::make_shared<Texture>("../data/textures/air_hockey_surface.png", 0, 0, atlas);
	std::shared_ptr<Texture> textureAnim = std::make_shared<Texture>("../data/textures/firstmain_idle.png", 0, 1024, 1, 3, atlas);

	atlas->initializeAtlas();
	registerComponentFunctors();

	textLoader.bufferSymbols();
	textLoader.initialize();

	newSprite = createSprite(100, 0, 100, 100, textureRaw);
	newSprite->createComponent<ClickClickMoveComponent>()->initialize(false, false);
	newSprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Engine");
	newSprite->createComponent<InteractionCreateEntityComponent>()->initialize();
	newSprite->getComponent<InteractionCreateEntityComponent>()->_createFunctor = [](std::tuple<int, int> coords) -> std::shared_ptr<Entity> {
		shared_ptr<Entity> sprite;
		Shader shader;
		auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
		sprite = world.createEntity();
		float speed;
		std::cout << "Speed coef from camera speed for this object" << std::endl;
		std::cin >> speed;
		int objectWidth, objectHeight;
		std::cout << "Enter width and height of object:" << std::endl;
		std::cin >> objectWidth >> objectHeight;

		std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
		objectComponent->initialize(std::get<0>(coords), std::get<1>(coords), objectWidth, objectHeight, program);
		objectComponent->_cameraCoefSpeed = speed;
		sprite->addComponent(objectComponent);

		//TODO: path to texture?
		sprite->createComponent<TextureComponent>()->initialize(program);
		bool moveToByClick;
		std::cout << "Move toward this object allowed?" << std::endl;
		std::cin >> moveToByClick;
		sprite->createComponent<ClickInsideComponent>()->initialize(moveToByClick);

		int groupID = 0;
		std::cout << "Enter the group ID" << std::endl;
		std::cin >> groupID;
		std::string groupName;
		std::cout << "Enter the group name" << std::endl;
		std::cin >> groupName;
		sprite->createComponent<GroupEntitiesComponent>()->initialize(groupID, groupName);
		sprite->createComponent<InteractionAddToEntityComponent>()->initialize(InteractionMember::OBJECT);
		
		
		return sprite;
	};
	newSprite->getComponent<InteractionCreateEntityComponent>()->_removeFunctor = [](int entityID) -> void {
		world.unregisterEntity(entityID);
	};


	staticSprite = createSprite(200, 0, 100, 100, textureRaw);
	staticSprite->createComponent<ClickClickMoveComponent>()->initialize(false, false);
	staticSprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Engine");
	staticSprite->createComponent<InteractionAddToEntityComponent>()->initialize(InteractionMember::SUBJECT);
	
	textureSprite = createSprite(300, 0, 100, 100, textureRaw);
	textureSprite->createComponent<ClickInsideComponent>()->initialize(false);
	textureSprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Engine");
	textureSprite->createComponent<TextureManagerComponent>()->initialize();

	loadSaveSprite = createSprite(400, 0, 100, 100, textureRaw);
	loadSaveSprite->createComponent<ClickInsideComponent>()->initialize(false);
	loadSaveSprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Engine");
	loadSaveSprite->createComponent<SaveLoadComponent>()->initialize();

	drawSystem = world.createSystem<DrawSystem>();
	mouseSystem = world.createSystem<MouseSystem>();
	interactionAddToSystem = world.createSystem<InteractionAddToSystem>();
	saveLoadSystem = world.createSystem<SaveLoadSystem>();
	moveSystem = world.createSystem<MoveSystem>();
	cameraSystem = world.createSystem<CameraSystem>();
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cameraSystem->update(world.getEntityManager());
	moveSystem->update(world.getEntityManager());
	mouseSystem->update(world.getEntityManager());
	drawSystem->update(world.getEntityManager());
	interactionAddToSystem->update(world.getEntityManager());
	saveLoadSystem->update(world.getEntityManager());

	textLoader.render("Test", 300, 300, 1, { 0.5f, 0.8f, 0.2f });
}
