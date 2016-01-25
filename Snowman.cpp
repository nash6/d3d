#include "Snowman.h"

Snowman::Snowman(const LPDIRECT3DDEVICE9 mDevice)
	:pDevice(mDevice) {

	LPDIRECT3DVERTEXBUFFER9 pTempVertexBuffer;
	//head
	LPD3DXMESH pTempHeadMesh;
	HRESULT re = D3DXCreateSphere(
		pDevice,
		headRadio, //Radius
		15, //Slice
		15, //Stack
		&pTempHeadMesh,
		NULL
		);

	pTempHeadMesh->CloneMeshFVF(0, Vertex::FVF_Flags, pDevice, &pHeadMesh);

	pHeadMesh->GetVertexBuffer(&pTempVertexBuffer);
	{
		int nNumVerts = pHeadMesh->GetNumVertices();
		Vertex *pVertices = NULL;
		pTempVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
		
		for (int i = 0; i < nNumVerts; ++i) {
			pVertices[i]._u = d3d::GetRandomFloat(0, 1);//random texture
			pVertices[i]._v = d3d::GetRandomFloat(0, 1);//random texture
		}
		
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
	}
	d3d::Release<LPD3DXMESH>(pTempHeadMesh);

	//body
	LPD3DXMESH pTempBodyMesh;
	re = D3DXCreateCylinder(
		pDevice,
		bodyNeck, //Radius at the negative Z end
		bodyFoot, //Radius at the positive Z end.
		bodyLen, //Length,
		15, //Slices,
		15, //Stacks,
		&pTempBodyMesh,
		NULL
		);
	if (FAILED(re))
		return;

	pTempBodyMesh->CloneMeshFVF(0, Vertex::FVF_Flags, pDevice, &pBodyMesh);

	pBodyMesh->GetVertexBuffer(&pTempVertexBuffer);
	{
		int nNumVerts = pBodyMesh->GetNumVertices();
		Vertex *pVertices = NULL;

		pTempVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
		
		for (int i = 0; i < nNumVerts; ++i) {
			pVertices[i]._u = d3d::GetRandomFloat(0, 1);//random texture
			pVertices[i]._v = d3d::GetRandomFloat(0, 1);//random texture
		}

		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
		
	}
	d3d::Release<LPD3DXMESH>(pTempBodyMesh);

	//eye
	LPD3DXMESH pTempEyeMesh;
	re = D3DXCreatePolygon(
		pDevice,
		eyeLen, //Length,
		10, //Sides,
		&pTempEyeMesh,
		NULL
		);
	pTempEyeMesh->CloneMeshFVF(0, Vertex::FVF_Flags, pDevice, &pEye1Mesh);
	pEye1Mesh->GetVertexBuffer(&pTempVertexBuffer);
	{
		int nNumVerts = pEye1Mesh->GetNumVertices();
		Vertex *pVertices = NULL;
		pTempVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
		{
			pVertices[0]._u = 0.5;
			pVertices[0]._v = 0.5;
			float delta = D3DX_PI * 2 / (nNumVerts - 1);
			float theta = 0;
			for (int i = 1; i < nNumVerts; ++i) {
				pVertices[i]._u = 0.5f * cos(theta) + 0.5f;
				pVertices[i]._v = 0.5f - 0.5f * sin(theta);
				theta += delta;
			}
		}
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
	}
	
	pEye1Mesh->CloneMeshFVF(0, Vertex::FVF_Flags, pDevice, &pEye2Mesh);

	d3d::Release<LPD3DXMESH>(pTempEyeMesh);
	
	//nose
	LPD3DXMESH pTempNoseMesh;
	re = D3DXCreateCylinder(
		pDevice,
		noseBottomRadius, //Radius at the negative Z end
		0.01f, //Radius at the positive Z end.
		noseLen, //Length,
		10, //Slices,
		10, //Stacks,
		&pTempNoseMesh,
		NULL
		);
	pTempNoseMesh->CloneMeshFVF(0, Vertex::FVF_Flags, pDevice, &pNoseMesh);
	pNoseMesh->GetVertexBuffer(&pTempVertexBuffer);
	{
		int nNumVerts = pNoseMesh->GetNumVertices();
		Vertex *pVertices = NULL;
		pTempVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
		{
			for (int i = 0; i < nNumVerts; ++i) {
				pVertices[i]._u = d3d::GetRandomFloat(0, 1);
				pVertices[i]._v = d3d::GetRandomFloat(0, 1);
			}
		}
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
	}
	d3d::Release<LPD3DXMESH>(pTempNoseMesh);

	//tex
	D3DXCreateTextureFromFile(pDevice, snowTexFile.c_str(), &pSnowTexture);
	D3DXCreateTextureFromFile(pDevice, noseTexFile.c_str(), &pNoseTexture);
	D3DXCreateTextureFromFile(pDevice, eyeTexFile.c_str(), &pEyeTexture);

	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

}

Snowman::~Snowman() {
	d3d::Release<LPD3DXMESH>(pHeadMesh);
	d3d::Release<LPD3DXMESH>(pBodyMesh);
	d3d::Release<LPD3DXMESH>(pEye1Mesh);
	d3d::Release<LPD3DXMESH>(pEye2Mesh);
	d3d::Release<LPD3DXMESH>(pNoseMesh);

	d3d::Release<LPDIRECT3DTEXTURE9>(pSnowTexture);
	d3d::Release<LPDIRECT3DTEXTURE9>(pNoseTexture);
	d3d::Release<LPDIRECT3DTEXTURE9>(pEyeTexture);
}

bool Snowman::Draw(const D3DXMATRIX* const world) {
	//head & body
	pDevice->SetMaterial(&snowMtrl);
	pDevice->SetTexture(0, pSnowTexture);
	
	//draw head
	D3DXMATRIX headMat;
	D3DXMatrixTranslation(&headMat, 0.0f, bodyLen + headRadio - 1.0f, 0.0f);
	headMat *= (*world);
	pDevice->SetTransform(D3DTS_WORLD, &headMat);
	pHeadMesh->DrawSubset(0);

	//draw body
	D3DXMATRIX rxMat, tmpMat;
	D3DXMatrixRotationX(&rxMat, D3DXToRadian(90.0f));
	D3DXMatrixTranslation(&tmpMat, 0.0f, bodyLen / 2, 0.0f);
	D3DXMATRIX bodyMat = rxMat * tmpMat * (*world);
	pDevice->SetTransform(D3DTS_WORLD, &bodyMat);
	pBodyMesh->DrawSubset(0);

	//draw eye
	pDevice->SetTexture(0, pEyeTexture);
	pDevice->SetMaterial(&eyeMtr);
	D3DXMATRIX Mat, eyeRx, eyeRy, eyeMat1, eyeMat2;
	//eye 1
	D3DXMatrixRotationX(&eyeRx, D3DXToRadian(-eyeHeight));
	D3DXMatrixRotationY(&eyeRy, D3DXToRadian(eyeSplit));
	D3DXMatrixTranslation(&Mat, 0.0f, 0.0f, headRadio);
	eyeMat1 = Mat * eyeRx * eyeRy * headMat;
	pDevice->SetTransform(D3DTS_WORLD, &eyeMat1);
	pEye1Mesh->DrawSubset(0);
	//eye 2
	D3DXMatrixRotationY(&eyeRy, D3DXToRadian(-eyeSplit));
	eyeMat2 = Mat * eyeRx * eyeRy* headMat;
	pDevice->SetTransform(D3DTS_WORLD, &eyeMat2);
	pEye2Mesh->DrawSubset(0);

	//draw nose
	pDevice->SetTexture(0, pNoseTexture);
	pDevice->SetMaterial(&noseMtr);
	D3DXMatrixTranslation(&Mat, 0.0f, 0.0f, headRadio + noseLen / 2);
	Mat *= headMat;
	pDevice->SetTransform(D3DTS_WORLD, &Mat);
	pNoseMesh->DrawSubset(0);

	return true;
}
