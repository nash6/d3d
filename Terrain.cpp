#include "Terrain.h"


Terrain::Terrain(IDirect3DDevice9* device, const D3DXMATRIX& proj, const D3DXMATRIX& view, D3DLIGHT9& lightDir) {
	pDevice = device;
	init(terrainXFile, terrainEfFile, terrainTexFile,
		proj, view, lightDir);
}

void Terrain::setLight(D3DLIGHT9& lightDir) {
	float lDir[] = { lightDir.Direction.x, lightDir.Direction.y, lightDir.Direction.z };
	pEffect->SetFloatArray(ligthHandle, lDir, 3);
	float la[] = { lightDir.Ambient.a, lightDir.Ambient.b, lightDir.Ambient.g, lightDir.Ambient.r };
	pEffect->SetFloatArray("I_a", la, 4);
	float ld[] = { lightDir.Diffuse.a, lightDir.Diffuse.b, lightDir.Diffuse.g, lightDir.Diffuse.r };
	pEffect->SetFloatArray("I_d", ld, 4);
	float ls[] = { lightDir.Specular.a, lightDir.Specular.b, lightDir.Specular.g, lightDir.Specular.r };
	pEffect->SetFloatArray("I_s", ls, 4);
}

void Terrain::Draw(D3DXMATRIX& viewMat) {

	pEffect->SetMatrix(viewMatHandle, &viewMat);
	pEffect->SetTechnique(techHandle);

	UINT numPasses = 0;
	pEffect->Begin(&numPasses, 0);

	for (UINT i = 0; i < numPasses; i++)
	{
		pEffect->BeginPass(i);
		for (UINT j = 0; j < numMtrls; j++)
		{
			Mesh->DrawSubset(j);
		}
		pEffect->EndPass();
	}
	pEffect->End();

}

void Terrain::init(string xFileStr, string efFileStr, string texFileStr,
			const D3DXMATRIX& proj, const D3DXMATRIX& view, D3DLIGHT9& lightDir) {

	HRESULT hr = 0;
	
	// Load the XFile data.
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	
	hr = D3DXLoadMeshFromX(
		xFileStr.c_str(),
		D3DXMESH_MANAGED,
		pDevice,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&Mesh);

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return;
	}
	
	// Create effect.
	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		pDevice,
		efFileStr.c_str(),
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // compile flags
		0,                // don't share parameters
		&pEffect,
		&errorBuffer);

	// output any error messages
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return;
	}

	
	// Save Frequently Accessed Parameter Handles
	worldMatHandle = pEffect->GetParameterByName(0, "matWorld");
	viewMatHandle = pEffect->GetParameterByName(0, "matViewInv");
	projMatHandle = pEffect->GetParameterByName(0, "matViewProj");
	texHandle = pEffect->GetParameterByName(0, "Texture0");
	techHandle = pEffect->GetTechniqueByName("tec0");
	ligthHandle = pEffect->GetParameterByName(0, "lightDir");

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	pEffect->SetMatrix(worldMatHandle, &W);
	pEffect->SetMatrix(viewMatHandle, &view);
	pEffect->SetMatrix(projMatHandle, &proj);
	setLight(lightDir);
	// Set texture
	IDirect3DTexture9* tex = 0;
	D3DXCreateTextureFromFile(pDevice, texFileStr.c_str(), &tex);

	pEffect->SetTexture(texHandle, tex);
	d3d::Release<IDirect3DTexture9*>(tex);

}
