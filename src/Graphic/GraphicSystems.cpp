#include "GraphicSystems.h"
#include "TextureManager.h"
#include <algorithm>
#include "State.h"

DrawSystem::DrawSystem(std::shared_ptr<BufferManager> bufferManager) {
	_bufferManager = bufferManager;
}

void DrawSystem::vertexUpdate(std::shared_ptr<ObjectComponent> vertexObject) {
	glUseProgram(vertexObject->getShader()->getProgram());
	_bufferManager->activateBuffer();
}

void DrawSystem::textureUpdate(std::shared_ptr<TextureComponent> textureObject) {
	glUniform1f(/*u_AdjustX*/       2, 0);
	glUniform1f(/*u_AdjustY*/       3, 0);
	glUniform4fv(/*color_mask*/     4, 1, reinterpret_cast<GLfloat *>(&textureObject->getColorMask()[0]));
	glUniform4fv(/*color_addition*/ 5, 1, reinterpret_cast<GLfloat *>(&textureObject->getColorAddition()[0]));
	glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTextureAtlas(textureObject->getTextureID())->getTextureObject());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	_bufferManager->deactivateBuffer();
}

void DrawSystem::textUpdate(std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };

	float currentX = std::get<0>(positionStart);
	float currentY = std::get<1>(positionStart);

	//the tallest char
	float allignHeight = 0;
	for (std::string::const_iterator c = textObject->getText().begin(); c != textObject->getText().end(); c++)
	{
		FT_Face chFace = textObject->getLoader()->getCharacters()[*c];
		allignHeight = std::max(static_cast<float>(chFace->glyph->bitmap.rows), allignHeight);
	}

	bool allignY = false;
	int yAllign = 0;
	// Iterate through all characters
	for (std::string::const_iterator c = textObject->getText().begin(); c != textObject->getText().end(); c++)
	{
		FT_Face chFace = textObject->getLoader()->getCharacters()[*c];
		GLfloat w = chFace->glyph->bitmap.width * textObject->getScale();
		GLfloat h = chFace->glyph->bitmap.rows * textObject->getScale();
		GLfloat yPos = 0;
		GLfloat xPos = 0;
		xPos = currentX + chFace->glyph->bitmap_left *  textObject->getScale();
		if (xPos + w >= std::get<0>(positionEnd)) {
			yAllign += allignHeight;
			currentX = std::get<0>(positionStart);
			xPos = currentX + chFace->glyph->bitmap_left *  textObject->getScale();
		}

		yPos = currentY + (allignHeight + yAllign + chFace->glyph->bitmap.rows - chFace->glyph->bitmap_top) * textObject->getScale();
		if (yPos >= std::get<1>(positionEnd))
			break;
		
		//First of all we should change vertex buffer by changing size and position
		_bufferManager->changeBuffer(vertexObject->getBuffer(), { xPos, yPos }, { w, h }, resolution);
		
		std::tuple<float, float> characterAtlasPosition = textObject->getLoader()->getCharactersAtlasPosition()[*c];
		//Now we should change texture buffer by passing position of current glyph in atlas to OpenGL API
		_bufferManager->changeBuffer(textObject->getBuffer(), characterAtlasPosition, { chFace->glyph->bitmap.width, chFace->glyph->bitmap.rows },
									 textObject->getLoader()->getAtlas()->getSize());

		glUniform1f(/*u_AdjustX*/       2, 0);
		glUniform1f(/*u_AdjustY*/       3, 0);
		std::vector<float> colorMask = {1.0f, 1.0f, 1.0f, 1.0f};
		glUniform4fv(/*color_mask*/     4, 1, reinterpret_cast<GLfloat *>(&colorMask[0]));
		glUniform4fv(/*color_addition*/ 5, 1, reinterpret_cast<GLfloat *>(&textObject->getColor()[0]));

		glBindTexture(GL_TEXTURE_2D, textObject->getLoader()->getAtlas()->getAtlasID());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		xPos += (chFace->glyph->advance.x >> 6) * textObject->getScale(); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
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

		auto textObject = entity->getComponent<TextComponent>();
		if (textObject)
			textUpdate(vertexObject, textObject);
	}
}