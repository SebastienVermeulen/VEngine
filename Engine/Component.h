#pragma once
#include "BaseUpdateStructure.h"

class Component abstract : public BaseUpdateStructure
{
public:
	Component();
	~Component();

	Component(Component& other) = delete;
	Component(Component&& other) = delete;
	Component operator=(Component& other) = delete;
	Component& operator=(Component&& other) = delete;
};