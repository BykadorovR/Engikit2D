#include "GraphicSystems.h"
#include "TextureManager.h"

DrawSystem::DrawSystem(std::shared_ptr<BufferManager> bufferManager) {
	_bufferManager = bufferManager;
}

void DrawSystem::vertexUpdate(std::shared_ptr<ObjectComponent> vertexObject) {
	glUseProgram(vertexObject->getShader()->getProgram());
	_bufferManager->activateBuffer();
}

void DrawSystem::textureUpdate(std::shared_ptr<TextureComponent> textureObject) {
	glUniform1f(/*u_AdjustX*/ 2, 0);
	glUniform1f(/*u_AdjustY*/ 3, 0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTextureAtlas(textureObject->getTextureID())->getTextureObject());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	_bufferManager->deactivateBuffer();
}

// Called every game update
void DrawSystem::update(std::shared_ptr<EntityManager> entityManager) {
	for (auto entity : entityManager->getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		if (vertexObject)
			vertexUpdate(vertexObject);

		auto textureObject = entity->getComponent<TextureComponent>();
		if (textureObject)
			textureUpdate(textureObject);
	}
}