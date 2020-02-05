#pragma once
#include "View.h"
#include "Scene.h"
#include "Shader.h"
#include "GlyphsLoader.h"
#include "GraphicComponents.h"

struct LabelOptions {

};

class Label: public View {
public:
	Label();
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::wstring text, std::shared_ptr<GlyphsLoader> glyphs, std::shared_ptr<Shader> shader);
	bool setColor(std::vector<float> color);
	bool setScale(float scale);
	bool setPageNumber(int pageNumber);
	bool setTextAllignment(std::tuple<TextAllignment, TextAllignment> allignment);
	bool setLineSpacingCoeff(float coeff);
};

class LabelFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
public:
	LabelFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView();
};