//-----------------------------------------------------------------------------
// Corey's Awesome Graphics File
//
// Same as the previous version but with custom meshes AND game world!
// Feel free to integrate with network or whatever.
//-----------------------------------------------------------------------------
#include <d3dx9.h>
#include "../game/Character.h"
#include "graphicsStuff.h"
#include "TextureLoader.h"
#include "PoundDefines.h"
#include "Vertex.h"
#include "Util.h"
#include "../game/TileBoard.h"
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
/* character objects */

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   gd3dDevice = NULL; // Our rendering device
MatrixTransform * world;
MatrixTransform * characters;
extern TileBoard * board;



TextureLoader * gTextureLoader =NULL; //for loading textures(global)
MatrixTransform * universe = new MatrixTransform("universe"); 

DirLight g_light;//global
CAMERA g_camera;//global

D3DXMATRIX mWorld;
D3DXMATRIX mView;
D3DXMATRIX mProj;
Cube * cubeGeo;
Tile * tile;



//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &gd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn on the zbuffer
    gd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    // Turn on ambient lighting 
    //gd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    InitAllVertexDeclarations();
    gTextureLoader = new TextureLoader();
	gTextureLoader->createTex(1,"../../data/crate.jpg");
	gTextureLoader->createTex(2,"../../data/grass.png");
	CameraLookAt(0,20,-10, 0,0,0, g_camera);
	CameraPerspective(50,640,480, 1 ,1000,g_camera);
	
	D3DXMatrixIdentity(& mWorld);
	cubeGeo= new Cube();
	tile = new Tile(4.9);
	
	world = new MatrixTransform("world");
	characters = new MatrixTransform("characters");
	universe->addChild(world);
	universe->addChild(characters);
	for(int i =0; i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			stringstream os;
			os << "tile"<<i<<","<<j;
			string name = os.str();
			MatrixTransform * square = new MatrixTransform(name);
			square->translate(5*i,0,5*j);
			square->addChild(tile);


			world->addChild(square);

		}
	}

	g_light.ambient=WHITE;
	g_light.ambient=WHITE;
	g_light.dirW=D3DXVECTOR3(0.0f, -1.0f,0.0f);
	g_light.spec=8.0f;


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
   

    if( gd3dDevice != NULL )
        gd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices(CAMERA camera)
{
	D3DXMatrixLookAtLH( &mView,  &camera.position, &camera.lookat, &camera.up );
	D3DXMatrixPerspectiveFovLH( &mProj, D3DXToRadian(camera.fov), camera.width/camera.height, camera.znear, 5000.0f);
   
}



void readBoard()
{
	for(int i=0;i<characters->children.size();i++)
	{
		characters->removeChildren();
	}
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			
			if(board->board[i][j]!=NULL)
			{
				MatrixTransform * m =new MatrixTransform("test"); 
				m->addChild(cubeGeo);
				characters->addChild(m);

			}
		}
	}


}
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer and the zbuffer
    gd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_RGBA(255,255,255 , 0), 1.0f, 0 );

    // Begin the scene
    gd3dDevice->BeginScene();

    // Setup the world, view, and projection matrices
    SetupMatrices(g_camera);

	readBoard();

	universe->draw(mWorld,mView,mProj);
	
	// End the scene
	gd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    gd3dDevice->Present( NULL, NULL, NULL, NULL );
}

