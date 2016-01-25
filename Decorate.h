#pragma once
#ifndef __DecorateH__
#define __DecorateH__
#include <d3dx9.h>
#include <d3d9.h>
#include <vector>
#include <string>
#include "d3dUtility.h"
using std::string;
class Decorate{
public:
	Decorate(LPDIRECT3DDEVICE9 device, const string& xFileName, const string& xTexName = "");
	
	void create();
	void draw(D3DXMATRIX& mat);

private:
	LPDIRECT3DDEVICE9 mDevice;

	string planeXFile;
	string texFile;
	LPD3DXMESH pMesh = NULL;
	std::vector<D3DMATERIAL9>       Mtrls;
	std::vector<IDirect3DTexture9*> Textures;
	
};
#endif