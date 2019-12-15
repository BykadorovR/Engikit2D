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
	glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTextureAtlas(textureObject->getTextureID())->getTextureObject());
	textureObject->getBufferManager()->activateBuffer();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	textureObject->getBufferManager()->deactivateBuffer();
}

void DrawSystem::textUpdate(std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };

	float currentX = std::get<0>(positionStart);
	float currentY = std::get<1>(positionStart);

	//the tallest char
	float allignHeight = 0;
	std::string text = textObject->getText();
	for (std::string::const_iterator c = text.begin(); c != text.end(); c++)
	{
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		allignHeight = std::max(static_cast<float>(std::get<1>(chInfo.size)), allignHeight);
	}

	int yAllign = 0;
	// Iterate through all characters
	for (std::string::const_iterator c = text.begin(); c != text.end(); c++)
	{
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		GLfloat w = std::get<0>(chInfo.size) * textObject->getScale();
		GLfloat h = std::get<1>(chInfo.size) * textObject->getScale();
		GLfloat yPos = 0;
		GLfloat xPos = 0;
		xPos = currentX + std::get<0>(chInfo.bearing) *  textObject->getScale();
		if (xPos + w >= std::get<0>(positionEnd)) {
			yAllign += allignHeight;
			currentX = std::get<0>(positionStart);
			xPos = currentX + std::get<0>(chInfo.bearing) *  textObject->getScale();
		}

		yPos = currentY - (allignHeight + yAllign + std::get<1>(chInfo.size) - std::get<1>(chInfo.bearing)) * textObject->getScale();
		if (yPos >= std::get<1>(positionEnd))
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
		currentX += (chInfo.advance >> 6) * textObject->getScale(); // Bitshift by 6 to get value in pixels (2^6 = 64)
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