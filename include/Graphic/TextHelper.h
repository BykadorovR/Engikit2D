#pragma once
#include "Entity.h"

class ITextEvent {
public:
	virtual void configureFunctor(std::shared_ptr<Entity> targetEntity) = 0;
};

class ComponentTextEvent : public ITextEvent {
public:
	//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
	void configureFunctor(std::shared_ptr<Entity> targetEntity);

};

class TextureLoadEvent : public ITextEvent {
public:
	//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
	void configureFunctor(std::shared_ptr<Entity> targetEntity);

};