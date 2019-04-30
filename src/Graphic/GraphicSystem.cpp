#include "GraphicSystem.h"

//INFO: for now every object use OWN program, all of them use one shader, though. It's convinient enough in sense of 
//programming (no needs to think about uniforms state) but can affect performance.
//If any perf gaps, need to add vertexReset, transformReset, etc functions which will zero uniforms
//so further objects that don't contain such component wont be affected

void vertexUpdate(std::shared_ptr<ObjectComponent> object) {
	glUseProgram(object->_program);

	//bind buffer and handle vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(object->_aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(object->_aPositionLocation);
}

void textureUpdate(std::shared_ptr<TextureComponent> object) {
	//bind buffer and handle texture shader
	glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(object->_aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(object->_aTextureCoordinatesLocation);
	glUniform1f(object->_uAdjustXLocation, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//bind texture and handle fragment shader
	//we can use multiple textures, but we use only one, do it active and send textureUnitLocation = 0 to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->_textureID);
	glUniform1i(object->_uTextureUnitLocation, 0);
}

void animatedTextureUpdate(std::shared_ptr<AnimatedTextureComponent> object) {
	//bind buffer and handle texture shader
	glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(object->_aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(object->_aTextureCoordinatesLocation);
	glUniform1f(object->_uAdjustXLocation, object->_widthTile * object->_tilesOrder[(object->_currentAnimateTile)]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//bind texture and handle fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->_textureID);
	glUniform1i(object->_uTextureUnitLocation, 0);

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

void transformUpdate(std::shared_ptr<TransformComponent> object) {
	object->_result = object->_result * object->_transform;
	glUniformMatrix4fv(object->_uMatrixLocation, 1, false, object->_result.getData());
	object->_transform.identity();
}

void DrawSystem::update() {
	for (auto entity : getEntities()) {
		auto vertexObject = entity->getComponent<ObjectComponent>();
		assert(vertexObject);
		vertexUpdate(vertexObject);

		auto transformTextureObject = entity->getComponent<TransformComponent>();
		if (transformTextureObject)
			transformUpdate(transformTextureObject);

		auto textureObject = entity->getComponent<TextureComponent>();
		if (textureObject)
			textureUpdate(textureObject);

		auto animatedTextureObject = entity->getComponent<AnimatedTextureComponent>();
		if (animatedTextureObject)
			animatedTextureUpdate(animatedTextureObject);
	}
}
