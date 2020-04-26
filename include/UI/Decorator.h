#pragma once
#include "Label.h"
#include "View.h"

class ScrollerDecorator : public View {
private:
	std::shared_ptr<View> _parent;
	std::vector<std::shared_ptr<View> > _views;
public:
	ScrollerDecorator(std::string name = "ScrollerDecorator", std::shared_ptr<View> parent = nullptr);
	bool initialize();

	std::vector<std::shared_ptr<View> > getViews();
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
	std::shared_ptr<View> createView(std::string name = "ScrollerDecorator", std::shared_ptr<View> parent = nullptr);
};