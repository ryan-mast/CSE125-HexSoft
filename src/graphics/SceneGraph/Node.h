#pragma once
#include <d3dx9.h>

class Node
{

  public:

    virtual void draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj) = 0;
};
