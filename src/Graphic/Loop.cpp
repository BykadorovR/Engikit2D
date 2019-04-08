#include "Loop.h"
#include "Buffer.h"
#include "PlatformGL.h"
#include "Texture.h"
#include "Sprite.h"
#include "Entity.h"
#include "World.h"
#include "GraphicSystem.h"

void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Object* hockey;
AnimatedSprite* anim;

shared_ptr<ObjectSystem> objectSystem;
shared_ptr<TextureSystem> textureSystem;
shared_ptr<TransformSystem> transformSystem;
shared_ptr<Entity> sprite;
void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(4096, 4096);
	Texture textureRaw("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/air_hockey_surface.png", 0, 0, atlas);
	Texture textureAnim("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/firstmain_idle.png", 1024, 1024, 1, 3, atlas);
	atlas->loadAtlas();
	Shader shader;
	auto _program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	
	World world;
	objectSystem = world.createSystem<ObjectSystem>();
	textureSystem = world.createSystem<TextureSystem>();
	transformSystem = world.createSystem<TransformSystem>();
	sprite = world.createEntity();
	sprite->createComponent<ObjectComponent>()->initialize(200, 100, 100, 100, _program);
	sprite->createComponent<TextureComponent>()->initialize(textureRaw, _program);
	sprite->createComponent<TransformComponent>()->initialize(_program);
}

void update(int value) {
	glutPostRedisplay();  // Redraw windows
	glutTimerFunc(40, update, 0);
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//hockey->draw();
	//anim->draw();
	objectSystem->update();
	textureSystem->update();
	Matrix2D transform;
	transform.translate(1, 0);
	transform.print();
	sprite->getComponent<TransformComponent>()->_transform = sprite->getComponent<TransformComponent>()->_transform * transform;
	transform.print();
	transformSystem->update();
	glutSwapBuffers(); // Flush drawing commands
}