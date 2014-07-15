#pragma once
#include "PoundDefines.h"
#include <d3d9.h>
#include <d3dx9.h>
//#include <dxerr.h>
#include <string>
#include <sstream>
#include <vector>
#include "TextureLoader.h"
//handle to global directx device

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }


//cosnts for calculation
const float INFINITY = FLT_MAX;
const float EPSILON  = 0.001f;
//===============================================================
// Colors and Materials
const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);




struct CAMERA
{
  D3DXVECTOR3 position, lookat,up;
  float fov,width,height,znear,zfar;
};
struct Mtrl
{
	Mtrl()
		:ambient(WHITE), diffuse(WHITE), spec(WHITE), specPower(8.0f){}
	Mtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, 
		 const D3DXCOLOR& s, float power)
		:ambient(a), diffuse(d), spec(s), specPower(power){}

	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	float specPower;
};

struct DirLight
{
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	D3DXVECTOR3 dirW;
};



struct AABB 
{
	// Initialize to an infinitely small bounding box.
	AABB()
		: minPt(INFINITY, INFINITY, INFINITY),
		  maxPt(-INFINITY, -INFINITY, -INFINITY){}

    D3DXVECTOR3 center()
	{
		return 0.5f*(minPt+maxPt);
	}

	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;
};

struct BoundingSphere 
{
	BoundingSphere()
		: pos(0.0f, 0.0f, 0.0f), radius(0.0f){}

	D3DXVECTOR3 pos;
	float radius;
};

//function declarations
void LoadXFile(
	const std::string& filename, 
	ID3DXMesh** meshOut, 
	std::vector<Mtrl>& mtrls, 
	std::vector<IDirect3DTexture9*>& texs);
void moveThisCamera(float x,float y, float z,CAMERA & a);
void CameraMove(float x,float y, float z,CAMERA &camera);
void CamMoveY (float f, CAMERA & camera);
void CameraLookAt(float px,float py, float pz, float lx, float ly, float lz, CAMERA & a);
void CameraPerspective(float fov, float w, float h, float n, float f, CAMERA & a);

//extern globals
extern TextureLoader * gTextureLoader;
extern IDirect3DDevice9* gd3dDevice;
extern DirLight g_light;
extern CAMERA g_camera;