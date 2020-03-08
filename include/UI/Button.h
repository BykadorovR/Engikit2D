#pragma once
#include "GraphicComponents.h"
#include "View.h"
#include "Scene.h"
#include "Label.h"
#include "Back.h"

class Button : public View {
private:
	std::vector<std::shared_ptr<View> > _views;
public:
	Button(std::string name = "Button");
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<float> backColor, std::string text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader);
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, int backTextureID, std::string text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader);
	bool setLabel(std::shared_ptr<View> label);
	bool setBack(std::shared_ptr<View> back);
	std::shared_ptr<Label> getLabel();
	std::shared_ptr<Back> getBack();
	std::vector<std::shared_ptr<View> > getViews();
};
	
class ButtonFactory : public ViewFactory {
private:
	std::shared_ptr<LabelFactory> _labelFactory;
	std::shared_ptr<BackFactory> _backFactory;
public:
	ButtonFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "Button");
};