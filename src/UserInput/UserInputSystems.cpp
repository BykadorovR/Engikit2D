#include "GraphicComponents.h"
#include "UserInputSystems.h"
#include "Operation.h"
#include "InteractionComponents.h"
#include "UserInputComponents.h"
#include <locale>
#include <codecvt>

MouseSystem::MouseSystem() {
	MouseEvent::instance().registerComponent(this);
}

bool MouseSystem::checkOperation() {
	for (auto entity : _entityManager->getEntities()) {
		auto interactionComponent = entity->getComponent<InteractionComponent>();
		auto mouseComponent = entity->getComponent<MouseComponent>();
		if (interactionComponent) {
			std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > operations = interactionComponent->getOperations();
			for (auto operation : operations) {
				if (std::get<1>(operation) == InteractionType::MOUSE && mouseComponent && std::get<0>(operation)->checkOperation()) {
					for (auto action : std::get<0>(operation)->getActions()) {
						action->doAction();
					}
				}
			}
		}
	}
	//TODO: Add enumeration for return values
	return false;
}

void MouseSystem::mouseClickDownLeft(int x, int y) {
	//first update coords in mouse components
	for (auto entity : _entityManager->getEntities()) {
		auto mouseComponent = entity->getComponent<MouseComponent>();
		if (mouseComponent) {
			mouseComponent->setMember("leftClickX", x);
			mouseComponent->setMember("leftClickY", y);
		}
	}

	//check mouse related operations
	checkOperation();
}

void MouseSystem::mouseClickDownRight(int x, int y) {
}

MouseSystem::~MouseSystem() {
	MouseEvent::instance().unregisterComponent(this);
}

KeyboardSystem::KeyboardSystem() {
	KeyboardEvent::instance().registerComponent(this);
}

bool KeyboardSystem::checkOperation() {
	for (auto entity : _entityManager->getEntities()) {
		auto interactionComponent = entity->getComponent<InteractionComponent>();
		auto keyboardComponent = entity->getComponent<KeyboardComponent>();
		if (interactionComponent) {
			std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > operations = interactionComponent->getOperations();
			for (auto operation : operations) {
				if (std::get<1>(operation) == InteractionType::KEYBOARD && keyboardComponent && std::get<0>(operation)->checkOperation()) {
					for (auto action : std::get<0>(operation)->getActions()) {
						action->doAction();
					}
				}
			}
		}
	}
	//TODO: Add enumeration for return values
	return false;
}

void KeyboardSystem::keyboardPressed(int key, int action, int mode) {
	//Here should be handled HARDWARE keys, so "W" key is "W" on ALL keyboard layouts
	//first update coords in keyboard components
	if (action == GLFW_PRESS) {
		for (auto entity : _entityManager->getEntities()) {
			auto keyboardComponent = entity->getComponent<KeyboardComponent>();
			if (keyboardComponent) {
				switch (key) {
				case GLFW_KEY_BACKSPACE:
					auto textComponent = entity->getComponent<TextComponent>();
					if (textComponent && textComponent->getFocus()) {
						auto stringWithoutLastChar = textComponent->getText();
						//First convert to wstring
						std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converterToW;
						std::wstring intermediate = converterToW.from_bytes(stringWithoutLastChar);
						//remove last symbol
						intermediate.pop_back();
						//remove wstring back to string
						std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converterFromW;
						std::string result = converterFromW.to_bytes(intermediate);
						textComponent->setMember("text", result);
					}
					break;
				}
			}
		}
	}
}

void KeyboardSystem::textInput(unsigned int character) {
	//Here we handle UTF8 characters pressed. IMPORTANT: it's keyboard layout dependent function! So "W" on US and French keyboards are on different places
	//We don't need to convert character from multibyte to UTF8 because it's already UTF8
	auto k = (wchar_t)character;
	std::wstring wideText;
	wideText.push_back(k);

	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converter;
	std::string text = converter.to_bytes(wideText);
	//first update coords in keyboard components
	for (auto entity : _entityManager->getEntities()) {
		auto keyboardComponent = entity->getComponent<KeyboardComponent>();
		if (keyboardComponent) {
			keyboardComponent->setMember("symbol", text);
		}
	}

	//check keyboard related operations
	checkOperation();
}

KeyboardSystem::~KeyboardSystem() {
	KeyboardEvent::instance().unregisterComponent(this);
}