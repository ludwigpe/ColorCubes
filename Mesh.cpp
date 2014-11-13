#include "Mesh.h"

Mesh::Mesh()
{
	m_refCount = 0;
	m_materialBuffer = NULL;
	m_materials = NULL;
	m_mesh = NULL;
	m_numberOfMaterials = NULL;
	m_materials = NULL;
	m_filename = NULL;

}


bool Mesh::Load(LPDIRECT3DDEVICE9 device, const char* filename, TextureManager* textureManager)
{
	if(FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, device, NULL, &m_materialBuffer, NULL, &m_numberOfMaterials, &m_mesh)))
	{
		return false;
	}

	m_filename = filename;
	m_materials = new D3DMATERIAL9[m_numberOfMaterials];
	m_meshTextures = new Texture*[m_numberOfMaterials];

	D3DXMATERIAL* loadedMaterials = (D3DXMATERIAL*)m_materialBuffer->GetBufferPointer();

	for(DWORD i = 0; i < m_numberOfMaterials; i++) 
	{
		m_materials[i] = loadedMaterials[i].MatD3D;
		m_materials[i].Ambient = m_materials[i].Diffuse;

		char* textureFilename = loadedMaterials[i].pTextureFilename;
		if(textureManager->Load(device, textureFilename))
		{
			m_meshTextures[i] = textureManager->GetTexture(textureFilename);
		}
		else 
		{
			m_meshTextures[i] = NULL;
		}
	}

	return true;


}

void Mesh::Release( TextureManager* textureManager)
{
	for(DWORD i = 0; i < m_numberOfMaterials; i++)
	{
		if(m_meshTextures[i] != NULL)
		{
			Texture* texture = m_meshTextures[i];
			textureManager->ReleaseTexture(texture);
			m_meshTextures[i] = NULL;
		}
	}

	if(m_materials != NULL)
	{
		delete[] m_materials;
		m_materials = NULL;
		m_numberOfMaterials = 0;
	}

	if(m_meshTextures != NULL)
	{
		delete[] m_meshTextures;
		m_meshTextures = NULL;
	}

	if(m_materialBuffer != NULL)
	{
		m_materialBuffer->Release();
		m_materialBuffer = NULL;
	}
	
	if(m_mesh != NULL)
	{
		m_mesh->Release();
		m_mesh = NULL;
	}
}

void Mesh::Render(LPDIRECT3DDEVICE9 device)
{
	for(DWORD i = 0; i < m_numberOfMaterials; i++)
	{
		device->SetMaterial(&m_materials[i]);
		device->SetTexture(0, m_meshTextures[i]->GetTexture());
		m_mesh->DrawSubset(i);
	}
}

void Mesh::Render(LPDIRECT3DDEVICE9 device, Texture* texture)
{
	for(DWORD i = 0; i < m_numberOfMaterials; i++)
	{
		
		device->SetMaterial(&m_materials[i]);
		device->SetTexture(0, texture->GetTexture());
		m_mesh->DrawSubset(i);
	}
}