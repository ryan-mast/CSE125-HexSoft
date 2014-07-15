#pragma once

#include <d3dx9.h>
#include <map>
#include <string>
using namespace std;

class TextureLoader
{
public:
	
    TextureLoader(void);
    
    ~TextureLoader(void);
    
    void createTex(int id, char* pSrc);

    //member variables
    map<int, IDirect3DTexture9* > map_textures;
};

