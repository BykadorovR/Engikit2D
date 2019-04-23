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
	//we can use multiple textures, but we use only one, do it active and send textureUnitLocation = 0 to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->_textureID);
	glUniform1i(object->_uTextureUnitLocation, 0);
}

void TextureSystem::update() {
	for (auto entity : getEntities()) {
		auto object = entity->getComponent<TextureComponent>();
		if (!object) {
			continue;
		}
			
		draw(object);
	}
}

void AnimatedTextureSystem::draw(std::shared_ptr<AnimatedTextureComponent> object) {
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

	glUniform1f(object->_uAdjustXLocation, object->_widthTile * object->_tilesOrder[(object->_currentAnimateTile)]);

}

void AnimatedTextureSystem::update() {
	for (auto entity : getEntities()) {
		auto object = entity->getComponent<AnimatedTextureComponent>();
		if (!object)
			continue;
		draw(object);
		if (object->_currentAnimateTile < object->_tilesOrder.size()) {
			if (object->_currentLatency < object->_tilesLatency[object->_currentAnimateTile])
				object->_currentLatency++;
			else {
				object->_currentLatency = 0;
				object->_currentAnimateTile++;
				if (object->_currentAnimateTile == object->_tilesOrder.size())
					object->_currentAnimateTile = 0;
			}
		}
	}
}

void TransformSystem::draw(std::shared_ptr<TransformComponent> object) {
	glUseProgram(object->_program);
	glUniformMatrix4fv(object->_uMatrixLocation, 1, false, object->_result.getData());
}

void TransformSystem::update() {
	for (auto entity : getEntities()) {
		auto transform = entity->getComponent<TransformComponent>();
		if (!transform)
			continue;
		transform->_result = transform->_result * transform->_transform;
		draw(transform);
		transform->_transform.print();
		transform->_transform.identity();
		transform->_transform.print();
	}
};