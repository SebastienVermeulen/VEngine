#pragma once

class Texture 
{
public:
	Texture(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView);
	~Texture();

	ID3D11Resource** GetResourceLocation() const;
	ID3D11Resource* GetResource() const;
	ID3D11ShaderResourceView** GetResourceViewLocation() const;
	ID3D11ShaderResourceView* GetResourceView() const;

private:
	ID3D11Resource* m_pResource;
	ID3D11ShaderResourceView* m_pResourceView;

	std::wstring m_LocalFileDir;
};
