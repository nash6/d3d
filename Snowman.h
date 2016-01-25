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

	//head & body
	D3DMATERIAL9 snowMtrl = d3d::SNOW_MTRL;
	const string snowTexFile = d3d::mediaPath + "snowTex.png";
	const float headRadio = 1.8f;

	const float bodyNeck = 0.5f;
	const float bodyFoot = 3.0f;
	const float bodyLen = 3.0f;

	//eye
	const D3DMATERIAL9 eyeMtr = d3d::EYE_MTRL;
	const string eyeTexFile = d3d::mediaPath + "eye.jpg";
	const float eyeLen = 0.15f;
	const float eyeSplit = 20.0f;
	const float eyeHeight = 18.0f;
	
	//nose
	const D3DMATERIAL9 noseMtr = d3d::NOSE_MTRL;
	const string noseTexFile = d3d::mediaPath + "nose.jpg";
	const float noseLen = 1.f;
	const float noseBottomRadius = 0.3f;
	
};
#endif //__SnowmanH__