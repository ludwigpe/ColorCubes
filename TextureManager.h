#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <d3dx9.h>
#include <map>
#include <string>

#include "Texture.h"


class TextureManager
{
private:
	typedef std::map<std::string, Texture*> TextureMap;
	TextureMap m_textureMap;
	
public:
	TextureManager();
	~TextureManager();
	bool Load(LPDIRECT3DDEVICE9 device, const char* filename);
	Texture* GetTexture(const char* filename);
	void ReleaseTexture(Texture* texture);
	void Release();
};

#endif