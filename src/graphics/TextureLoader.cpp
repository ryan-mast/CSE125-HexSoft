#include "TextureLoader.h"
#include <d3dx9.h>

#include "Util.h"
#include <iostream>
using namespace std;
TextureLoader::TextureLoader(void)
{
}


TextureLoader::~TextureLoader(void)
{
}

void TextureLoader::createTex(int ID, char* pSrc)
{
    IDirect3DTexture9* ppTexture;
    HRESULT hr=D3DXCreateTextureFromFile(gd3dDevice, pSrc , &ppTexture);
	if(!SUCCEEDED(hr))
	{
		cout<<"ERROR"<<endl;
	}
    map_textures[ID]= ppTexture;

}