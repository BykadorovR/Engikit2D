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

class TextureListEvent : public ITextEvent {
public:
	//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
	void configureFunctor(std::shared_ptr<Entity> targetEntity);

};

class TextureLoadEvent : public ITextEvent {
public:
	//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
	void configureFunctor(std::shared_ptr<Entity> targetEntity);
};

class TextureAcceptLoadEvent : public ITextEvent {
public:
	void configureFunctor(std::shared_ptr<Entity> targetEntity);
	std::string _fullPath;
	int _atlasID;
	int _objectX;
	int _objectY;
	int _type;
	int _tileX;
	int _tileY;
};

class TextureChangeEvent : public ITextEvent {
public:
	void configureFunctor(std::shared_ptr<Entity> targetEntity);
	std::vector<float> _latency;
	std::vector<float> _order;
	int _id;
};

class SaveEvent : public ITextEvent {
public:
	void configureFunctor(std::shared_ptr<Entity> targetEntity);
};

class LoadEvent : public ITextEvent {
public:
	void configureFunctor(std::shared_ptr<Entity> targetEntity);
};

class AddComponentEvent : public ITextEvent {
public:
	std::string _name;
	//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
	void configureFunctor(std::shared_ptr<Entity> targetEntity);

};
