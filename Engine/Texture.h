#pragma once
class Texture 
{
public:
	Texture(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView);
	~Texture();

	ID3D11Resource* GetTexture() const;
	ID3D11ShaderResourceView* GetTextureView() const;

private:
	ID3D11Resource* m_pTexture;
	ID3D11ShaderResourceView* m_pTextureView;

	std::wstring m_LocalFileDir;
};