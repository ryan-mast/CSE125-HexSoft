#pragma once
#include "SGGroup.h"
#include <string>
using namespace std;
class MatrixTransform : public SGGroup
{
  protected:
    D3DXMATRIX transform, rotator;

  public:
    string Name;
    // create a matrix transform with the identity
    MatrixTransform(string name) { this->Name=name ; D3DXMatrixIdentity(&transform); D3DXMatrixIdentity(&rotator); }

    // create a matrix transform node with the given input matrix
    MatrixTransform(string name, D3DXMATRIX  m) { this->Name=name; transform = m; D3DXMatrixIdentity(&rotator); }

    // set the transformation matrix
    void setTransformation(D3DXMATRIX & m) { transform = m; }

    // set the rotation of this transformation
    void setRotation(D3DXMATRIX & rot);

    // return the transformation matrix
    D3DXMATRIX & getTransformation() { return transform; }

	void translate(D3DXVECTOR3  &v);

	void rotate(D3DXVECTOR3 & axis, float degrees);	
	
    void draw(D3DXMATRIX & mWorld,D3DXMATRIX & mView, D3DXMATRIX & mProj);
	void translate(float x, float y, float z);
};

