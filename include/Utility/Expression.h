#pragma once

template <class T>
class BaseExpression {
	std::vector<std::shared_ptr<T> > arguments;
	std::string expression;
public:
	bool addArgument();
	bool addExpression();
};