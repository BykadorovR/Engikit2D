#include "GraphicSystem.h"

void ObjectSystem::draw(std::shared_ptr<ObjectComponent> object) {
	glUseProgram(object->_program);

	//bind buffer and handle vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(object->_aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(object->_aPositionLocation);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ObjectSystem::update() {
	for (auto entity : getEntities()) {
		auto object = entity->getComponent<ObjectComponent>();
		if (!object)
			continue;
		draw(object);
	}
}

void TextureSystem::draw(std::shared_ptr<TextureComponent> object) {
	glUseProgram(object->_program);
	//bind buffer and handle texture shader
	glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(object->_aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(object->_aTextureCoordinatesLocation);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//bind texture and handle fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->_textureID);
	glUniform1i(object->_uTextureUnitLocation, 0);
	
}

void TextureSystem::update() {
	for (auto entity : getEntities()) {
		auto object = entity->getComponent<TextureComponent>();
		if (!object)
			continue;
		draw(object);
	}
}

void TransformSystem::draw(std::shared_ptr<TransformComponent> object) {
	glUseProgram(object->_program);
	glUniformMatrix4fv(object->_uMatrixLocation, 1, false, object->_transform.getData());
}

void TransformSystem::update() {
	for (auto entity : getEntities()) {
		auto transform = entity->getComponent<TransformComponent>();
		draw(transform);
	}
};