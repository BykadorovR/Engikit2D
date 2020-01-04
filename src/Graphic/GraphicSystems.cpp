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
		GLfloat w = std::get<0>(chInfo.size) * textObject->getScale();
		GLfloat h = std::get<1>(chInfo.size) * textObject->getScale();

		GLfloat xPos = startX + xAllign;
		//allign by the tallest char (bearing is the upper part of symbol)
		GLfloat yPos = startY + (allignBearingY - std::get<1>(chInfo.bearing)) * textObject->getScale();
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
		xAllign += ((chInfo.advance >> 6) + std::get<0>(chInfo.bearing)) * textObject->getScale(); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

struct Word {
	Word() {
		_size = 0;
	}

	std::vector<std::wstring> _text;
	int _size;
};

struct Line {
	Line() {
		_size = 0;
	
	}
	std::vector<Word> _text;
	int _size;
};

//TODO: add possibility to choose allignment (center, left, right)
//TODO: Need to split text to words and lines only once! at init
void DrawSystem::textUpdate(std::shared_ptr<ObjectComponent> vertexObject, std::shared_ptr<TextComponent> textObject) {
	const float lineSpacingCoeff = 1;

	std::tuple<float, float> positionStart = vertexObject->getPosition();
	std::tuple<float, float> positionEnd = { std::get<0>(vertexObject->getPosition()) + std::get<0>(vertexObject->getSize()),
											 std::get<1>(vertexObject->getPosition()) + std::get<1>(vertexObject->getSize()) };


	float objectWidth = std::get<0>(positionEnd) - std::get<1>(positionStart);
	float startX = std::get<0>(positionStart);
	float startY = std::get<1>(positionStart);
	
	wchar_t delimiter = *(L" ");
	CharacterInfo delimiterInfo = textObject->getLoader()->getCharacters()[delimiter];
	int delimiterSize = ((delimiterInfo.advance >> 6) + std::get<0>(delimiterInfo.bearing)) * textObject->getScale();

	std::vector<std::tuple<std::wstring, int> > words;
	words.push_back({L"", 0});
	int wordIndex = 0;

	//the tallest char
	float allignBearingY = 0;
	std::wstring text = textObject->getText();
	//let's find the char with the biggest upper part (not size but height) and the biggest overall size
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		allignBearingY = std::max(static_cast<float>(std::get<1>(chInfo.bearing)), allignBearingY);
		
		//Split the text to words to avoid situation when 1 word is splited to two lines
		if (*c == delimiter) {
			//end of word
			words.push_back({ L"", 0 });
			wordIndex++;
		}
		else {
			std::get<0>(words[wordIndex]) += *c;
			//we have to calculate size of word (it's offset of the char to next char + bearing) but for last char we don't use offset but just it's width + bearing
			if (std::next(c) != text.end())
				std::get<1>(words[wordIndex]) += ((chInfo.advance >> 6) + std::get<0>(chInfo.bearing)) * textObject->getScale();
			else
				std::get<1>(words[wordIndex]) += std::get<0>(chInfo.size) * textObject->getScale();
		}
	}

	float allignHeight = 0;
	for (auto c = text.begin(); c != text.end(); c++) {
		CharacterInfo chInfo = textObject->getLoader()->getCharacters()[*c];
		allignHeight = std::max(allignBearingY - static_cast<float>(std::get<1>(chInfo.bearing)) +
												static_cast<float>(std::get<1>(chInfo.size)), allignHeight);
	}

	std::vector<int> widthLine;
	widthLine.push_back(0);
	int heightText = 0;

	//calculate line width + line heights. Also split words to lines (insert delimiters to correct words)
	float xPos = startX;
	float yPos = startY;
	int lineIndex = 0;
	for (auto word = words.begin(); word != words.end(); word++) {
		if (xPos + std::get<1>(*word) >= std::get<0>(positionEnd)) {
			yPos += (allignHeight)* lineSpacingCoeff * textObject->getScale();
			xPos = startX;
		}

		if (std::next(word) != words.end() &&
			xPos + std::get<1>(*word) + std::get<1>(*std::next(word)) + delimiterSize < std::get<0>(positionEnd)) {
			std::get<0>(*word) += delimiter;
			std::get<1>(*word) += delimiterSize;
			widthLine[lineIndex] += std::get<1>(*word);
		}
		else {
			widthLine[lineIndex] += std::get<1>(*word);
			widthLine.push_back(0);
			lineIndex++;
		}

		xPos += std::get<1>(*word);
	}

	xPos = startX;
	yPos = startY;
	lineIndex = 0;
	//need to find overall width for row
	int widthAllign = 0;
	for (auto word = words.begin(); word != words.end(); word++) {
		std::wstring currentText = std::get<0>(*word);
		int currentWordSize = std::get<1>(*word);
			
		widthAllign = (objectWidth - widthLine[lineIndex]) / 2;
		if (widthAllign < 0)
			widthAllign = 0;

		if (yPos + allignHeight >= std::get<1>(positionEnd)) {
			//TODO: Need to change _page variable
			break;
		}

		if (lineIndex >= textObject->getPageNumber())
			renderChar(currentText, { xPos + widthAllign, yPos }, allignBearingY, vertexObject, textObject);

		xPos += currentWordSize;

		//criteria of line break
		if (currentText.back() != delimiter) {
			xPos = startX;
			if (lineIndex >= textObject->getPageNumber())
				yPos += (allignHeight)* lineSpacingCoeff * textObject->getScale();
			lineIndex++;
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