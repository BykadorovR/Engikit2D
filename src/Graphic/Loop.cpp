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
void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(4096, 4096);
	std::shared_ptr<Texture> textureRaw = std::make_shared<Texture>("../data/textures/air_hockey_surface.png", 0, 0, atlas);
	std::shared_ptr<Texture> textureAnim = std::make_shared<Texture>("../data/textures/firstmain_idle.png", 0, 1024, 1, 3, atlas);

	atlas->initializeAtlas();
	registerComponentFunctors();

	newSprite = createSprite(100, 0, 100, 100, textureRaw);
	newSprite->createComponent<ClickClickMoveComponent>()->initialize(false, false);
	newSprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Engine");
	newSprite->createComponent<InteractionCreateEntityComponent>()->initialize();
	newSprite->getComponent<InteractionCreateEntityComponent>()->_createFunctor = [](std::tuple<int, int> coords) -> std::shared_ptr<Entity> {
		shared_ptr<Entity> sprite;
		Shader shader;
		auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
		sprite = world.createEntity();
		std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
		objectComponent->initialize(std::get<0>(coords), std::get<1>(coords), 100, 100, program);
		objectComponent->_cameraCoefSpeed = 1;
		sprite->addComponent(objectComponent);

		//TODO: path to texture?
		sprite->createComponent<TextureComponent>()->initialize(program);
		std::shared_ptr<ClickInsideComponent> clickInsideComponent(new ClickInsideComponent());
		clickInsideComponent->initialize();
		std::shared_ptr<ComponentTextEvent> textEvent = std::make_shared<ComponentTextEvent>();
		clickInsideComponent->_event = std::make_pair(textEvent, sprite);
		sprite->addComponent(clickInsideComponent);

		int groupID = 1;
		std::string groupName = "Default";
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
	textureSprite->createComponent<TextureManagerComponent>();

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
}
