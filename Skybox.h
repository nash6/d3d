#pragma once
#ifndef __SkyboxH__
#define __SkyboxH__

#include "d3dUtility.h"
#include <vector>
#include <string>
using std::string;

class Skybox
{
public:
	Skybox(IDirect3DDevice9* device, const D3DXMATRIX& proj, const D3DXMATRIX& view);
	void Draw(D3DXMATRIX& viewMat);

private:
	bool init(string xFileStr, string efFileStr, string texFileStr,
		const D3DXMATRIX& proj, const D3DXMATRIX& view);
	
	IDirect3DDevice9* pDevice = NULL;
	ID3DXEffect* pEffect = NULL;
	ID3DXMesh* Mesh = NULL;

	D3DXHANDLE viewMatHandle;
	D3DXHANDLE projMatHandle;
	D3DXHANDLE texHandle;
	D3DXHANDLE techHandle;

	const string viewMat = "matView";
	const string projMat = "matProj";
	const string tex0 = "Texture0";
	const string tec0 = "tec0";

	DWORD        numMtrls;

	const string skyXFile = d3d::mediaPath + "skybox01.x";
	const string skyEfFile = "skybox01.fx";
	const string skyTexFile = d3d::mediaPath + "skybox02.dds";

};


#endif //__SkyboxH__