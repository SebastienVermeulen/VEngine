#pragma once

class Material final 
{
public:
	Material();
	~Material();

	Material(Material& other) = delete;
	Material(Material&& other) = delete;
	Material operator=(Material& other) = delete;
	Material& operator=(Material&& other) = delete;

	void InitShader(ID3D11Device& pDevice, ID3D11DeviceContext& context);

private:
	ID3D10Blob* m_pVSBlob;
	ID3D10Blob* m_pPSBlob;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;

	ID3D11InputLayout* m_pLayout;
};
