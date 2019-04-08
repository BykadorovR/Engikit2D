#include "System.h"
#include "GraphicComponent.h"

class ObjectSystem : public System {
private:
	void draw(std::shared_ptr<ObjectComponent> object);
public:
	// Called every game update
	void update();
};

class TextureSystem : public System {
private:
	void draw(std::shared_ptr<TextureComponent> object);
public:
	// Called every game update
	void update();
};

class TransformSystem : public System {
private:
	void draw(std::shared_ptr<TransformComponent> object);
public:
	void update();
};