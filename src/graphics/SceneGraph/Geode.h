#pragma once
#include "Node.h"

class Geode : public Node
{

  public:
    //Geode();
	D3DXVECTOR3  center;

    // draw the subclass of this Geode
    void draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj);

    // render the object class on screen - must be overwritten
    
};

