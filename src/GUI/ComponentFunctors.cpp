#include "ComponentFunctors.h"

std::map<std::string, std::shared_ptr<ComponentFunctor> > componentFunctors;

void registerComponentFunctors() {
 componentFunctors["ClickMoveComponent"] = make_shared<ClickMoveComponentFunctor>();
 componentFunctors["TextureComponent"] = make_shared<TextureComponentFunctor>();
}