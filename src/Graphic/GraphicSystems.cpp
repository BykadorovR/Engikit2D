#include "Common.h"
#include "GraphicSystems.h"
#include "TextureManager.h"
#include <algorithm>
#include "State.h"
#include <locale>
#include <codecvt>

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

void renderChar(std::wstring word, std::tuple<float, float> wordPosition, float allignBearingY, std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	int xAllign = 0;
	float startX = std::get<0>(wordPosition);
	float startY = std::get<1>(wordPosition);
	for (auto c = word.begin(); c != word.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		GLfloat w = std::get<0>(chInfo._size) * textObject->getScale();
		GLfloat h = std::get<1>(chInfo._size) * textObject->getScale();

		xAllign += std::get<0>(chInfo._bearing);

		GLfloat xPos = startX + xAllign;
		//allign by the tallest char (bearing is the upper part of symbol)
		GLfloat yPos = startY + (allignBearingY - std::get<1>(chInfo._bearing)) * textObject->getScale();
		//First of all we should change vertex buffer by changing size and position
		vertexObject->getBufferManager()->changeBuffer(BufferType::Position, { xPos, yPos }, { w, h }, resolution);

		std::tuple<float, float> characterAtlasPosition = textObject->getLoader()->getCharactersAtlasPosition()[*c];
		//Now we should change texture buffer by passing position of current glyph in atlas to OpenGL API
		textObject->getBufferManager()->changeBuffer(BufferType::Texture, characterAtlasPosition,
			{ std::get<0>(chInfo._size), std::get<1>(chInfo._size) },
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
		xAllign += ((chInfo._advance >> 6) - std::get<0>(chInfo._bearing)) * textObject->getScale(); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

//TODO: Need to split text to words and lines only once! at init
void DrawSystem::textUpdate(std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	std::tuple<TextAllignment, TextAllignment> textAllignment = textObject->getAllignment();
	
	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };

	float objectWidth = std::get<0>(positionEnd) - std::get<0>(positionStart);
	float objectHeight = std::get<1>(positionEnd) - std::get<1>(positionStart);
	float startX = std::get<0>(positionStart);
	float startY = std::get<1>(positionStart);
	
	int scaledLineSpacing = textObject->getLoader()->getLineSpace();

	wchar_t delimiter = *(L" ");
	CharacterInfo delimiterInfo = textObject->getLoader()->getCharacters()[delimiter];
	int delimiterSize = ((delimiterInfo._advance >> 6) + std::get<0>(delimiterInfo._bearing)) * textObject->getScale();

	std::vector<Line> lines;
	lines.push_back(Line());

	int wordSize = 0;
	int allignBearingYMax = 0;
	
	Word currentWord;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring text = converter.from_bytes(textObject->getText());
	//let's find the char with the biggest upper part (not size but height) and the biggest overall size
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		int symbolWidth = chInfo._advance >> 6;

		if (*c == delimiter) {
			//skip delimiter, add current word to next line
			if (lines.back().getWidth() > 0 && (lines.back().getWidth() + currentWord.getWidth()) * textObject->getScale() > objectWidth) {
				lines.back().getText().back().cropTrailingSpace();
				lines.push_back(Line());
				currentWord += {*c, chInfo};
				lines.back().addWord(currentWord);
			} else {
				if ((lines.back().getWidth() + currentWord.getWidth() + symbolWidth) * textObject->getScale() <= objectWidth) {
					currentWord += {*c, chInfo};
				}
				lines.back().addWord(currentWord);
			}

			allignBearingYMax = max(lines.back().getBearingYMax(), allignBearingYMax);

			currentWord.clear();
			continue;
		} else if ((currentWord.getWidth() + symbolWidth) * textObject->getScale() > objectWidth) {
			if (lines.back().getWidth() > 0) {
				lines.back().getText().back().cropTrailingSpace();
				lines.push_back(Line());
				lines.back().addWord(currentWord);
				currentWord.clear();
				lines.push_back(Line());
				currentWord += {*c, chInfo};
			}
			else {
				lines.back().addWord(currentWord);
				allignBearingYMax = max(lines.back().getBearingYMax(), allignBearingYMax);
				lines.push_back(Line());
				currentWord.clear();
				currentWord += {*c, chInfo};
			}
			continue;
		}

		currentWord += {*c, chInfo};
	} 

	if (currentWord.getWidth() > 0) {
		if (lines.back().getWidth() > 0 && (lines.back().getWidth() + currentWord.getWidth()) * textObject->getScale() > objectWidth) {
			lines.back().getText().back().cropTrailingSpace();
			lines.push_back(Line());
			lines.back().addWord(currentWord);
		}
		else {
			lines.back().addWord(currentWord);
		}

		allignBearingYMax = max(lines.back().getBearingYMax(), allignBearingYMax);
	}

	textObject->setMember("totalPages", lines.size());

	if (lines.size() <= textObject->getPageNumber())
		return;

	//Calculate real height of text
	float linesInObject = objectHeight / ((scaledLineSpacing)* textObject->getLineSpacingCoeff() * textObject->getScale());
	float realLinesNumber = (lines.size() - textObject->getPageNumber()) >= floor(linesInObject) ? floor(linesInObject) : (lines.size() - textObject->getPageNumber());
	int realLinesHeight = 0;
	for (auto line = lines.begin() + textObject->getPageNumber(); line != lines.begin() + textObject->getPageNumber() + realLinesNumber; line++) {
		float currentLineSize = line->getHeightAdjusted(allignBearingYMax);
		realLinesHeight += currentLineSize;
		if (std::next(line) != lines.begin() + textObject->getPageNumber() + realLinesNumber)
			realLinesHeight += scaledLineSpacing * textObject->getLineSpacingCoeff() - currentLineSize;
	}

	//calculate allignment
	float heightAllign = 0;
	if (std::get<1>(textAllignment) == TextAllignment::CENTER)
		heightAllign = (objectHeight - (realLinesHeight) * textObject->getScale()) / 2;
	else if (std::get<1>(textAllignment) == TextAllignment::RIGHT) {
		heightAllign = objectHeight - (realLinesHeight) * textObject->getScale();
	}

	if (heightAllign < 0)
		heightAllign = 0;

	float xPos = startX;
	float yPos = startY;
	//need to find overall width for row
	for (auto line = lines.begin() + textObject->getPageNumber(); line != lines.end(); line++) {
		float widthAllign = 0;
		if (std::get<0>(textAllignment) == TextAllignment::CENTER)
			widthAllign = (objectWidth - line->getWidth() * textObject->getScale()) / 2;
		else if (std::get<0>(textAllignment) == TextAllignment::RIGHT)
			widthAllign = objectWidth - line->getWidth() * textObject->getScale();

		if (widthAllign < 0)
			widthAllign = 0;

		std::vector<Word> words = line->getText();
		for (auto word = words.begin(); word != words.end(); word++) {
			std::wstring currentText = word->getText();
			int currentWordSize = word->getWidth() * textObject->getScale();

			renderChar(currentText, { xPos + widthAllign, yPos + heightAllign }, allignBearingYMax, vertexObject, textObject);
			xPos += currentWordSize;
		}

		xPos = startX;
		yPos += (scaledLineSpacing) * textObject->getLineSpacingCoeff() * textObject->getScale();
		if (yPos + (scaledLineSpacing) * textObject->getLineSpacingCoeff() * textObject->getScale() >= std::get<1>(positionEnd)) {
			break;
		}

	}
}

// Called every game update
void DrawSystem::update() {
	for (auto entity : _entityManager->getEntities()) {
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