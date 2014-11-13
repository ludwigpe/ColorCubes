#ifndef MESH_H
#define MESH_H

#include <d3dx9.h>
#include "TextureManager.h"

class Mesh
{
private:
	int				m_refCount;
	LPD3DXBUFFER	m_materialBuffer;
	LPD3DXMESH		m_mesh;
	DWORD			m_numberOfMaterials;
	D3DMATERIAL9*	m_materials;
	Texture**		m_meshTextures;
	const char*		m_filename;

	void Release(TextureManager* textureManager);

protected:
	Mesh();

	void AddRef() {m_refCount++;}
	void RemoveRef() { m_refCount--;	}
	int GetRefCount() { return m_refCount;}

public:
	bool Load(LPDIRECT3DDEVICE9	device, const char* filename, TextureManager* textureManager);
	const char* GetFileName() { return m_filename; }
	LPD3DXMESH GetD3DMesh() { return m_mesh; }
	D3DMATERIAL9* GetMeshMaterial(int index) { return &m_materials[index]; }
	Texture* GetMeshTexture(int index) { return m_meshTextures[index]; }

	void Render(LPDIRECT3DDEVICE9 d3dDevice);
	void Render(LPDIRECT3DDEVICE9 d3dDevice, Texture* texture);
	friend class MeshManager;
};

#endif // !MESH_H
