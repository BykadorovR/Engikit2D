#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Interface.h"
#include "Grid.h"
#include "State.h"
#include "UserInputEvents.h"
#include "TextureAtlas.h"
#include "Texture.h"
#include "Entity.h"
#include "SceneManager.h"
#include "GraphicComponents.h"
#include "GraphicSystems.h"
#include "TextureManager.h"
#include "GlyphsLoader.h"
#include "Common.h"
#include "UIActions.h"
#include "Texture.h"
#include "Back.h"
#include "Label.h"
#include "InteractionOperations.h"
#include "UserInputComponents.h"
#include "InteractionComponents.h"
#include "InteractionSystems.h"
#include "InteractionActions.h"
#include "UserInputSystems.h"
#include "Decorator.h"
#include "List.h"
#include "CustomComponents.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float Zoom = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

GLuint vbo, vao, texture, textureSpecular;
GLuint lightVAO;
std::shared_ptr<Shader> shader, lightShader;
void surfaceCreated() {
	Assimp::Importer importer;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	// Order of coordinates: X, Y
	// 3      2
	// |   \  |
	// 0,4    1
	//draw every object in the center and after transform it with model transforms + draw result
	std::vector<float> data = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(/*location = 0*/0);
	glVertexAttribPointer(/*location = 0*/0, /*position count*/3, GL_FLOAT, GL_FALSE, /*stride*/ 8 * sizeof(float), /*start*/ 0);

	glEnableVertexAttribArray(/*location = 1*/1);
	glVertexAttribPointer(/*location = 1*/1, /*position count*/2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(/*location = 2*/2);
	glVertexAttribPointer(/*location = 2*/2, /*position count*/3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	shader = std::make_shared<Shader>("../data/shaders/shader.vert", "../data/shaders/shader.frag");
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		auto imageLoader = std::shared_ptr<ImageLoader>(new ImageLoader);
		imageLoader->loadPNG("../data/textures/container2.png");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, std::get<0>(imageLoader->getSize()), std::get<1>(imageLoader->getSize()), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader->getData().data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	{
		glGenTextures(1, &textureSpecular);
		glBindTexture(GL_TEXTURE_2D, textureSpecular);
		auto imageLoader = std::shared_ptr<ImageLoader>(new ImageLoader);
		imageLoader->loadPNG("../data/textures/container2_specular.png");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, std::get<0>(imageLoader->getSize()), std::get<1>(imageLoader->getSize()), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader->getData().data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	lightShader = std::make_shared<Shader>("../data/shaders/light.vert", "../data/shaders/light.frag");
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// set the vertex attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}


std::vector<float> lightColor = { 1.0f, 1.0f, 1.0f };
glm::vec3 lightPos(1.2f, 0.0f, 2.0f);
void drawFrame() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
	//lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	//lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
	//Object
	glUseProgram(shader->getProgram());
	glUniform1i(glGetUniformLocation(shader->getProgram(), "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shader->getProgram(), "material.specular"), 1);

	glUniform3fv(glGetUniformLocation(shader->getProgram(), "material.specular"), 1, std::vector<float>({0.5f, 0.5f, 0.5f}).data());
	glUniform1f(glGetUniformLocation(shader->getProgram(), "material.shininess"), 32.0f);

	glUniform3fv(glGetUniformLocation(shader->getProgram(), "light.ambient"), 1, std::vector<float>({ 0.2f, 0.2f, 0.2f }).data());
	glUniform3fv(glGetUniformLocation(shader->getProgram(), "light.diffuse"), 1, std::vector<float>({ 0.8f, 0.8f, 0.8f }).data());
	glUniform3fv(glGetUniformLocation(shader->getProgram(), "light.specular"), 1, std::vector<float>({ 1.0f, 1.0f, 1.0f }).data());
	glUniform3fv(glGetUniformLocation(shader->getProgram(), "light.position"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shader->getProgram(), "viewPos"), 1, glm::value_ptr(cameraPos));
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 proj = glm::mat4(1.0f);
	//if FOV increases, more objects in such frustum can be maped to near -> screen, so it's like unzoom world
	proj = glm::perspective(glm::radians(fov), (float)std::get<0>(currentResolution) / (float)std::get<1>(currentResolution), 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureSpecular);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	//Light
	glUseProgram(lightShader->getProgram());
	glUniformMatrix4fv(glGetUniformLocation(lightShader->getProgram(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightShader->getProgram(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(proj));
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader->getProgram(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

//need to separate to cpp and h due to a lot of dependencies between classes
int main(int argc, char **argv) {
	FreeConsole();
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core profile deprecate all fixed function API calls

	GLFWwindow* mainWindow;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//mainWindow = glfwCreateWindow(std::get<0>(currentResolution), std::get<1>(currentResolution), "Main window", monitor, NULL);
	mainWindow = glfwCreateWindow(std::get<0>(currentResolution), std::get<1>(currentResolution), "Main window", NULL, NULL);
	if (mainWindow == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}
	//disable VSync
	//glfwSwapInterval(0);
	//wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(mainWindow, mouse_callback);
	glfwSetScrollCallback(mainWindow, scroll_callback);

	glfwSetMouseButtonCallback(mainWindow, mousePressed);
	glfwSetKeyCallback(mainWindow, processInput);
	glfwSetCharCallback(mainWindow, textInput);
	//if size of window has changed we need to notify OpenGL that "working" area changed via glViewport
	glfwSetFramebufferSizeCallback(mainWindow, [](GLFWwindow* window, int width, int height){ glViewport(0, 0, width, height); });
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	surfaceCreated();
	while (!glfwWindowShouldClose(mainWindow)) {
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		//TODO: separate display and bussness logic (display will be at 60 FPS, but business w/o restriction)
		//create buffer for rendering
		drawFrame();
		//double buffer - one for displaying, one for current render, once filled it's displayed. Needed to avoid flickering due to writing to displayed buffer
		glfwSwapBuffers(mainWindow);
		//checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls the corresponding functions
		glfwPollEvents();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - startTime).count();
		OUT_STREAM("Elapsed: " + std::to_string(elapsed) + " ms\n");
	}

	glfwDestroyWindow(mainWindow);
	glfwTerminate();	
	return 0;
}