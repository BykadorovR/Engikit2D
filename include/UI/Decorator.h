#pragma once
#include "Label.h"
#include "View.h"

class ScrollerDecorator : public View {
public:
	ScrollerDecorator(std::string name = "ScrollerDecorator");
	bool initialize(std::shared_ptr<Label> label);

	bool setScrollerUp(std::shared_ptr<View> scrollerUp);
	bool setScrollerDown(std::shared_ptr<View> scrollerDown);
	bool setScrollerProgress(std::shared_ptr<View> scrollerProgress);
	std::shared_ptr<Back> getScrollerUp();
	std::shared_ptr<Back> getScrollerDown();
	std::shared_ptr<Back> getScrollerProgress();
};

class ScrollerDecoratorFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
public:
	ScrollerDecoratorFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "ScrollerDecorator");
};

enum ViewAppearanceState {
	REGISTER = 0,
	UNREGISTER = 1
};

class AppearanceDecorator : public View {
private:
	//conditions for view register/unregister
	std::vector<std::tuple<std::shared_ptr<Operation>, ViewAppearanceState> > _appearanceConditions;
public:
	AppearanceDecorator(std::string name = "AppearanceDecorator");
	bool initialize(std::shared_ptr<View> parentView);

	bool addAppearanceCondition(std::tuple<std::shared_ptr<Operation>, ViewAppearanceState> appearanceCondition);

	bool registerViewAction();
	bool unregisterViewAction();
};