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
		auto texture = entity->getComponent<TextureComponent>();
		glUseProgram(object->_program);

		//bind buffer and handle vertex shader
		glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
		//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
		glVertexAttribPointer(object->_aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(object->_aPositionLocation);

		glBindBuffer(GL_ARRAY_BUFFER, texture->_buffer.getVBOObject());
		glVertexAttribPointer(texture->_aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(texture->_aTextureCoordinatesLocation);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//bind texture and handle fragment shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->_textureID);
		glUniform1i(texture->_uTextureUnitLocation, 0);
		//glUniformMatrix4fv(_uMatrixLocation, 1, false, _transform.getData());
	}
}

void TextureSystem::draw(std::shared_ptr<TextureComponent> object) {
	/*glUseProgram(object->_program);
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
	*/
}

void TextureSystem::update() {
	for (auto entity : getEntities()) {
		auto object = entity->getComponent<TextureComponent>();
		if (!object)
			continue;
		draw(object);
	}
}