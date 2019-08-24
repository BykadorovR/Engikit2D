#include "System.h"
#include "GraphicComponent.h"

class DrawSystem : public System {
private:
	void draw();
public:
	// Called every game update
	void update(shared_ptr<EntityManager> entityManager);

};

class ObjectSystem : public System {
private:
	void draw(std::shared_ptr<ObjectComponent> object);
public:
	// Called every game update
	void update(shared_ptr<EntityManager> entityManager);
};

class TextureSystem : public System {
private:
	void draw(std::shared_ptr<TextureComponent> object);
public:
	// Called every game update
	void update(shared_ptr<EntityManager> entityManager);
};
