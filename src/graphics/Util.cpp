#include "Util.h"
#include "Vertex.h"



void LoadXFile(
	const std::string& filename, 
	ID3DXMesh** meshOut,
	std::vector<Mtrl>& mtrls, 
	std::vector<IDirect3DTexture9*>& texs)
{
	// Step 1: Load the .x file from file into a system memory mesh.

	ID3DXMesh* meshSys      = 0;
	ID3DXBuffer* adjBuffer  = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD numMtrls          = 0;

	(D3DXLoadMeshFromX( filename.c_str(), D3DXMESH_SYSTEMMEM, gd3dDevice,
		&adjBuffer,	&mtrlBuffer, 0, &numMtrls, &meshSys));


	// Step 2: Find out if the mesh already has normal info?

	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	(meshSys->GetDeclaration(elems));
	
	bool hasNormals = false;
	D3DVERTEXELEMENT9 term = D3DDECL_END();
	for(int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		// Did we reach D3DDECL_END() {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}?
		if(elems[i].Stream == 0xff )
			break;

		if( elems[i].Type == D3DDECLTYPE_FLOAT3 &&
			elems[i].Usage == D3DDECLUSAGE_NORMAL &&
			elems[i].UsageIndex == 0 )
		{
			hasNormals = true;
			break;
		}
	}


	// Step 3: Change vertex format to VertexPNT.

	D3DVERTEXELEMENT9 elements[64];
	UINT numElements = 0;
	VertexPNT::Decl->GetDeclaration(elements, &numElements);

	ID3DXMesh* temp = 0;
	(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM, 
		elements, gd3dDevice, &temp));
	ReleaseCOM(meshSys);
	meshSys = temp;


	// Step 4: If the mesh did not have normals, generate them.

	if( hasNormals == false)
		(D3DXComputeNormals(meshSys, 0));


	// Step 5: Optimize the mesh.

	(meshSys->Optimize(D3DXMESH_MANAGED | 
		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, 
		(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, meshOut));
	ReleaseCOM(meshSys); // Done w/ system mesh.
	ReleaseCOM(adjBuffer); // Done with buffer.

	// Step 6: Extract the materials and load the textures.

	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* d3dxmtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(DWORD i = 0; i < numMtrls; ++i)
		{
			// Save the ith material.  Note that the MatD3D property does not have an ambient
			// value set when its loaded, so just set it to the diffuse value.
			Mtrl m;
			m.ambient   = d3dxmtrls[i].MatD3D.Diffuse;
			m.diffuse   = d3dxmtrls[i].MatD3D.Diffuse;
			m.spec      = d3dxmtrls[i].MatD3D.Specular;
			m.specPower = d3dxmtrls[i].MatD3D.Power;
			mtrls.push_back( m );

			// Check if the ith material has an associative texture
			if( d3dxmtrls[i].pTextureFilename != 0 )
			{
				// Yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				char* texFN = d3dxmtrls[i].pTextureFilename;
				(D3DXCreateTextureFromFile(gd3dDevice, texFN, &tex));

				// Save the loaded texture
				texs.push_back( tex );
			}
			else
			{
				// No texture for the ith subset
				texs.push_back( 0 );
			}
		}
	}
	ReleaseCOM(mtrlBuffer); // done w/ buffer
}


void CameraLookAt(float px,float py, float pz, float lx, float ly, float lz, CAMERA & a)
{
	a.position=D3DXVECTOR3(px,py,pz);
	a.lookat=D3DXVECTOR3(lx,ly,lz);
	//D3DXVECTOR3 right=D3DXVECTOR3(1,0,0);
	//D3DXVec3Cross(&a.up,&-a.lookat,&right);
	// a.up=D3DXVECTOR3(0,0,1);
	a.up=D3DXVECTOR3(0,1,0);
}
void CameraPerspective(float fov, float w, float h, float n, float f, CAMERA & a)
{
  a.fov=fov;
  a.width=w;
  a.height=h;
  a.znear=n;
  a.zfar=f;
}

void moveThisCamera(float x,float y, float z,CAMERA & a)
{
	
	a.position=D3DXVECTOR3(a.position.x+x,a.position.y+y,a.position.z+z);
	a.lookat=D3DXVECTOR3(a.lookat.x+x,a.lookat.y+y,a.lookat.z+z);
	//a.up=D3DXVECTOR3(0,0,1);
}
void CameraMove(float x,float y, float z,CAMERA &camera)
{
	moveThisCamera(x,y,z,camera);
	
}

void CamMoveY (float f, CAMERA & camera)
{
	camera.position.y+=f;
}