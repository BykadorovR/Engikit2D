#include "Loop.h"
#include "buffer.h"
#include "image.h"
#include "platformGL.h"
#include "shader.h"
#include "texture.h"

static GLuint texture;
static GLuint buffer;
static GLuint program;

static GLint a_position_location;
static GLint a_texture_coordinates_location;
static GLint u_texture_unit_location;

// position X, Y, texture S, T
static const float rect[] = { -1.0f, -1.0f, 0.0f, 0.0f,
							 -1.0f,  1.0f, 0.0f, 1.0f,
							  1.0f, -1.0f, 1.0f, 0.0f,
							  1.0f,  1.0f, 1.0f, 1.0f };

void on_surface_created() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void on_surface_changed() {
	Texture atlas(2048, 2048);
	Texture texture_raw("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/textures/air_hockey_surface.png");
	join_image_to_result_image(texture_raw, 512, 512, atlas);
	join_image_to_result_image(texture_raw, 1024, 512, atlas);
	texture = load_raw_image_data_into_texture(atlas);
	//We need to delete added to atlas texture (!) and replace code related to creating VBO to separate class and calculate coordinates for
	//displaying as textureSize/atlasSize else we will see not requested texture but atlas
	buffer = createVBO(sizeof(rect), rect, GL_STATIC_DRAW);
	program = build_program_from_assets("C:/Users/Home/Desktop/Engine/TimeOfWitch/data/shaders/shader.vsh", 
		                                "C:/Users/Home/Desktop/Engine/TimeOfWitch/data/shaders/shader.fsh");

	a_position_location = glGetAttribLocation(program, "a_Position");
	a_texture_coordinates_location = glGetAttribLocation(program, "a_TextureCoordinates");
	u_texture_unit_location = glGetUniformLocation(program, "u_TextureUnit");
}

void on_draw_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(u_texture_unit_location, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(a_position_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), BUFFER_OFFSET(0));
	glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), BUFFER_OFFSET(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(a_position_location);
	glEnableVertexAttribArray(a_texture_coordinates_location);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glutSwapBuffers(); // Flush drawing commands
}