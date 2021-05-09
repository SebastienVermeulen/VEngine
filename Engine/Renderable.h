#pragma once
class Renderable abstract
{
public:
	inline virtual void Render(ID3D11Device& device, ID3D11DeviceContext& context) const {}

	inline bool ShouldRender() { return m_ShouldRender; }

protected:
	inline Renderable() 
		:m_ShouldRender{false} {}
	inline virtual ~Renderable(){}

	Renderable(Renderable& other) = delete;
	Renderable(Renderable&& other) = delete;
	Renderable operator=(Renderable& other) = delete;
	Renderable& operator=(Renderable&& other) = delete;

	bool m_ShouldRender;
};