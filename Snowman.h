#pragma once
#ifndef __SnowmanH__
#define __SnowmanH__
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "d3dUtility.h"
#include "Vertex.h"
using std::string;

class Snowman
{
public:
	Snowman(const LPDIRECT3DDEVICE9 mpDevice);
	~Snowman();

	bool Draw(const D3DXMATRIX* const world);
private:
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXMESH pHeadMesh;
	LPD3DXMESH pBodyMesh;
	LPD3DXMESH pEye1Mesh;
	LPD3DXMESH pEye2Mesh;
	LPD3DXMESH pNoseMesh;

	LPDIRECT3DTEXTURE9 pSnowTexture;
	LPDIRECT3DTEXTURE9 pNoseTexture;
	LPDIRECT3DTEXTURE9 pEyeTexture;

	D3DMATERIAL9 snowMtrl = d3d::WHITE_MTRL;
	const string snowTexFile = d3d::mediaPath + "snowTex.jpg";
	
	//head
	const float headRadio = 1.8f;
	//body
	const float bodyNeck = 0.5f;
	const float bodyFoot = 3.0f;
	const float bodyLen = 3.0f;
	//eye
	const float eyeLen = 0.15f;
	const D3DMATERIAL9 eyeMtr = d3d::WHITE_MTRL;
	const float eyeSplit = 15.0f;
	const float eyeHeight = 15.0f;
	const string eyeTexFile = d3d::mediaPath+"eye.jpg";
	//nose
	const float noseLen = 0.8f;
	const float noseBottomRadius = 0.15f;
	const D3DMATERIAL9 noseMtr = d3d::WHITE_MTRL;
	const string noseTexFile = d3d::mediaPath+"nose.jpg";
};
#endif //__SnowmanH__