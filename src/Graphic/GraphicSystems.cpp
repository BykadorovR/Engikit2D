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

struct Symbol {
	Symbol() {

	}

	Symbol(std::tuple<wchar_t, CharacterInfo> params) {
		_text = std::get<0>(params);
		_chInfo = std::get<1>(params);
	}

	CharacterInfo _chInfo;
	wchar_t _text;
};

struct Word {
	Word() {
	}

	std::wstring getText() {
		std::wstring text;
		for (auto symbol : _text) {
			text += symbol._text;
		}
		return text;
	}

	int getWidth() {
		int size = 0;
		for (auto symbol : _text) {
			size += symbol._chInfo._advance >> 6;
		}
		return size;
	}

	int getHeight() {
		int size = 0;
		for (auto symbol : _text) {
			size = std::max(std::get<1>(symbol._chInfo._size), size);
		}
		return size;
	}

	int getHeightAdjusted(int bearingYMax) {
		int size = 0;
		for (auto symbol : _text) {
			size = std::max(std::get<1>(symbol._chInfo._size) + bearingYMax - std::get<1>(symbol._chInfo._bearing), size);
		}
		return size;
	}

	bool clear() {
		_text.clear();
		return false;
	}

	bool cropTrailingSpace() {
		if (_text.back()._text == wordsDelimiter)
			_text.pop_back();
		return false;
	}

	int getBearingYMin() {
		int size = INT_MAX;
		for (auto symbol : _text) {
			size = std::min(std::get<1>(symbol._chInfo._bearing), size);
		}
		return size;
	}

	int getBearingYMax() {
		int size = INT_MIN;
		for (auto symbol : _text) {
			size = std::max(std::get<1>(symbol._chInfo._bearing), size);
		}
		return size;
	}

	std::wstring operator+=(std::tuple<wchar_t, CharacterInfo> rhs) {
		_text.push_back(rhs);
		return getText();
	}

	std::vector<Symbol> _text;
};

struct Line {
	Line() {

	}

	bool addWord(Word word) {
		_text.push_back(word);
		return false;
	}

	int getWidth() {
		int width = 0;
		for (auto word : _text) {
			width += word.getWidth();
		}
		return width;
	}

	int getHeight() {
		int height = 0;
		for (auto word : _text) {
			height = std::max(word.getHeight(), height);
		}
		return height;
	}

	int getHeightAdjusted(int bearingYMax) {
		int height = 0;
		for (auto word : _text) {
			height = std::max(word.getHeightAdjusted(bearingYMax), height);
		}
		return height;
	}

	int getBearingYMin() {
		int size = INT_MAX;
		for (auto symbol : _text) {
			size = std::min(symbol.getBearingYMin(), size);
		}
		return size;
	}

	int getBearingYMax() {
		int size = INT_MIN;
		for (auto symbol : _text) {
			size = std::max(symbol.getBearingYMax(), size);
		}
		return size;
	}

	std::vector<Word>& getText() {
		return _text;
	}

	std::vector<Word> _text;
};

//TODO: add possibility to choose allignment (center, left, right)
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
	std::wstring text = textObject->getText();
	Word currentWord;
	//let's find the char with the biggest upper part (not size but height) and the biggest overall size
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		int symbolWidth = chInfo._advance >> 6;

		if (*c == delimiter || std::next(c) == text.end()) {
			//skip delimiter, add current word to next line
			if ((lines.back().getWidth() + currentWord.getWidth()) * textObject->getScale() > objectWidth) {
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

			allignBearingYMax = std::max(lines.back().getBearingYMax(), allignBearingYMax);

			currentWord.clear();
			continue;
		}

		currentWord += {*c, chInfo};
	}

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