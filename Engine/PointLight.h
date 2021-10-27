#pragma once
#include "Light.h"

class PointLight final : public Light
{
public:
	PointLight();
	virtual ~PointLight();

	PointLight(PointLight& other) = delete;
	PointLight(PointLight&& other) = delete;
	PointLight operator=(PointLight& other) = delete;
	PointLight& operator=(PointLight&& other) = delete;
};
