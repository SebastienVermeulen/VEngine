#pragma once
#include "Renderer.h"

class EngineDevice;

class ForwardsDX11 final : public Renderer
{
public:
	ForwardsDX11(EngineDevice* device);
	~ForwardsDX11();

	ForwardsDX11(ForwardsDX11& other) = delete;
	ForwardsDX11(ForwardsDX11&& other) = delete;
	ForwardsDX11 operator=(ForwardsDX11& other) = delete;
	ForwardsDX11& operator=(ForwardsDX11&& other) = delete;

	virtual void Render() override;
	virtual void ClearBuffers() override;

private:
	void SetupTargetsForwardsPass();
};