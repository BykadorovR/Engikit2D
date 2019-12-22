#include "View.h"
#include "Scene.h"
#include "Shader.h"

class Back : public View {
public:
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, int textureID, std::shared_ptr<Shader> shader);
	bool initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<float> color, std::shared_ptr<Shader> shader);
};

class BackFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
public:
	BackFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView();
};