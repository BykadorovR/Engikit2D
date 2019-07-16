#include "ComponentFunctors.h"

std::map<std::string, std::shared_ptr<ComponentFunctor> > componentFunctors;

void registerComponentFunctors() {
	componentFunctors["ObjectComponent"] = make_shared<ObjectComponentFunctor>();
	componentFunctors["TextureComponent"] = make_shared<TextureComponentFunctor>();
	componentFunctors["ClickInsideComponent"] = make_shared<ClickInsideFunctor>();
	componentFunctors["GroupEntitiesComponent"] = make_shared<GroupEntitiesFunctor>();
	componentFunctors["InteractionAddToEntityComponent"] = make_shared<InteractionAddToEntityFunctor>();
	componentFunctors["MoveComponent"] = make_shared<MoveFunctor>();
	componentFunctors["CameraComponent"] = make_shared<CameraFunctor>();
}