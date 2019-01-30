#include "Loop.h"
#include "Buffer.h"
#include "PlatformGL.h"
#include "Texture.h"
#include "Sprite.h"

void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Object* hockey;
AnimatedSprite* anim;
void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(4096, 4096);
	Texture textureRaw("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/air_hockey_surface.png", 0, 0, atlas);
	Texture textureAnim("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/firstmain_idle.png", 1024, 1024, 1, 3, atlas);
	atlas->loadAtlas();
	hockey = new Sprite(200, 100, 100, 100, textureRaw);
	anim = new AnimatedSprite(200, 200, 100, 200, textureAnim);
	anim->setAnimate({ 0, 1, 2, 1 }, { 17, 8, 17, 8 });
	anim->attach();
	hockey->attach();
}

void update(int value) {
	hockey->translate(1, 0);
	glutPostRedisplay();  // Redraw windows
	glutTimerFunc(40, update, 0);
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hockey->draw();
	anim->draw();
	glutSwapBuffers(); // Flush drawing commands
}