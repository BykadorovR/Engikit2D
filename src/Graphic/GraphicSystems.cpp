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
	glUniformMatrix4fv(glGetUniformLocation(vertexObject->getShader()->getProgram(), "u_View"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	auto ortoProjection = glm::ortho(0.0f, (float) std::get<0>(currentResolution), 0.0f, (float) std::get<1>(currentResolution), 0.0f, 100.0f);
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
	std::tuple<TextAllignment, TextAllignment> textAllignment = textObject->getAllignment();
	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };

	float objectWidth = std::get<0>(positionEnd) - std::get<0>(positionStart);
	float objectHeight = std::get<1>(positionEnd) - std::get<1>(positionStart);
	float startX = std::get<0>(positionStart);
	float startY = std::get<1>(positionStart);
	int scaledLineSpacing = GlyphsLoader::instance().getLineSpace();

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring text = converter.from_bytes(textObject->getText());
	wchar_t delimiter = *(L" ");
	CharacterInfo delimiterInfo = GlyphsLoader::instance().getCharacters()[delimiter];
	int xAllign = 0;
	int yAllign = 0;
	float xPos = startX;
	float yPos = startY;
	float* cursorPosition = std::get<0>(textObject->getMemberFloat("cursorPosition"));
	float currentCursor = 0;
	float currentHorizontal = 0;
	float startHorizontal = 0;
	//TODO: ideal solution for horizontal decorator + cursor is to have horizontal and vertical cursos poisition + array with every line width
	//for now let's calculate startHorizontal manually in another loop
	if (*std::get<0>(textObject->getMemberFloat("editable")) &&
		*std::get<0>(textObject->getMemberFloat("focus"))) {
		if (*cursorPosition == 0) {
			CharacterInfo chInfoCursor = GlyphsLoader::instance().getCharacters()['|'];
			renderChar('|', { startX, startY + (GlyphsLoader::instance().getGlyphHeight() - std::get<1>(chInfoCursor._bearing)) },
					   vertexObject, textObject);
		} else {
			//TODO: PERFORMANCE: basically we should find the nearest to cursor '\n' and start calculation from this position, so it allow us to avoid big part of text
			for (auto c = text.begin(); c < text.end(); c++) {
				CharacterInfo chInfo = GlyphsLoader::instance().getCharacters()[*c];
				if (*c == '\n') {
					currentHorizontal = 0;
					startHorizontal = 0;
					xAllign = 0;
				} else {
					currentHorizontal++;
					//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					xAllign += ((chInfo._advance >> 6) - std::get<0>(chInfo._bearing)); // Bitshift by 6 to get value in pixels (2^6 = 64)
				}
				currentCursor++;

				while (xAllign >= objectWidth) {
					CharacterInfo chRemove = GlyphsLoader::instance().getCharacters()[text[currentCursor - currentHorizontal + startHorizontal]];
					xAllign -= ((chRemove._advance >> 6) - std::get<0>(chRemove._bearing));
					startHorizontal += 1;
				}

				if (currentCursor == *cursorPosition)
					break;
			}
		}
	}

	xAllign = 0;
	currentHorizontal = 0;
	currentCursor = 0;
	float startVertical = *std::get<0>(entity->getComponent<CustomFloatComponent>()->getMemberFloat("textStartVertical"));
	float currentVertical = 0;
	for (auto c = text.begin(); c != text.end(); c++) {
		if (*c == '\n') {
			xAllign = 0;
			yAllign += GlyphsLoader::instance().getGlyphHeight();
			currentVertical++;
			currentHorizontal = 0;
			//once we reach the same page as in decorator we should start from 0;0
			if (startVertical > 0 && startVertical == currentVertical)
				yAllign = 0;

			//+1 because first line
			if ((currentVertical + 1 - startVertical) * GlyphsLoader::instance().getGlyphHeight() > objectHeight) {
				break;
			}
		}

		currentCursor++;

		if (*c != '\n' && currentHorizontal++ < startHorizontal) {
			continue;
		}

		if (currentVertical < startVertical)
			continue;

		CharacterInfo chInfo = GlyphsLoader::instance().getCharacters()[*c];
		xPos = startX + xAllign;
		float symbolWidth = (chInfo._advance >> 6) - std::get<0>(chInfo._bearing);
		//sizeY - bearingY = size of symbol under line (bottom part)
		float symbolHeightAllign = (GlyphsLoader::instance().getGlyphHeight() - std::get<1>(chInfo._bearing));
		if (xPos + symbolWidth >= std::get<0>(positionEnd))
			continue;
		//allign by the tallest char (bearing is the upper part of symbol)
		yPos = startY + symbolHeightAllign + yAllign;
		
		if (*c != '\n') {
			renderChar(*c, { xPos, yPos }, vertexObject, textObject);
			//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			xAllign += symbolWidth; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}

		if (*std::get<0>(textObject->getMemberFloat("editable")) &&
			*std::get<0>(textObject->getMemberFloat("focus"))) {
			if (currentCursor == *cursorPosition) {
				CharacterInfo chInfoCursor = GlyphsLoader::instance().getCharacters()['|'];
				symbolHeightAllign = (GlyphsLoader::instance().getGlyphHeight() - std::get<1>(chInfoCursor._bearing));
				renderChar('|', { xPos + symbolWidth, startY + symbolHeightAllign + yAllign },
					vertexObject, textObject);
			}
		}
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