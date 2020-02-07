#pragma once
#include "View.h"
#include "Scene.h"
#include "Shader.h"
#include "GlyphsLoader.h"
#include "GraphicComponents.h"
#include "Back.h"

class Label: public View {
private:
	std::vector<std::shared_ptr<View> > _views;
public:
	Label(std::string name);
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::wstring text, std::shared_ptr<GlyphsLoader> glyphs, std::shared_ptr<Shader> shader);
	bool setColor(std::vector<float> color);
	bool setScale(float scale);
	bool setPageNumber(int pageNumber);
	bool setTextAllignment(std::tuple<TextAllignment, TextAllignment> allignment);
	bool setLineSpacingCoeff(float coeff);
	bool setScrollerUp(std::shared_ptr<View> scrollerUp);
	bool setScrollerDown(std::shared_ptr<View> scrollerDown);
	std::shared_ptr<Back> getScrollerUp();
	std::shared_ptr<Back> getScrollerDown();
};

class LabelFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
public:
	LabelFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "Label");
};