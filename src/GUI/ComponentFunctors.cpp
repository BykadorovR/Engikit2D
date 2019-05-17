#include "ComponentFunctors.h"

std::map<std::string, std::shared_ptr<ComponentFunctor> > componentFunctors;

void registerComponentFunctors(std::shared_ptr<TextureManager> textureManager) {
 componentFunctors["ClickMoveComponent"] = make_shared<ClickMoveComponentFunctor>();
 componentFunctors["TextureComponent"] = make_shared<TextureComponentFunctor>(textureManager);
}