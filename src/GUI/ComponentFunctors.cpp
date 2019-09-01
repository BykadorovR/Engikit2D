#include "ComponentFunctors.h"

std::map<std::string, std::shared_ptr<ComponentFunctor> > componentFunctors;
std::map<std::string, bool> addFunctors;

void registerComponentFunctors() {
	componentFunctors["ClickInsideComponent"] = make_shared<ClickInsideFunctor>();
	addFunctors["ClickInsideComponent"] = false;
	componentFunctors["GroupEntitiesComponent"] = make_shared<GroupEntitiesFunctor>();
	addFunctors["GroupEntitiesComponent"] = false;
	componentFunctors["InteractionAddToEntityComponent"] = make_shared<InteractionAddToEntityFunctor>();
	addFunctors["InteractionAddToEntityComponent"] = false;
	componentFunctors["MoveComponent"] = make_shared<MoveFunctor>();
	addFunctors["MoveComponent"] = true;
	componentFunctors["CameraComponent"] = make_shared<CameraFunctor>();
	addFunctors["CameraComponent"] = true;
	componentFunctors["TextureComponent"] = make_shared<TextureComponentFunctor>();
	addFunctors["TextureComponent"] = true;
	componentFunctors["TextComponent"] = make_shared<TextComponentFunctor>();
	addFunctors["TextComponent"] = true;
	componentFunctors["ObjectComponent"] = make_shared<ObjectComponentFunctor>();
	addFunctors["ObjectComponent"] = false;
}