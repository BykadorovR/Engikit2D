#include "General.h"
#include "World.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"
#include "Events.h"
#include "PlatformGL.h"
#include "Loop.h"
#include "Common.h"
#include "Events.h"

//need to separate to cpp and h due to a lot of dependencies between classes
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(resolution.first, resolution.second);
	glutCreateWindow("2D Image Texture");
	if (int status = glewInit() != GLEW_OK)
	{
		printf("Error in glewInit %d\n", status);

		return 1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glutDisplayFunc(&on_draw_frame);
	auto mouseEvent = MouseEvent::initialize();
	glutPassiveMotionFunc(&mouseEvent.mouseCursorPassive);
	glutMouseFunc(&mouseEvent.mousePress);
	glutTimerFunc(33, update, 0);
	on_surface_created();
	on_surface_changed();
	glutMainLoop();
	return 0;
}
