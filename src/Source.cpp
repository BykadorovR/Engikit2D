#include "General.h"
#include "World.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"

#include "PlatformGL.h"
#include "Loop.h"
class PositionComponent : public Component {
public:
	double x;
	double y;
};

class MovementComponent : public Component {
public:
	double dx;
	double dy;
};

class InputComponent : public Component {
public:
	enum keyCodes {
		W_KEY,
		S_KEY,
		A_KEY,
		D_KEY
	};

	keyCodes pressedKey;
};

class CollisionComponent : public Component {
public:
	//Size of bounding box for collision
	double wx;
	double wy;
};


//should be template with variadic number of parameters for component set
class MovementSystem : public System {
public:
	void update(int dt) {
		for (auto entity : getEntities()) {
			auto position = entity->getComponent<PositionComponent>();
			auto  movement = entity->getComponent<MovementComponent>();
			if (!position || !movement)
				continue;
			position->x += movement->dx;
			position->y += movement->dy;
		}
	}
};

//need to separate to cpp and h due to a lot of dependencies between classes
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(720, 480);
	glutCreateWindow("2D Image Texture");
	if (int status = glewInit() != GLEW_OK)
	{
		printf("Error in glewInit %d\n", status);

		return 1;
	}
	glutDisplayFunc(&on_draw_frame);
	on_surface_created();
	on_surface_changed();
	glutMainLoop();
	return 0;
}
