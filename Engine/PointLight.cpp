#include "pch.h"
#include "PointLight.h"
#include "LightWidget.h"

PointLight::PointLight()
	:Light()
{
	m_CanRender = true;

	m_LightType = LightType::point;

	m_pWidget = new LightWidget(this);
}
PointLight::~PointLight()
{
}
