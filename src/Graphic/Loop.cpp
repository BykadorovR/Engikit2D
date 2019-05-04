#include "Loop.h"
#include "Buffer.h"
#include "PlatformGL.h"
#include "Texture.h"
#include "Entity.h"
#include "World.h"
#include "GraphicSystem.h"
#include "UISystem.h"
#include "Events.h"

/*
TODO: 
collision component
move GraphicComponent impl to cpp file
add rotate and scale functions
create windows with buttons
*/


void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

World world;

shared_ptr<Entity> createSprite(int x, int y, int width, int height, Texture texture) {
	shared_ptr<Entity> sprite;
	Shader shader;
	auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	sprite = world.createEntity();
	sprite->createComponent<ObjectComponent>()->initialize(x, y, width, height, program);
	sprite->createComponent<TextureComponent>()->initialize(texture, program);
	sprite->createComponent<TransformComponent>()->initialize(program);
	return sprite;
}

shared_ptr<Entity> createAnimatedSprite(int x, int y, int width, int height,
										std::vector<int> tiles, std::vector<int> latency, Texture texture) {
	shared_ptr<Entity> sprite;
	Shader shader;
	auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	sprite = world.createEntity();
	sprite->createComponent<ObjectComponent>()->initialize(x, y, width, height, program);
	sprite->createComponent<TransformComponent>()->initialize(program);
	sprite->createComponent<AnimatedTextureComponent>()->initialize(texture, tiles, latency, program);
	return sprite;
}

int transform(float shiftX, float shiftY, shared_ptr<Entity> object) {
	Matrix2D transform;
	transform.translate(shiftX, shiftY);
	auto component = object->getComponent<TransformComponent>();
	if (component) {
		component->setTransform(transform);
		return 0;
	}
	return -1;
}



shared_ptr<DrawSystem> drawSystem;
shared_ptr<ClickInsideSystem> clickInsideSystem;
shared_ptr<PointMoveSystem> pointMoveSystem;
shared_ptr<Entity> animatedSprite, staticSprite;


void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(4096, 4096);
	Texture textureRaw("../data/textures/air_hockey_surface.png", 0, 0, atlas);
	Texture textureAnim("../data/textures/firstmain_idle.png", 0, 1024, 1, 3, atlas);

	atlas->loadAtlas();

	staticSprite = createSprite(100, 0, 100, 100, textureRaw);
	staticSprite->createComponent<ClickInsideComponent>()->initialize();

	animatedSprite = createAnimatedSprite(100, 200, 200, 200, { 0, 1, 2, 1 }, { 17, 8, 17, 8 }, textureAnim);
	animatedSprite->getComponent<TransformComponent>()->_speed = 1;
	animatedSprite->createComponent<PointMoveComponent>()->initialize();

	drawSystem = world.createSystem<DrawSystem>();
	pointMoveSystem = world.createSystem<PointMoveSystem>();
	clickInsideSystem = world.createSystem<ClickInsideSystem>();

}

void update(int value) {
	glutPostRedisplay();  // Redraw windows
	glutTimerFunc(40, update, 0);
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	clickInsideSystem->update();
	pointMoveSystem->update();
	drawSystem->update();
	glutSwapBuffers(); // Flush drawing commands
}