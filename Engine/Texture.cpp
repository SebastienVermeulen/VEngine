#include "pch.h"
#include "Texture.h"
#include "FileManager.h"

Texture::Texture(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView)
	:m_pResource{ pTexture }
	, m_pResourceView{ pTextureView }
	, m_LocalFileDir{}
{
}
Texture::~Texture() 
{
	SafeRelease(m_pResourceView);
	SafeRelease(m_pResource);
}

ID3D11Resource** Texture::GetResourceLocation() const
{
	return (ID3D11Resource**)&m_pResource;
}
ID3D11Resource* Texture::GetResource() const
{
	return m_pResource;
}
ID3D11ShaderResourceView** Texture::GetResourceViewLocation() const
{
	return (ID3D11ShaderResourceView**)&m_pResourceView;
}
ID3D11ShaderResourceView* Texture::GetResourceView() const
{
	return m_pResourceView;
}
