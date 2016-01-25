#pragma once
#pragma once
#ifndef __TerrainH__
#define __TerrainH__

#include "d3dUtility.h"
#include <vector>
#include <string>
using std::string;

class Terrain
{
public:
	Terrain(IDirect3DDevice9* device, const D3DXMATRIX& proj, const D3DXMATRIX& view, D3DLIGHT9& lightDir);
	void Draw(D3DXMATRIX& viewMat);

private:
	void init(string xFileStr, string efFileStr, string texFileStr,
		const D3DXMATRIX& proj, const D3DXMATRIX& view, D3DLIGHT9& lightDir);
	void setLight(D3DLIGHT9& light);
	IDirect3DDevice9* pDevice = NULL;

	ID3DXEffect* pEffect = 0;

	D3DXHANDLE worldMatHandle = 0;
	D3DXHANDLE viewMatHandle = 0;
	D3DXHANDLE projMatHandle = 0;
	D3DXHANDLE texHandle = 0;
	D3DXHANDLE techHandle = 0;
	D3DXHANDLE ligthHandle = 0;

	ID3DXMesh* Mesh = 0;
	DWORD numMtrls = 0;

	const string terrainXFile = d3d::mediaPath + "terrain01.x";
	const string terrainEfFile = "snow.fx";
	const string terrainTexFile = d3d::mediaPath + "ripples.dds";
	
};

#endif // __TerrainH__