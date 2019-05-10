#include "UISystem.h"

ClickCount MouseSystem::processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent,
	std::shared_ptr<TransformComponent> transformComponent) {
	int currentClickX = std::get<0>(clickClickMoveComponent->_currentClick);
	int currentClickY = std::get<1>(clickClickMoveComponent->_currentClick);
	ClickCount clickedInside = ClickCount::NO;

	//first click
	if (clickClickMoveComponent->_clickFlag && currentClickX > objectComponent->_sceneX  && currentClickY > objectComponent->_sceneY &&
		currentClickX < objectComponent->_sceneX + objectComponent->_objectWidth && currentClickY < objectComponent->_sceneY + objectComponent->_objectHeight) {

		clickClickMoveComponent->_previousClick = clickClickMoveComponent->_currentClick;
		clickClickMoveComponent->_currentClick = { 0, 0 };
		clickClickMoveComponent->_clickFlag = false;
		clickedInside = ClickCount::FIRST;
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
			clickedInside = ClickCount::SECOND;
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

ClickCount MouseSystem::processClickInside(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
	std::shared_ptr<GroupEntitiesComponent> groupComponent) {
	int clickX = std::get<0>(clickInsideComponent->_leftClick);
	int clickY = std::get<1>(clickInsideComponent->_leftClick);
	ClickCount clickedInside = ClickCount::NO;

	if (!clickX || !clickY)
		return clickedInside;

	if (clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight &&
		clickInsideComponent->_leftClickFlag) {
		//TODO: add group component
		std::cout << "Clicked inside: group " << groupComponent->_groupNumber << " " << groupComponent->_groupName << std::endl;
		clickInsideComponent->_leftClickFlag = false;
		clickedInside = ClickCount::FIRST;
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
	std::vector<std::shared_ptr<Entity> > playerControlledEntities;
	int playerControlledEntitiesDisableMoving = 0;

	//first of all we should find GG
	for (auto entity : getEntities()) {
		auto clickMoveComponent = entity->getComponent<ClickMoveComponent>();
		//Can use existence of ClickMoveComponent also
		if (clickMoveComponent)
			playerControlledEntities.push_back(entity);
	}

	//handle all components except ClickMoveComponent
	for (auto entity : getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();


		if (objectComponent && clickInsideComponent && groupComponent) {			
			int clickedInside = processClickInside(objectComponent, clickInsideComponent, groupComponent);
			if (clickInsideComponent->_moveToByClick == false)
				playerControlledEntitiesDisableMoving += clickedInside;
			if (interactionComponent && clickedInside)
				interactionComponent->_interactReady = true;
		}

		auto transformComponent = entity->getComponent<TransformComponent>();
		auto clickClickMoveComponent = entity->getComponent<ClickClickMoveComponent>();

		if (objectComponent && clickClickMoveComponent && transformComponent) {
			int clickedInside = processClickClickMove(objectComponent, clickClickMoveComponent, transformComponent);

			if (clickClickMoveComponent->_moveToByClickFirst == false && clickedInside == ClickCount::FIRST)
				playerControlledEntitiesDisableMoving += clickedInside;
			if (clickClickMoveComponent->_moveToByClickSecond == false && clickedInside == ClickCount::SECOND)
				playerControlledEntitiesDisableMoving += clickedInside;

			if (interactionComponent && clickedInside == ClickCount::SECOND)
				interactionComponent->_interactReady = true;
			if (interactionComponent && clickedInside == ClickCount::FIRST)
				interactionComponent->_interactReady = false;
		}
	}

	for (auto entity : playerControlledEntities) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto clickMoveComponent = entity->getComponent<ClickMoveComponent>();
		auto transformComponent = entity->getComponent<TransformComponent>();

		if (objectComponent && clickMoveComponent && transformComponent && !playerControlledEntitiesDisableMoving)
			processClickMove(objectComponent, clickMoveComponent, transformComponent);
		else if (playerControlledEntitiesDisableMoving) {
			if (clickMoveComponent) {
				clickMoveComponent->_leftClick = { 0, 0 };
				clickMoveComponent->_rightClick = { 0, 0 };
			}
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
		std::shared_ptr<Component> addedComponent = interactionComponentSubject->_createFunctor();
		objectEntity->addComponent(addedComponent);
	}

	//We should handle only if BOTH entities subject and object are ready for interaction
	if (objectEntity) {
		auto interactionComponentObject = objectEntity->getComponent<InteractionAddToEntityComponent>();
		if (interactionComponentObject)
			interactionComponentObject->_interactReady = false;
	}

	if (subjectEntity) {
		auto interactionComponentSubject = subjectEntity->getComponent<InteractionAddToEntityComponent>();
		if (interactionComponentSubject)
			interactionComponentSubject->_interactReady = false;
	}

}