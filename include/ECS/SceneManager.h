#pragma once

#include "Scene.h"

class SceneManager {
private:
	std::vector<std::shared_ptr<Scene> > _scenes;
	std::shared_ptr<Scene> _activeScene;
public:
	SceneManager();
	bool addScene(std::string fileName);
	bool saveActiveScene(std::string fileName);
	bool changeScene(std::string sceneName);
	std::shared_ptr<Scene> getActiveScene();
	bool removeScene(std::string sceneName);
};