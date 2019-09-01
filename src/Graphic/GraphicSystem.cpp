#include "GraphicSystem.h"
#include "Camera.h"
#include "TextLoader.h"
#include "UIComponent.h"
//INFO: for now every object use OWN program, all of them use one shader, though. It's convinient enough in sense of 
//programming (no needs to think about uniforms state) but can affect performance.
//If any perf gaps, need to add vertexReset, transformReset, etc functions which will zero uniforms
//so further objects that don't contain such component wont be affected

void vertexUpdate(std::shared_ptr<ObjectComponent> object) {
	glUseProgram(object->_program);

	float objectWidthN = (float)object->getWidth() / (float)resolution.first;
	float objectHeightN = (float)object->getHeight() / (float)resolution.second;
	float startX = (float)(object->getSceneX()) / (float)resolution.first;
	float startY = (float)(resolution.second - object->getSceneY()) / (float)resolution.second;
	// Order of coordinates: X, Y
	// 0   2
	// | / |
	// 1   3
	float vertexData[] = { startX,                startY,
						   startX,                startY - objectHeightN,
						   startX + objectWidthN, startY,
						   startX + objectWidthN, startY - objectHeightN };

	glBindVertexArray(object->_buffer.getVAOObject());
	//bind buffer and handle vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
	//index, size, type, normalized, stride, offset in GL_ARRAY_BUFFER target
	glVertexAttribPointer(object->_aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(object->_aPositionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void textureUpdate(std::shared_ptr<TextureComponent> object) {
	if (object->_textureID != -1 || object->_texture != nullptr) {
		std::shared_ptr<Texture> targetTexture;
		targetTexture = object->_texture;
		object->_textureObject = targetTexture->getAtlas()->getTexureObjectID();
		float posXInAtlasN = (float)targetTexture->getX() / (float)targetTexture->getAtlas()->getWidth();
		float posYInAtlasN = (float)targetTexture->getY() / (float)targetTexture->getAtlas()->getHeight();
		float widthTile = (float)targetTexture->getWidth() / (float)targetTexture->getColumn() / (float)targetTexture->getAtlas()->getWidth();
		object->_widthTile = widthTile;
		float heightTile = (float)targetTexture->getHeight() / (float)targetTexture->getRow() / (float)targetTexture->getAtlas()->getHeight();
		object->_heightTile = heightTile;
		// Order of coordinates: S, T
		// 0   2
		// | / |
		// 1   3
		float textureData[] = { posXInAtlasN,                 posYInAtlasN,
								posXInAtlasN,                 posYInAtlasN + heightTile,
								posXInAtlasN + widthTile, posYInAtlasN,
								posXInAtlasN + widthTile, posYInAtlasN + heightTile };
		glUniform1i(object->_uSolidLocation, object->_solid);
		glUniform1i(object->_uTextureUnitLocation, 0);
		if (object->_tilesOrder.size() > 0 && object->_tilesLatency.size() > 0) {
			float width = object->_widthTile * ((int) (object->_tilesOrder[(object->_currentAnimateTile)]) % object->_texture->getColumn());
			glUniform1f(object->_uAdjustXLocation, width);
			float height = object->_heightTile * (int) (object->_tilesOrder[(object->_currentAnimateTile)] / object->_texture->getColumn());
			glUniform1f(object->_uAdjustYLocation, height);
		}
		else {
			glUniform1f(object->_uAdjustXLocation, 0);
			glUniform1f(object->_uAdjustYLocation, 0);
		}

		glBindTexture(GL_TEXTURE_2D, object->_textureObject);
		glBindBuffer(GL_ARRAY_BUFFER, object->_buffer.getVBOObject());
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textureData), textureData);
		glVertexAttribPointer(object->_aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(object->_aTextureCoordinatesLocation);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (object->_solid != 0) {
		glUniform1i(object->_uSolidLocation, object->_solid);
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (object->_tilesOrder.size() > 0 && object->_tilesLatency.size() > 0) {
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

void textUpdate(std::shared_ptr<ObjectComponent> object, std::shared_ptr<TextComponent> textComponent) {
	float startX = object->getSceneX();
	float startY = object->getSceneY();
	float endX = startX + object->getWidth();
	float endY = startY + object->getHeight();
	float x = startX;
	float y = startY;
	glUseProgram(object->_program);
	glUniform3f(glGetUniformLocation(object->_program, "textColor"), textComponent->_color[0], textComponent->_color[1], textComponent->_color[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textComponent->_VAO);

	//the tallest char
	int allignHeight = 0;
	for (std::string::const_iterator c = textComponent->_text.begin(); c != textComponent->_text.end(); c++)
	{
		Character ch = textComponent->_loader->_characters[*c];
		if (allignHeight < ch.size.second)
			allignHeight = ch.size.second;
	}

	bool allignY = false;
	int yAllign = 0;
	// Iterate through all characters
	for (std::string::const_iterator c = textComponent->_text.begin(); c != textComponent->_text.end(); c++)
	{
		Character ch = textComponent->_loader->_characters[*c];
		GLfloat w = ch.size.first * textComponent->_scale;
		GLfloat h = ch.size.second * textComponent->_scale;
		GLfloat ypos = 0;
		GLfloat xpos = 0;
		xpos = x + ch.bearing.first *  textComponent->_scale;
		if (xpos + w >= endX) {
			yAllign += allignHeight;
			x = startX;
			xpos = x + ch.bearing.first *  textComponent->_scale;
		}

		xpos /= (float)resolution.first;
		ypos = y + (allignHeight + yAllign + ch.size.second - ch.bearing.second) * textComponent->_scale;
		if (ypos >= endY)
			break;
		ypos = (float)resolution.second - ypos;
		ypos /= (float)resolution.second;

		
		w /= (float)resolution.first;
		h /= (float)resolution.second;
		/*
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		*/
		// Order of coordinates: X, Y
		// 1   3
		// | \ |
		// 0   2
		float vertices[4][4] = {
								 { xpos,     ypos,     0.0, 1.0 },
								 { xpos,     ypos + h, 0.0, 0.0 },
								 { xpos + w, ypos,     1.0, 1.0 },
								 { xpos + w, ypos + h, 1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textComponent->_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * textComponent->_scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void transformUpdate(std::shared_ptr<ObjectComponent> object, std::shared_ptr<MoveComponent> move) {
	float adjustX = std::get<0>(move->_coords);
	float adjustY = std::get<1>(move->_coords);
	//Matrix2D matrix;
	//matrix.translate(adjustX, adjustY);
	//object->_transform = object->_transform * matrix;
	//glUniformMatrix4fv(move->_uMatrixLocation, 1, false, object->_transform.getData());
	object->changeSceneX(adjustX);
	object->changeSceneY(adjustY);
	//TODO: ISSUE!! ORDER DEPENDENCIES BETWEEN CAMERA AND TRANSFORM. CAMERA USES _coords VAR TOO
	move->_coords = { 0, 0 };
}

void cameraUpdate(std::shared_ptr<ObjectComponent> object, std::shared_ptr<CameraComponent> camera, std::tuple<float, float> coords) {
	float x = -std::get<0>(coords) * object->_cameraCoefSpeed;
	float y = -std::get<1>(coords) * object->_cameraCoefSpeed;
	//Matrix2D matrix;
	//matrix.translate(x, y);
	//object->_camera = object->_camera * matrix;
	//glUniformMatrix4fv(camera->_uViewMatrixLocation, 1, false, object->_camera.getData());
	object->changeSceneX(x);
	object->changeSceneY(y);
}

std::tuple<float, float> cameraFindCoords(shared_ptr<EntityManager> entityManager, std::shared_ptr<CameraComponent>& cameraComponent, /*std::shared_ptr<ObjectComponent>& cameraObjectComponent,*/ std::shared_ptr<Entity>& camera) {
	int targetEntityID = -1;
	std::tuple<float, float> coords = { 0, 0 };
	for (auto entity : entityManager->getEntities()) {
		cameraComponent = entity->getComponent<CameraComponent>();
		if (cameraComponent) {
			coords = cameraComponent->_coords;
			//cameraObjectComponent = entity->getComponent<ObjectComponent>();
			camera = entity;
			break;
		}
	}

	return coords;
}

void DrawSystem::update(shared_ptr<EntityManager> entityManager) {
	std::shared_ptr<CameraComponent> cameraComponent;
	//std::shared_ptr<ObjectComponent> cameraObjectComponent;
	std::shared_ptr<Entity> camera;
	auto coords = cameraFindCoords(entityManager, cameraComponent, /*cameraObjectComponent,*/ camera);

	for (auto entity : entityManager->getEntities()) {
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		
		if (groupComponent->_groupName == "Engine" && gameMode == true)
			continue;

		auto vertexObject = entity->getComponent<ObjectComponent>();
		if (vertexObject)
			vertexUpdate(vertexObject);

		if (cameraComponent && entity == camera) {
			cameraComponent->_cameraX += std::get<0>(coords);
			cameraComponent->_cameraY += std::get<1>(coords);
		}
		

		if (cameraComponent && camera != entity) {
			cameraUpdate(vertexObject, cameraComponent, coords);
		}

		auto textTextureObject = entity->getComponent<TextComponent>();
		if (textTextureObject)
			textUpdate(vertexObject, textTextureObject);

		auto transformTextureObject = entity->getComponent<MoveComponent>();
		if (transformTextureObject)
			transformUpdate(vertexObject, transformTextureObject);

		auto textureObject = entity->getComponent<TextureComponent>();
		if (textureObject)
			textureUpdate(textureObject);
	}
}
