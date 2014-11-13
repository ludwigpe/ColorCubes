#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <d3dx9.h>

class Texture
{
private:
	int m_referenceCount;
	LPDIRECT3DTEXTURE9 m_texture;
	const char* m_filename;

	void Release();

protected:
	Texture();
	~Texture();
	void AddRef() { m_referenceCount++; }
	void RemoveRef() { m_referenceCount--; }
	int GetRefCount() { return m_referenceCount; }

public:
	bool Load(LPDIRECT3DDEVICE9 device, const char* filename);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_texture; }
	const char* GetFileName() { return m_filename; }

	friend class TextureManager;
};

#endif