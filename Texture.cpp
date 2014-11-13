#include "Texture.h"

Texture::Texture()
{
	m_referenceCount = 0;
	m_texture = NULL;
	m_filename = NULL;
}

Texture::~Texture()
{
	Release(); //Just in case it's not called by the manager
}

bool Texture::Load(LPDIRECT3DDEVICE9 device, const char* filename)
{
	if(FAILED(D3DXCreateTextureFromFile(device, filename, &m_texture)))
	{
		return false;
	}

	m_filename = filename;
	return true;
}

void Texture::Release()
{
	if(m_texture != NULL)
	{
		m_texture->Release();
		m_texture = NULL;
	}
}