#include "Tile.h"
#include "../Vertex.h"
#include "../TextureLoader.h"
#include "../Util.h"
#include "../PoundDefines.h"
Tile::Tile(void)
{
	length=5;
	float center=length/2.0;
	init();
	buildFX();
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans,-center,0, center);
	defaultTransform=trans;
}
Tile::Tile(float i)
{
	length=i;
	init();
	buildFX();
	float center=length/2.0;
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, -center,0, center);
	defaultTransform=trans;
	//D3DXMatrixIdentity(&defaultTransform);
}

Tile::~Tile(void)
{
}

void Tile::init()
{
	mtrl=Mtrl();
	  //create vertex buffer
    HRESULT hr= gd3dDevice->CreateVertexBuffer(4*sizeof(VertexPNT), 
        D3DUSAGE_WRITEONLY, 0 , D3DPOOL_MANAGED, &mVB,0);
	VertexPNT* v = 0;
	mVB->Lock(0, 0, (void**)&v, 0);
	float hl = length/2.0;
	float z=0;
	//top face corner at 0,0 and extends to 1,1
	v[0] = VertexPNT( 0.0f, z,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexPNT( 0.0f, z, -1.0f*length, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexPNT( 1.0f*length, z, -1.0f*length, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[3] = VertexPNT( 1.0f*length, z,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	mVB->Unlock();

	gd3dDevice->CreateIndexBuffer(6 * sizeof(WORD),	D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,	D3DPOOL_MANAGED, &mIB, 0);

	// Write box indices to the index buffer.
	WORD* i = 0;
	mIB->Lock(0, 0, (void**)&i, 0);

	// Fill in the front face index data
	i[0] = 0; i[1] = 3; i[2] = 1;
	i[3] = 3; i[4] = 2; i[5] = 1;
	mIB->Unlock();


}
void Tile::draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj)
{
	render( mWorld, mView,  mProj);
}

void Tile::render(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj)
{
	D3DXMATRIX ourWorld=mWorld;
	ourWorld=defaultTransform*mWorld;
	D3DXMATRIX worldInvTrans;
	D3DXMatrixInverse(&worldInvTrans, 0, &ourWorld);
	D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);


	   //set the shader
    mFX->SetTechnique(mhTech);
    mFX->SetMatrix(mhWVP, &(ourWorld*mView*mProj));
	mFX->SetFloatArray(mhEyePos,g_camera.position ,3);
    mFX->SetMatrix(mhWorldInvTrans, &worldInvTrans);
	mFX->SetValue(mhLight,&g_light,sizeof(DirLight));
	mFX->SetValue(mhMtrl, &mtrl, sizeof(Mtrl));
	mFX->SetMatrix(mhWorld, &ourWorld);

    mTexture=gTextureLoader->map_textures[2];
	mFX->SetTexture(mhTex, mTexture);

	(gd3dDevice->SetVertexDeclaration(VertexPNT::Decl));
	(gd3dDevice->SetStreamSource(0, mVB, 0, sizeof(VertexPNT)));
	(gd3dDevice->SetIndices(mIB));

    UINT numPasses = 0;
	(mFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		(mFX->BeginPass(i));
		(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2 ));
		(mFX->EndPass());
	}
	(mFX->End());


}

void Tile::buildFX()
{
	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HRESULT hr =D3DXCreateEffectFromFile(gd3dDevice, "PhongDirLtTex.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors);
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhTech          = mFX->GetTechniqueByName("PhongDirLtTexTech");
	mhWVP           = mFX->GetParameterByName(0, "gWVP");
	mhWorldInvTrans = mFX->GetParameterByName(0, "gWorldInvTrans");
	mhEyePos        = mFX->GetParameterByName(0, "gEyePosW");
	mhWorld         = mFX->GetParameterByName(0, "gWorld");
	mhTex           = mFX->GetParameterByName(0, "gTex");
	mhLight			= mFX->GetParameterByName(0, "gLight");
	mhMtrl			= mFX->GetParameterByName(0, "gMtrl");
}