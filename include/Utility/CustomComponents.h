#pragma once
#include "Component.h"

class CustomComponent : public OperationComponent {
public:
	CustomComponent();
private:
	//should be extended to map or vector of vectors
	std::vector<std::string> _strings;
};

