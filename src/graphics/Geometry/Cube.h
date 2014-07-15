#pragma once
#include "../SceneGraph/Geode.h"
#include "../TextureLoader.h"
#include "../Util.h"
class Cube :
    public Geode
{
public:

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

	D3DXMATRIX defaultTransform;

    //methods
    Cube();
    
    ~Cube();

    void init();
    void render(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj);
    void buildFX();
    void draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj);
};

