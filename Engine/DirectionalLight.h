#pragma once
#include "Light.h"

class DirectionalLight final : public Light
{
public:
	DirectionalLight();
	virtual ~DirectionalLight();

	DirectionalLight(DirectionalLight& other) = delete;
	DirectionalLight(DirectionalLight&& other) = delete;
	DirectionalLight operator=(DirectionalLight& other) = delete;
	DirectionalLight& operator=(DirectionalLight&& other) = delete;
};
