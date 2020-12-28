#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Common.h"
#include "GraphicSystems.h"
#include "TextureManager.h"
#include <algorithm>
#include "State.h"
#include <locale>
#include <codecvt>
#include <chrono>
#include "CustomComponents.h"
DrawSystem::DrawSystem() {
}

void DrawSystem::vertexUpdate(std::shared_ptr<ObjectComponent> vertexObject) {
	glUseProgram(vertexObject->getShader()->getProgram());
	glUniformMatrix4fv(glGetUniformLocation(vertexObject->getShader()->getProgram(), "u_Model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(vertexObject->getShader()->getProgram(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
	auto ortoProjection = glm::ortho(0.0f, (float) std::get<0>(currentResolution), 0.0f, (float) std::get<1>(currentResolution), 1.0f, 100.0f);
	ortoProjection = glm::frustum(0.0f, (float)std::get<0>(currentResolution), 0.0f, (float)std::get<1>(currentResolution), 1.0f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(vertexObject->getShader()->getProgram(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(ortoProjection));
}

void DrawSystem::textureUpdate(std::shared_ptr<TextureComponent> textureObject) {
	glUniform1f(/*u_AdjustX*/       2, 0);
	glUniform1f(/*u_AdjustY*/       3, 0);
	glUniform4fv(/*color_mask*/     4, 1, reinterpret_cast<GLfloat *>(&textureObject->getColorMask()[0]));
	glUniform4fv(/*color_addition*/ 5, 1, reinterpret_cast<GLfloat *>(&textureObject->getColorAddition()[0]));
	if (textureObject->hasBindedTexture())
		glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTextureAtlas(textureObject->getTextureID())->getTextureObject());
	textureObject->getBufferManager()->activateBuffer();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	textureObject->getBufferManager()->deactivateBuffer();
}

bool renderChar(wchar_t c, std::tuple<float, float> charPosition, std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	CharacterInfo chInfo = GlyphsLoader::instance().getCharacters()[c];
	GLfloat w = std::get<0>(chInfo._size) * textObject->getScale();
	GLfloat h = std::get<1>(chInfo._size) * textObject->getScale();
	//First of all we should change vertex buffer by changing size and position
	vertexObject->getBufferManager()->changeBuffer(BufferType::Position, charPosition, { w, h }, currentResolution);

	std::tuple<float, float> characterAtlasPosition = GlyphsLoader::instance().getCharactersAtlasPosition()[c];
	//Now we should change texture buffer by passing position of current glyph in atlas to OpenGL API
	textObject->getBufferManager()->changeBuffer(BufferType::Texture, characterAtlasPosition,
		{ std::get<0>(chInfo._size), std::get<1>(chInfo._size) },
		GlyphsLoader::instance().getAtlas()->getSize());

	glUniform1f(/*u_AdjustX*/       2, 0);
	glUniform1f(/*u_AdjustY*/       3, 0);

	glUniform4fv(/*color_mask*/     4, 1, reinterpret_cast<GLfloat *>(&textObject->getColor()[0]));
	std::vector<float> colorAddition = { 0.0f, 0.0f, 0.0f, 0.0f };
	glUniform4fv(/*color_addition*/ 5, 1, reinterpret_cast<GLfloat *>(&colorAddition[0]));

	glBindTexture(GL_TEXTURE_2D, GlyphsLoader::instance().getAtlas()->getTextureObject());
	textObject->getBufferManager()->activateBuffer();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	textObject->getBufferManager()->deactivateBuffer();
	return false;
}

void DrawSystem::textUpdate(std::shared_ptr<Entity> entity) {
	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::shared_ptr<ObjectComponent> vertexObject = entity->getComponent<ObjectComponent>();
	std::shared_ptr<TextComponent> textObject = entity->getComponent<TextComponent>();
	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };

	float objectWidth = std::get<0>(positionEnd) - std::get<0>(positionStart);
	float objectHeight = std::get<1>(positionEnd) - std::get<1>(positionStart);
	float startX = std::get<0>(positionStart);
	float startY = std::get<1>(positionStart);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring text = converter.from_bytes(textObject->getText());
	int xAllign = 0;
	int yAllign = 0;
	float xPos = startX;
	float yPos = startY;
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = GlyphsLoader::instance().getCharacters()[*c];
		xPos = startX + xAllign;
		float symbolWidth = (chInfo._advance >> 6) - std::get<0>(chInfo._bearing);
		//sizeY - bearingY = size of symbol under line (bottom part)
		float symbolHeightAllign = (GlyphsLoader::instance().getGlyphHeight() - std::get<1>(chInfo._bearing));
		if (xPos + symbolWidth >= std::get<0>(positionEnd))
			continue;
		//allign by the tallest char (bearing is the upper part of symbol)
		yPos = startY + symbolHeightAllign + yAllign;
		
		renderChar(*c, { xPos, yPos }, vertexObject, textObject);
		//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		xAllign += symbolWidth; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

// Called every game update
void DrawSystem::update() {
	for (auto entity : _entityManager->getEntities()) {
		if (std::get<1>(entity) == EntityState::ENTITY_UNREGISTERED)
			continue;

		auto vertexObject = std::get<0>(entity)->getComponent<ObjectComponent>();
		if (vertexObject)
			vertexUpdate(vertexObject);

		auto textureObject = std::get<0>(entity)->getComponent<TextureComponent>();
		if (textureObject)
			textureUpdate(textureObject);

		auto textObject = std::get<0>(entity)->getComponent<TextComponent>();
		if (textObject && vertexObject)
			textUpdate(std::get<0>(entity));
	}
}