#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <d3dx9.h>
#include <map>
#include <string>

#include "Mesh.h"

class MeshManager
{
private:
	TextureManager* m_textureManager;
	typedef std::map<std::string, Mesh*> MeshMap;
	MeshMap m_meshMap;

public:
	MeshManager(TextureManager* textureManager);
	~MeshManager();
	bool Load(LPDIRECT3DDEVICE9 device, const char* filename);
	Mesh* GetMesh(const char* filename);
	void ReleaseMesh(Mesh* mesh);
	void Release();

};


#endif // !MESHMANAGER_H
