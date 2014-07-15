#include "Cube.h"
#include "../Vertex.h"
#include "../TextureLoader.h"
#include "../Util.h"
#include "../PoundDefines.h"
#include <d3dx9math.h>
Cube::Cube()
{
	float length=1;
	float center=2.5;
    init();
	buildFX();
	D3DXMATRIX scale;
	D3DXMATRIX trans;
	D3DXMatrixScaling(&scale,length,length,length);
	D3DXMatrixTranslation(&trans, 0,1,0);
	defaultTransform=scale*trans;
}

Cube::~Cube()
{
	mVB->Release();
	mIB->Release();
	mTexture->Release();
}

void Cube::init()
{
 
    //create vertex buffer
    HRESULT hr= gd3dDevice->CreateVertexBuffer(24*sizeof(VertexPNT), 
        D3DUSAGE_WRITEONLY, 0 , D3DPOOL_MANAGED, &mVB,0);

    
	// Write box vertices to the vertex buffer.
	VertexPNT* v = 0;
	mVB->Lock(0, 0, (void**)&v, 0);

	// Fill in the front face vertex data.
	v[0] = VertexPNT(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = VertexPNT(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = VertexPNT( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = VertexPNT( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = VertexPNT(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[5] = VertexPNT( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = VertexPNT( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[7] = VertexPNT(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8]  = VertexPNT(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[9]  = VertexPNT(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[10] = VertexPNT( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = VertexPNT( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = VertexPNT(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = VertexPNT( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = VertexPNT( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[15] = VertexPNT(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = VertexPNT(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[17] = VertexPNT(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[18] = VertexPNT(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[19] = VertexPNT(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = VertexPNT( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[21] = VertexPNT( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = VertexPNT( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = VertexPNT( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	mVB->Unlock();


	// Create the index buffer.
	gd3dDevice->CreateIndexBuffer(36 * sizeof(WORD),	D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,	D3DPOOL_MANAGED, &mIB, 0);

	// Write box indices to the index buffer.
	WORD* i = 0;
	mIB->Lock(0, 0, (void**)&i, 0);

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mIB->Unlock();


}

void Cube::draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj)
{

    render( mWorld, mView,  mProj);
}

void Cube::render(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj)
{
	
	mWorld=defaultTransform*mWorld;
    D3DXMATRIX worldInvTrans;
	D3DXMatrixInverse(&worldInvTrans, 0, &mWorld);
	D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
	HRESULT hr;
    //set the shader
    mFX->SetTechnique(mhTech);
    mFX->SetMatrix(mhWVP, &(mWorld*mView*mProj));
    mFX->SetMatrix(mhWorldInvTrans, &worldInvTrans);
	mFX->SetMatrix(mhWorld, &mWorld);

	mFX->SetFloatArray(mhEyePos,g_camera.position ,3);
	mFX->SetValue(mhLight,&g_light,sizeof(DirLight));
	mFX->SetValue(mhMtrl, &mtrl, sizeof(Mtrl));

    mTexture=gTextureLoader->map_textures[1];
	mFX->SetTexture(mhTex, mTexture);

	gd3dDevice->SetVertexDeclaration(VertexPNT::Decl);
	gd3dDevice->SetStreamSource(0, mVB, 0, sizeof(VertexPNT));
	gd3dDevice->SetIndices(mIB);

    UINT numPasses = 0;
	hr=(mFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		hr=(mFX->BeginPass(i));
		hr=(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12));
		hr=(mFX->EndPass());
	}
	hr=(mFX->End());

}


void Cube::buildFX()
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