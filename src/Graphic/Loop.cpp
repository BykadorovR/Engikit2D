#include "Loop.h"
#include "Buffer.h"
#include "PlatformGL.h"
#include "Texture.h"
#include "Sprite.h"

// position X, Y, texture S, T
static const float rect[] = { -1.0f, -1.0f, 0.0f, 0.0f,
							 -1.0f,  1.0f, 0.0f, 1.0f,
							  1.0f, -1.0f, 1.0f, 0.0f,
							  1.0f,  1.0f, 1.0f, 1.0f };

void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Sprite* hockey;
void on_surface_changed() {
	Texture atlas(2048, 2048);
	Texture textureRaw("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/air_hockey_surface.png");
	atlas.addTexture(textureRaw, 512, 512);
	atlas.addTexture(textureRaw, 1024, 512);
	hockey = new Sprite(0, 0, 512, 512, textureRaw);
	hockey->attach();

}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hockey->draw();
	glutSwapBuffers(); // Flush drawing commands
}