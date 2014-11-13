#include "MeshManager.h"

using namespace std;

MeshManager::MeshManager(TextureManager* textureManager)
{
	m_textureManager = textureManager;
}

MeshManager::~MeshManager()
{ 
	Release();
}

bool MeshManager::Load(LPDIRECT3DDEVICE9 device, const char* filename)
{
	Mesh* tempMesh = NULL;

	if(filename == 0)
		return false;

	if(GetMesh(filename) != NULL)
		return true;

	tempMesh = new Mesh();
	if(tempMesh->Load(device, filename, m_textureManager	))
	{
		string filenameString = filename;
		m_meshMap[filenameString] = tempMesh;
		tempMesh = NULL;
		return true;
	}
	else
	{
		delete tempMesh;
		tempMesh = NULL;
		return false;
	}
}

Mesh* MeshManager::GetMesh(const char* filename)
{
	string filenameString = filename;
	MeshMap::const_iterator searchResult = m_meshMap.find(filenameString);

	if(searchResult != m_meshMap.end())
	{
		searchResult->second->AddRef();
		return searchResult->second;
	}

	return NULL;
}

void MeshManager::ReleaseMesh(Mesh* mesh)
{
	if(mesh)
	{
		mesh->RemoveRef();
		if(mesh->GetRefCount() <= 0)
		{
			string filename = mesh->GetFileName();
			MeshMap::const_iterator result = m_meshMap.find(filename);
			if( result != m_meshMap.end())
			{
				m_meshMap.erase(result);
			}
			delete mesh;
			mesh = NULL;
				
		}
	}
}

void MeshManager::Release()
{
	if(!m_meshMap.empty())
	{
		MeshMap::iterator mapIterator;
		for(mapIterator = m_meshMap.begin(); mapIterator != m_meshMap.end(); mapIterator++)
		{
			mapIterator->second->Release(m_textureManager);
			mapIterator->second = NULL;
		}
		m_meshMap.clear();
	}
}
