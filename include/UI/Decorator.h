#pragma once
#include "Label.h"
#include "View.h"

class ScrollerVerticalDecorator : public View {
private:
	bool applyToGrid();
	bool applyToList();
	bool applyToLabel();
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

class HeaderDecorator : public View {
private:
	bool applyToGrid();
	bool applyToList();
	bool applyToLabel();
public:
	HeaderDecorator(std::string name = "HeaderDecorator");
	bool initialize();
	bool setText(std::vector<std::string> text);
	std::shared_ptr<Grid> getBack();
	std::shared_ptr<Grid> getLabel();
};

class HeaderDecoratorFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
	std::shared_ptr<BackFactory> _backFactory;
	std::shared_ptr<LabelFactory> _labelFactory;
public:
	HeaderDecoratorFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView(std::string name = "HeaderDecorator", std::shared_ptr<View> parent = nullptr);
	std::shared_ptr<View> createGrid(std::tuple<int, int> grid, std::string name = "HeaderDecorator", std::shared_ptr<View> parent = nullptr);
};