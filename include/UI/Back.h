#pragma once
#include "View.h"
#include "Scene.h"
#include "Shader.h"

struct BackOptions {

};

class Back : public View {
public:
	Back(std::string name = "Back");
	bool initialize();
	bool setColorMask(std::vector<float> mask);
	bool setColorAddition(std::vector<float> addition);
	bool setTexture(int textureID);
	bool setPosition(std::tuple<float, float> position);
	bool setSize(std::tuple<float, float> size);

	std::tuple<float, float> getPosition();
	std::tuple<float, float> getSize();
};

class BackFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
public:
	BackFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "Back", std::shared_ptr<View> parent = nullptr);
};