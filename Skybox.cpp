#include "Skybox.h"

Skybox::Skybox(IDirect3DDevice9* device, const D3DXMATRIX& proj, const D3DXMATRIX& view) {
	pDevice = device;
	init(skyXFile, skyEfFile, skyTexFile,
		proj, view);
}

void Skybox::Draw(D3DXMATRIX& viewMat) {
	
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

bool Skybox::init(string xFileStr, string efFileStr, string texFileStr,
	const D3DXMATRIX& proj, const D3DXMATRIX& view) {

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
		return false;
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
		return false;
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	// Save Frequently Accessed Parameter Handles
	viewMatHandle = pEffect->GetParameterByName(0, viewMat.c_str());
	projMatHandle = pEffect->GetParameterByName(0, projMat.c_str());
	texHandle = pEffect->GetParameterByName(0, tex0.c_str());
	techHandle = pEffect->GetTechniqueByName(tec0.c_str());


	pEffect->SetMatrix(viewMatHandle, &view);
	pEffect->SetMatrix(projMatHandle, &proj);

	// Set texture
	IDirect3DCubeTexture9* tex = 0;
	hr = D3DXCreateCubeTextureFromFile(pDevice, texFileStr.c_str(), &tex);
	hr = pEffect->SetTexture("Texture0", tex);
	d3d::Release<IDirect3DCubeTexture9*>(tex);

	return true;
}
