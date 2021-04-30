#pragma once

class Shader final 
{
public:
	Shader();
	~Shader();

	Shader(Shader& other) = delete;
	Shader(Shader&& other) = delete;
	Shader operator=(Shader& other) = delete;
	Shader& operator=(Shader&& other) = delete;

	void Init(ID3D11Device& pDevice, ID3D11DeviceContext& context);

private:
	ID3D10Blob* m_pVSBlob;
	ID3D10Blob* m_pPSBlob;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;
};
