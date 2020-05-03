#pragma once
#include "GraphicComponents.h"
#include "View.h"
#include "Action.h"
#include "Scene.h"
#include "Label.h"
#include "Back.h"
#include "InteractionOperations.h"

class Button : public View {
private:
	std::shared_ptr<ExpressionOperation> _clickInside;
public:
	Button(std::string name = "Button");
	bool initialize();

	bool setTexture(int textureID);
	bool addClickAction(std::shared_ptr<Action> action);
	bool setLabel(std::shared_ptr<View> label);
	bool setBack(std::shared_ptr<View> back);
	std::shared_ptr<Label> getLabel();
	std::shared_ptr<Back> getBack();
};
	
class ButtonFactory : public ViewFactory {
private:
	std::shared_ptr<LabelFactory> _labelFactory;
	std::shared_ptr<BackFactory> _backFactory;
	std::shared_ptr<Scene> _activeScene;
public:
	ButtonFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "Button", std::shared_ptr<View> parent = nullptr);
};