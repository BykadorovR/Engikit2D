#include "SceneManager.h"
#include <algorithm>

SceneManager::SceneManager() {
	_activeScene = nullptr;
}

std::shared_ptr<Scene> SceneManager::createScene(std::string sceneName) {
	std::shared_ptr<Scene> scene = std::make_shared<Scene>(sceneName);
	_scenes.push_back(scene);
	return scene;
}

bool SceneManager::loadScene(std::string fileName) {
	return true;
}

bool SceneManager::saveActiveScene(std::string fileName) {
	return true;
}

bool SceneManager::changeScene(std::string sceneName) {
	auto sceneIterator = std::find_if(_scenes.begin(), _scenes.end(),
			  [sceneName](std::shared_ptr<Scene> scene) { return scene->getName() == sceneName; });
	//if scene wasn't found return error
	if (sceneIterator == _scenes.end())
		return true;

	_activeScene = *sceneIterator;
	return false;
}

std::shared_ptr<Scene> SceneManager::getActiveScene() {
	return _activeScene;
}

bool SceneManager::removeScene(std::string sceneName) {
	int size = _scenes.size();
	_scenes.erase(std::remove_if(_scenes.begin(), _scenes.end(),
		[sceneName](std::shared_ptr<Scene> scene) { return scene->getName() == sceneName; }));
	return size == _scenes.size() ? true : false;

}