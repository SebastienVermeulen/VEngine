#pragma once
class EngineDevice;
class Renderable;

class Renderer final
{
public:
	Renderer(EngineDevice* device);
	~Renderer();

	void Render();

	void AddRenderable(Renderable* pRenderable);
	void RemoveRenderable(Renderable* pRenderable);

private:
	static EngineDevice* m_pDevice;

	std::vector<Renderable*> m_Renderables;
};
