#include "pch.h"
#include "DirectionalLight.h"
#include "LightWidget.h"

DirectionalLight::DirectionalLight()
	:Light()
{
	m_ShouldRender = true;

	m_LightType = LightType::directional;

	m_pWidget = new LightWidget(this);
}
DirectionalLight::~DirectionalLight()
{
}
