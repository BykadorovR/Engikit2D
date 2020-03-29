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
	Label(std::string name);
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::string text, std::shared_ptr<GlyphsLoader> glyphs);
	bool setColor(std::vector<float> color);
	bool setScale(float scale);
	std::tuple<float, float> getPosition();
	std::tuple<float, float> getSize();

	bool setPageNumber(int pageNumber);
	bool setTextAllignment(std::tuple<TextAllignment, TextAllignment> allignment);
	bool setLineSpacingCoeff(float coeff);
};

class LabelFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
public:
	LabelFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "Label");
};