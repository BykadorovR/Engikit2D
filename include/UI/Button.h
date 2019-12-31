#pragma once
#include "GraphicComponents.h"
#include "View.h"
#include "Scene.h"
#include "Label.h"
#include "Back.h"

class Button : public View {
private:
	std::shared_ptr<Label> _label;
	std::shared_ptr<Back> _back;
public:
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<float> backColor, std::wstring text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader, std::shared_ptr<Shader> shader);
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, int backTextureID, std::wstring text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader, std::shared_ptr<Shader> shader);
	bool setLabel(std::shared_ptr<Label> label);
	bool setBack(std::shared_ptr<Back> back);
};
	
class ButtonFactory : public ViewFactory {
private:
	std::shared_ptr<LabelFactory> _labelFactory;
	std::shared_ptr<BackFactory> _backFactory;
public:
	ButtonFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView();
};