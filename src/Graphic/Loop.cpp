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

shared_ptr<DrawSystem> drawSystem;
shared_ptr<Entity> sprite;
shared_ptr<Entity> animatedSprite;
void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(4096, 4096);
	Texture textureRaw("../data/textures/air_hockey_surface.png", 0, 0, atlas);
	Texture textureAnim("../data/textures/firstmain_idle.png", 0, 1024, 1, 3, atlas);
	Texture textureRawTest("../data/textures/firstmain_idle.png", 0, 2048, atlas);

	atlas->loadAtlas();
	Shader shader, shaderAnim;
	auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
	auto programAnim = shaderAnim.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");

	World world;
	drawSystem = world.createSystem<DrawSystem>();

	sprite = world.createEntity();
	sprite->createComponent<ObjectComponent>()->initialize(200, 100, 100, 100, program);
	sprite->createComponent<TextureComponent>()->initialize(textureRaw, program);
//	sprite->createComponent<TransformComponent>()->initialize(program);

	animatedSprite = world.createEntity();
	animatedSprite->createComponent<ObjectComponent>()->initialize(200, 400, 400, 100, program);
	animatedSprite->createComponent<TextureComponent>()->initialize(textureRawTest, program);
	//animatedSprite->createComponent<AnimatedTextureComponent>()->initialize(textureAnim, { 0, 1, 2, 1 }, { 17, 8, 17, 8 }, _programAnimated);
	//animatedSprite->createComponent<TransformComponent>()->initialize(_programAnimated);
}

void update(int value) {
	glutPostRedisplay();  // Redraw windows
	glutTimerFunc(40, update, 0);
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Matrix2D transform;
	//transform.translate(1, 0);
	//sprite->getComponent<TransformComponent>()->setTransform(transform);
	//animatedSprite->getComponent<TransformComponent>()->setTransform(transform);

	//animatedTextureSystem->update();
	//transformSystem->update();
	drawSystem->update();
	glutSwapBuffers(); // Flush drawing commands
}