#include "Loop.h"
#include "Buffer.h"
#include "PlatformGL.h"
#include "Texture.h"
#include "Sprite.h"

void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Sprite* hockey;
Sprite* hockey2;
void on_surface_changed() {
	std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(2048, 2048);
	Texture textureRaw("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/air_hockey_surface.png", 0, 0, atlas);
	atlas->loadAtlas();
	hockey = new Sprite(0, 0, 100, 100, textureRaw);
	hockey2 = new Sprite(0, 0, 100, 100, textureRaw);
	hockey->translate(-100, 100);
//	hockey2->translate(0, 0);
	hockey->attach();
	hockey2->attach();
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hockey->draw();
	hockey2->draw();

	glutSwapBuffers(); // Flush drawing commands
}