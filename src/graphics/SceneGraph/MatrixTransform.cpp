#include <d3dx9.h>
#include "SGGroup.h"
#include "MatrixTransform.h"
#include <iostream>
#include "../Util.h"
#include <d3dx9math.h>
static int counter = 0;

void MatrixTransform::draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj)
{

    
    std::list<Node*>::iterator nodes;

    D3DXMATRIX tf;

    //std::cout << "BEFORE TRANFORM: " << std::endl;

    //tf.printMe();

    tf=rotator*transform*mWorld;



    for(nodes = children.begin(); nodes != children.end(); ++nodes)
    {
      (*nodes)->draw(  tf, mView,   mProj);
    }
}

void MatrixTransform::setRotation(D3DXMATRIX & rot)
{
  rotator = rot;
}


void MatrixTransform::translate(D3DXVECTOR3 & v)
{
	D3DXMATRIX t;
	D3DXMatrixTranslation(&t, v.x, v.y, v.z);
	transform=t*transform;
}

void MatrixTransform::rotate( D3DXVECTOR3 & v, float degrees)
{
	D3DXMATRIX r;
	D3DXMatrixRotationAxis(&r, &v, degrees/180*3.14); 
	rotator=rotator*r;
}
void MatrixTransform::translate(float x, float y, float z)
{
	translate(D3DXVECTOR3(x,y,z));
}