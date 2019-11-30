#pragma once

#include "Scene.h"

class SceneManager {
private:
	std::vector<std::shared_ptr<Scene> > _scenes;
	std::shared_ptr<Scene> _activeScene;
public:
	SceneManager();
	std::shared_ptr<Scene> createScene(std::string sceneName);
	bool loadScene(std::string fileName);
	bool saveActiveScene(std::string fileName);
	bool changeScene(std::string sceneName);
	std::shared_ptr<Scene> getActiveScene();
	bool removeScene(std::string sceneName);
};