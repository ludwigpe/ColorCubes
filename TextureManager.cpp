#include "TextureManager.h"

using namespace std;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
	Release();
}

bool TextureManager::Load(LPDIRECT3DDEVICE9 device, const char* filename)
{
	Texture* tempTexture = NULL;		//Create a texture pointer to store the loaded texture

	if(filename == 0)					//If the filename is empty then return
	{
		return false;
	}

	if(GetTexture(filename) != NULL)	//If the texture is already loaded then return
	{
		return true;
	}

	tempTexture = new Texture();		//Create a new Texture Object

	if(tempTexture->Load(device, filename))		//Attempt to load the file
	{
		string filenameString = filename;
		m_textureMap[filenameString] = tempTexture;	//If loaded then add texture to the map
		tempTexture = NULL;
		return true;
	}
	else
	{
		delete tempTexture;						//If the texture didn't load delete the Texture object
		tempTexture = NULL;
		return false;
	}
}

Texture* TextureManager::GetTexture(const char* filename)
{
	string filenameString = filename;		//Convert the char* to a std::string

	//Use the find method to get an iterator pointing to the texture stored with the filename
	TextureMap::const_iterator searchResult = m_textureMap.find(filenameString);

	if(searchResult != m_textureMap.end())	//If the texture is found
	{
		searchResult->second->AddRef();		//Increase the reference counter
		return searchResult->second;		//And return it
	}

	return NULL;							//Otherwise return NULL
}

void TextureManager::ReleaseTexture(Texture* texture)
{
	if(texture)
	{
		texture->RemoveRef();
		if(texture->GetRefCount() <= 0)
		{
			string filename = texture->GetFileName();
			TextureMap::const_iterator searchResult = m_textureMap.find(filename);
			if(searchResult != m_textureMap.end())
			{
				m_textureMap.erase(searchResult);
			}
			delete texture;
			texture = NULL;
		}
	}
}

void TextureManager::Release()
{
	if(!m_textureMap.empty())
	{
		TextureMap::iterator mapIterator;
		for(mapIterator = m_textureMap.begin(); mapIterator != m_textureMap.end(); mapIterator++)
		{
			mapIterator->second->Release();
			mapIterator->second = NULL;
		}

		m_textureMap.clear();
	}
}