#include "pch.h"
#include "Texture.h"
#include "FileManager.h"

Texture::Texture(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView)
	:m_pTexture{ pTexture }
	, m_pTextureView{ pTextureView }
	, m_LocalFileDir{}
{
}
Texture::~Texture() 
{
	SafeRelease(m_pTextureView);
	SafeRelease(m_pTexture);
}

ID3D11Resource* Texture::GetTexture() const
{
	return m_pTexture;
}
ID3D11ShaderResourceView* Texture::GetTextureView() const
{
	return m_pTextureView;
}
