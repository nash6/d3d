#include "Decorate.h"
#include "d3dUtility.h"

Decorate::Decorate(LPDIRECT3DDEVICE9 device, const string& xFileName, const string& xTexName)
	:mDevice(device), planeXFile(xFileName), texFile(xTexName){

	create();
}

void Decorate::draw(D3DXMATRIX& mat) {
	mDevice->SetTransform(D3DTS_WORLD, &mat);
	for (size_t i = 0; i < Mtrls.size(); i++)
	{
		mDevice->SetMaterial(&Mtrls[i]);
		mDevice->SetTexture(0, Textures[i]);
		pMesh->DrawSubset(i);
	}
}
void Decorate::create() {
	HRESULT hr = 0;

	// Load the XFile data.
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		planeXFile.c_str(),
		D3DXMESH_MANAGED,
		mDevice,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&pMesh);

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return;
	}

	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for (DWORD i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			Mtrls.push_back(mtrls[i].MatD3D);

			// check if the ith material has an associative texture
			if (mtrls[i].pTextureFilename != 0)
			{
				string texStr;
				if (!texFile.empty())
					texStr = texFile;
				else
					texStr = d3d::mediaPath + string(mtrls[i].pTextureFilename);
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					mDevice,
					texStr.c_str(),
					&tex);

				// save the loaded texture
				Textures.push_back(tex);
			}
			else
			{
				// no texture for the ith subset
				Textures.push_back(0);
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer); // done w/ buffer
								
	// Optimize the mesh.
	hr = pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);
	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer

	if (FAILED(hr))
	{
		::MessageBox(0, "OptimizeInplace() - FAILED", 0, 0);
		return;
	}

}





