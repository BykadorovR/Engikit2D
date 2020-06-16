#pragma once
#include "View.h"
#include "Scene.h"
#include "Shader.h"
#include "GlyphsLoader.h"
#include "GraphicComponents.h"
#include "Back.h"
#include "Action.h"

class Label: public View {
public:
	Label(std::string name = "Label");
	bool initialize();
	bool setText(std::string text);
	bool setColor(std::vector<float> color);
	bool setScale(float scale);
	bool setEditable(bool editable);
	bool setPosition(std::tuple<float, float> position);
	bool setSize(std::tuple<float, float> size);
	std::tuple<float, float> getPosition();
	std::tuple<float, float> getSize();

	bool setTextAllignment(std::tuple<TextAllignment, TextAllignment> allignment);
	bool setLineSpacingCoeff(float coeff);
};

//TODO: remove all factories except only one because they are all equal
class LabelFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
public:
	LabelFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "Label", std::shared_ptr<View> parent = nullptr);
	std::shared_ptr<Grid> createGrid(std::tuple<int, int> dim, std::string name = "Grid", std::shared_ptr<View> parent = nullptr);
};