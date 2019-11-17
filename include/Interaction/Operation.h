#pragma once
#include <string>

class Operation {
public:
	Operation();
	bool compare();
	bool prepare(std::string config);
};