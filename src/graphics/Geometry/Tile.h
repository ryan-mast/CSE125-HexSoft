#pragma once
#include "../SceneGraph/Geode.h"
#include "../TextureLoader.h"
#include "../Util.h"
class Tile :
	public Geode
{
public:
	float length;
	 //member variables
    IDirect3DVertexBuffer9* mVB;
	IDirect3DIndexBuffer9*  mIB;
    IDirect3DTexture9*     mTexture;
    //shaders
    
    ID3DXEffect* mFX;
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhWVP;
	D3DXHANDLE   mhWorldInvTrans;
	D3DXHANDLE   mhEyePos;
	D3DXHANDLE   mhWorld;
	D3DXHANDLE   mhTex;
	D3DXHANDLE   mhLight;
	D3DXHANDLE	 mhMtrl;
	Mtrl mtrl;


	//transform to place it correctly in world
	D3DXMATRIX defaultTransform;

	Tile(void);
	Tile(float i);
	~Tile(void);
	void init();
    void render(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj);
    void buildFX();
    void draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj);
};

