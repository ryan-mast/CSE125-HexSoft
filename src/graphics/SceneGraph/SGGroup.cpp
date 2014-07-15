#include "SGGroup.h"


void SGGroup::draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj)
{
  std::list<Node*>::iterator nodes;

  for(nodes = children.begin(); nodes != children.end(); ++nodes)
  {
    (*nodes)->draw(  mWorld,  mView,   mProj);
  }
}