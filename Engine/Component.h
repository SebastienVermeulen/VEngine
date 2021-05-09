#pragma once
#include "BaseUpdateStructure.h"
#include "Renderable.h"

class Component abstract : public BaseUpdateStructure, public Renderable
{
public:
	Component();
	virtual ~Component();

	Component(Component& other) = delete;
	Component(Component&& other) = delete;
	Component operator=(Component& other) = delete;
	Component& operator=(Component&& other) = delete;
};