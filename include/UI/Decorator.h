#pragma once
#include "Label.h"
#include "View.h"

class ScrollerVerticalDecorator : public View {
public:
	ScrollerVerticalDecorator(std::string name = "ScrollerVerticalDecorator");
	bool initialize();

	bool setScrollerUp(std::shared_ptr<View> scrollerUp);
	bool setScrollerDown(std::shared_ptr<View> scrollerDown);
	bool setScrollerProgress(std::shared_ptr<View> scrollerProgress);
	std::shared_ptr<Back> getScrollerUp();
	std::shared_ptr<Back> getScrollerDown();
	std::shared_ptr<Back> getScrollerProgress();
};

class ScrollerVerticalDecoratorFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
public:
	ScrollerVerticalDecoratorFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "ScrollerVerticalDecorator", std::shared_ptr<View> parent = nullptr);
};
