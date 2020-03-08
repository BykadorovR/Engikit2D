#pragma once
#include "Label.h"
#include "View.h"

class ScrollerDecorator : public View {
private:
	std::vector<std::shared_ptr<View> > _views;
public:
	ScrollerDecorator(std::string name);
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