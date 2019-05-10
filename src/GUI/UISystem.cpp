#include "UISystem.h"

int MouseSystem::processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent,
	std::shared_ptr<TransformComponent> transformComponent) {
	int currentClickX = std::get<0>(clickClickMoveComponent->_currentClick);
	int currentClickY = std::get<1>(clickClickMoveComponent->_currentClick);
	int clickedInside = 0;

	//first click
	if (clickClickMoveComponent->_clickFlag && currentClickX > objectComponent->_sceneX  && currentClickY > objectComponent->_sceneY &&
		currentClickX < objectComponent->_sceneX + objectComponent->_objectWidth && currentClickY < objectComponent->_sceneY + objectComponent->_objectHeight) {

		clickClickMoveComponent->_previousClick = clickClickMoveComponent->_currentClick;
		clickClickMoveComponent->_currentClick = { 0, 0 };
		clickClickMoveComponent->_clickFlag = false;
		clickedInside = 1;
		return clickedInside;
	}

	int previousClickX = std::get<0>(clickClickMoveComponent->_previousClick);
	int previousClickY = std::get<1>(clickClickMoveComponent->_previousClick);

	if (clickClickMoveComponent->_clickFlag && previousClickX && previousClickY) {
		//We should point ADJUSTMENT not coords
		/*transformComponent->setTransform({ currentClickX - (objectComponent->_sceneX + objectComponent->_objectWidth / 2),
										  currentClickY - (objectComponent->_sceneY + objectComponent->_objectHeight / 2) });
		*/
		clickClickMoveComponent->_previousClick = { 0, 0 };
		if (clickClickMoveComponent->_moveToByClickSecond == false)
			clickedInside = 2;
	}

	clickClickMoveComponent->_currentClick = { 0, 0 };
	clickClickMoveComponent->_clickFlag = false;

	return clickedInside;
}

void MouseSystem::processClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickMoveComponent> clickMoveComponent,
	std::shared_ptr<TransformComponent> transformComponent) {
	float speed = clickMoveComponent->_speed;
	int clickX = std::get<0>(clickMoveComponent->_leftClick);
	int clickY = std::get<1>(clickMoveComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (clickMoveComponent->_move == false && clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight)
		return;

	clickMoveComponent->_move = true;
	int objectX = objectComponent->_sceneX + objectComponent->_objectWidth / 2;
	int objectY = objectComponent->_sceneY + objectComponent->_objectHeight / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		transformComponent->setTransform({ stepX, stepY });
	}
	else {
		clickMoveComponent->_move = false;
	}
}

int MouseSystem::processClickInside(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
	std::shared_ptr<GroupEntitiesComponent> groupComponent) {
	int clickX = std::get<0>(clickInsideComponent->_leftClick);
	int clickY = std::get<1>(clickInsideComponent->_leftClick);
	int clickedInside = 0;

	if (!clickX || !clickY)
		return clickedInside;

	if (clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight &&
		clickInsideComponent->_leftClickFlag) {
		//TODO: add group component
		std::cout << "Clicked inside: group " << groupComponent->_groupNumber << " " << groupComponent->_groupName << std::endl;
		clickInsideComponent->_leftClickFlag = false;
		clickedInside = 1;
	}

	//We should handle click inside action only once per click, so reset coords of click after first handle
	clickInsideComponent->_leftClick = { 0, 0 };
	clickInsideComponent->_rightClick = { 0, 0 };

	return clickedInside;
}

//TODO: DISABLE ONLY MOVEMENT SYSTEM IF CLICKED
//THINK ABOUT SEVERAL CLICKTOMOVE ENTITIES
//DO MORE APROPRIATE LOGIC FOR HANDLING mainHeroDisableMoving (NOW ONLY 1 is disallowed but it's not correct so GG moves to second click)
void MouseSystem::update() {
	std::shared_ptr<Entity> mainHero = nullptr;
	int mainHeroDisableMoving = 0;

	//first of all we should find GG
	for (auto entity : getEntities()) {
		auto groupComponent = entity->getComponent<ClickMoveComponent>();
		//Can use existence of ClickMoveComponent also
		if (groupComponent)
			mainHero = entity;
	}

	for (auto entity : getEntities()) {
		if (mainHero && entity == mainHero)
			continue;

		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto transformComponent = entity->getComponent<TransformComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		auto clickClickMoveComponent = entity->getComponent<ClickClickMoveComponent>();
		auto clickMoveComponent = entity->getComponent<ClickMoveComponent>();
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();

		if (objectComponent && clickInsideComponent && groupComponent) {
			
			int clickedInside = processClickInside(objectComponent, clickInsideComponent, groupComponent);
			if (clickInsideComponent->_moveToByClick == false)
				mainHeroDisableMoving = clickedInside;
			if (interactionComponent && clickedInside)
				interactionComponent->_interactReady = true;
		}

		if (objectComponent && clickClickMoveComponent && transformComponent) {
			int clickedInside = processClickClickMove(objectComponent, clickClickMoveComponent, transformComponent);
			if (clickClickMoveComponent->_moveToByClickFirst == false && clickedInside == 1)
				mainHeroDisableMoving = clickedInside;
			if (clickClickMoveComponent->_moveToByClickSecond == false && clickedInside == 2)
				mainHeroDisableMoving = clickedInside;

			if (interactionComponent && clickedInside == 2)
				interactionComponent->_interactReady = true;
			if (interactionComponent && clickedInside == 1)
				interactionComponent->_interactReady = false;
		}
	}

	//after all entities processed we can understand either it's allowed to move main hero or not
	if (mainHero && mainHeroDisableMoving != 1) {
		auto objectComponent = mainHero->getComponent<ObjectComponent>();
		auto transformComponent = mainHero->getComponent<TransformComponent>();
		auto clickMoveComponent = mainHero->getComponent<ClickMoveComponent>();
		auto groupComponent = mainHero->getComponent<GroupEntitiesComponent>();
		auto clickInsideComponent = mainHero->getComponent<ClickInsideComponent>();
		auto interactionComponent = mainHero->getComponent<InteractionAddToEntityComponent>();

		if (objectComponent && clickMoveComponent && transformComponent)
			processClickMove(objectComponent, clickMoveComponent, transformComponent);

		if (objectComponent && clickInsideComponent && groupComponent) {
			int clickedInside = processClickInside(objectComponent, clickInsideComponent, groupComponent);
			if (interactionComponent && clickedInside)
				interactionComponent->_interactReady = true;
		}

	}
	else if (mainHero) {
		auto clickMoveComponent = mainHero->getComponent<ClickMoveComponent>();
		if (clickMoveComponent) {
			clickMoveComponent->_leftClick = { 0, 0 };
			clickMoveComponent->_rightClick = { 0, 0 };
		}
	}
}

void InteractionAddToEntitySystem::process() {

}

void InteractionAddToEntitySystem::update() {
	shared_ptr<Entity> subjectEntity = nullptr;
	shared_ptr<Entity> objectEntity = nullptr;
	for (auto entity : getEntities()) {
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();
		if (!interactionComponent)
			continue;
		if (interactionComponent->_interactReady) {
			if (interactionComponent->_interactionMember == InteractionMember::OBJECT)
				objectEntity = entity;
			else if (interactionComponent->_interactionMember == InteractionMember::SUBJECT)
				subjectEntity = entity;
		}
	}
	if (objectEntity && subjectEntity) {
		auto interactionComponentSubject = subjectEntity->getComponent<InteractionAddToEntityComponent>();
		auto interactionComponentObject = objectEntity->getComponent<InteractionAddToEntityComponent>();
		objectEntity->addComponent(interactionComponentSubject->_componentToAdd);
	}

	if (objectEntity) {
		auto interactionComponentObject = objectEntity->getComponent<InteractionAddToEntityComponent>();
		interactionComponentObject->_interactReady = false;
	}

}