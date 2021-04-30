#pragma once
class EngineDevice;

class Renderer final
{
public:
	Renderer(EngineDevice* device);
	~Renderer();

	void Render();

private:
	static EngineDevice* m_pDevice;
};
