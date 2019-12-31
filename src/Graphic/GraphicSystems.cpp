#include "GraphicSystems.h"
#include "TextureManager.h"
#include <algorithm>
#include "State.h"

DrawSystem::DrawSystem() {
}

void DrawSystem::vertexUpdate(std::shared_ptr<ObjectComponent> vertexObject) {
	glUseProgram(vertexObject->getShader()->getProgram());
}

void DrawSystem::textureUpdate(std::shared_ptr<TextureComponent> textureObject) {
	glUniform1f(/*u_AdjustX*/       2, 0);
	glUniform1f(/*u_AdjustY*/       3, 0);
	glUniform4fv(/*color_mask*/     4, 1, reinterpret_cast<GLfloat *>(&textureObject->getColorMask()[0]));
	glUniform4fv(/*color_addition*/ 5, 1, reinterpret_cast<GLfloat *>(&textureObject->getColorAddition()[0]));
	if (textureObject->hasBindedTexture())
		glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTextureAtlas(textureObject->getTextureID())->getTextureObject());
	textureObject->getBufferManager()->activateBuffer();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	textureObject->getBufferManager()->deactivateBuffer();
}

//TODO: add possibility to choose allignment (center, left, right)
void DrawSystem::textUpdate(std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };

	float startX = std::get<0>(positionStart);
	float startY = std::get<1>(positionStart);

	const float lineSpacingCoeff = 1;
	//the tallest char
	float allignBearing = 0;
	std::wstring text = textObject->getText();
	//let's find the char with the biggest upper part (not size but height) and the biggest overall size
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		allignBearing = std::max(static_cast<float>(std::get<1>(chInfo.bearing)), allignBearing);
	}

	float allignHeight = 0;
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		allignHeight = std::max(allignBearing - static_cast<float>(std::get<1>(chInfo.bearing)) + 
												static_cast<float>(std::get<1>(chInfo.size)), allignHeight);
	}

	int yAllign = 0;
	int xAllign = 0;
	// Iterate through all characters
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		GLfloat w = std::get<0>(chInfo.size) * textObject->getScale();
		GLfloat h = std::get<1>(chInfo.size) * textObject->getScale();
		GLfloat yPos = 0;
		GLfloat xPos = 0;
		xPos = startX + std::get<0>(chInfo.bearing) * textObject->getScale() + xAllign;
		if (xPos + w >= std::get<0>(positionEnd)) {
			yAllign += (allignHeight) * lineSpacingCoeff * textObject->getScale();
			xAllign = 0;
			xPos = startX + std::get<0>(chInfo.bearing) * textObject->getScale();
		}
		//allign by the tallest char (bearing is the upper part of symbol)
		yPos = startY + (allignBearing - std::get<1>(chInfo.bearing)) * textObject->getScale() + yAllign;
		if (yPos + h >= std::get<1>(positionEnd))
			break;
		//First of all we should change vertex buffer by changing size and position
		vertexObject->getBufferManager()->changeBuffer(BufferType::Position, { xPos, yPos }, { w, h }, resolution);
		
		std::tuple<float, float> characterAtlasPosition = textObject->getLoader()->getCharactersAtlasPosition()[*c];
		//Now we should change texture buffer by passing position of current glyph in atlas to OpenGL API
		textObject->getBufferManager()->changeBuffer(BufferType::Texture, characterAtlasPosition, 
													{ std::get<0>(chInfo.size), std::get<1>(chInfo.size) },
													textObject->getLoader()->getAtlas()->getSize());

		glUniform1f(/*u_AdjustX*/       2, 0);
		glUniform1f(/*u_AdjustY*/       3, 0);
		
		glUniform4fv(/*color_mask*/     4, 1, reinterpret_cast<GLfloat *>(&textObject->getColor()[0]));
		std::vector<float> colorAddition = { 0.0f, 0.0f, 0.0f, 0.0f };
		glUniform4fv(/*color_addition*/ 5, 1, reinterpret_cast<GLfloat *>(&colorAddition[0]));

		glBindTexture(GL_TEXTURE_2D, textObject->getLoader()->getAtlas()->getTextureObject());
		textObject->getBufferManager()->activateBuffer();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		textObject->getBufferManager()->deactivateBuffer();
		//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		xAllign += (chInfo.advance >> 6) * textObject->getScale(); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
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