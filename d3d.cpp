//d3d.cpp

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>
#include <ctime>
#include "d3dUtility.h"
#include "Snowman.h"
#include "Vertex.h"
#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"
#include "pSystem.h"

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
const int Width = 640; //screen width
const int Height = 480; //screen height
const float ViewMoveSpeed = 15.0f;
const float ViewMouseSense = 1.0f;

const float BoxHeight = 5.0f;
const float BoxWidth = 10.0f;
const float BoxDepth = 10.0f;

const float BoxUpBound = 5.0f;

const float BoxRSpeed = 1.0f;

const string snowTex = d3d::mediaPath + "snowflake.dds";
const string planeXFile = d3d::mediaPath + "airplane.x";

D3DXVECTOR3 lightDirection(1.577f, -0.577f, 0.0f);
D3DXVECTOR3 cameraPos(10.0f, 10.0, 10.0);

//ptr
HWND g_hWnd = NULL;
LPDIRECT3DDEVICE9 mDevice = NULL; //d3dDevive
Camera* pCamera = NULL; //camera
Terrain* pTerrain = NULL; //
Skybox* pSkybox = NULL;
Snowman* pSnowman = NULL; //snowman 
Snowman* pSnowmanM = NULL;//moving snowman
LPD3DXMESH pBoxMesh = NULL;
LPDIRECT3DTEXTURE9    pBoxTexture = NULL;
LPD3DXMESH pPlaneMesh = NULL;
std::vector<D3DMATERIAL9>       planeMtrls(0);
std::vector<IDirect3DTexture9*> planeTextures(0);
LPD3DXFONT        p3dxFont = NULL;
psys::PSystem* pSnow = 0;
//var
D3DLIGHT9 mlight;
POINT  g_ptLastMousePosit;
POINT  g_ptCurrentMousePosit;
bool   g_bMousing = false;

double g_dCurTime;
double g_dLastTime;


//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow);
void shutDown(void);
bool Display(float timeDelta);
bool Setup();
void InputViewChange(float g_fElpasedTime);
void createBox();
void createPlane();
void drawPlane();
////


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	if (!d3d::InitD3D(hInstance,
		640, 480, true, D3DDEVTYPE_HAL, &mDevice, nCmdShow, g_hWnd))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	shutDown();

	mDevice->Release();

	return 0;
}

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK d3d::WndProc(HWND   hWnd,
	UINT   msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case 'V':
			//pCamera->ReverseFollowFlag();
			break;
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		g_bMousing = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		g_bMousing = false;
	}
	break;

	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;

	default:
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: 
//-----------------------------------------------------------------------------
void shutDown(void)
{
	if (p3dxFont != NULL)
		p3dxFont->Release();
	if (mDevice != NULL)
		mDevice->Release();
	d3d::Delete<psys::PSystem*>(pSnow);

}


bool Display(float timeDelta)
{
	//Cal Box Mat
	D3DXMATRIX mBoxTranslation;
	D3DXMATRIX mBoxOrbitRotation;
	D3DXMATRIX mBoxMatrix;
	static float fBoxOrbit = 0.0f;
	float tmpR = BoxRSpeed * (timeDelta * 20.0f);
	fBoxOrbit += tmpR;
	D3DXMatrixTranslation(&mBoxTranslation, 0.0f, BoxHeight / 2, 20.0f);
	D3DXMatrixRotationY(&mBoxOrbitRotation, D3DXToRadian(fBoxOrbit));
	mBoxMatrix = mBoxTranslation * mBoxOrbitRotation;

	//camera follow box
	D3DXVECTOR3 v(mBoxMatrix._41, mBoxMatrix._42 + BoxHeight / 2 + 2.0f, mBoxMatrix._43);
	pCamera->setFollowPos(&v, D3DXToRadian(tmpR));
	
	//ViewMat
	InputViewChange(timeDelta);
	D3DXMATRIX view;
	pCamera->getViewMatrix(&view);
	mDevice->SetTransform(D3DTS_VIEW, &view);
	
	//
	pSnow->update(timeDelta);
	//clear
	mDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		0xff000000, 1.0f, 0);

	//BeginScene
	mDevice->BeginScene();

	//draw terrain and sky
	pSkybox->Draw(view);
	pTerrain->Draw(view);
	
	//Box
	mDevice->SetTexture(0, pBoxTexture);
	D3DMATERIAL9 mtrl = d3d::WHITE_MTRL;
	mDevice->SetMaterial(&mtrl);

	mDevice->SetTransform(D3DTS_WORLD, &mBoxMatrix);
	pBoxMesh->DrawSubset(0);

	//moving snowman
	D3DXMATRIX t;
	D3DXMatrixTranslation(&t, 0.0f, BoxHeight/2, 0.0f);
	mBoxMatrix *= t;
	pSnowmanM->Draw(&mBoxMatrix);
	

	//still snowman
	D3DXMATRIX tmpTranslation;
	D3DXMatrixTranslation(&tmpTranslation, 0.0f, 1.0f, 0.0f);
	pSnowman->Draw(&tmpTranslation);

	//
	pSnow->render();

	//plane
	D3DXMATRIX pMat;
	D3DXMatrixTranslation(&pMat, 10.0f, 20.0f, 0.0f);
	pMat *= mBoxOrbitRotation;
	mDevice->SetTransform(D3DTS_WORLD, &pMat);
	drawPlane();

	//font
	RECT destRect1;
	SetRect(&destRect1, 0, 0, 0, 0);
	string out = "Camera Status: ";
	if (pCamera->getFollow())
		out += "On Box.";
	else
		out += "Free.";
	p3dxFont->DrawText(NULL, out.c_str(), -1, &destRect1, DT_NOCLIP,d3d::CYAN);

	

	//EndScene
	mDevice->EndScene();

	mDevice->Present(NULL, NULL, NULL, NULL);

	return TRUE;
}

void createBox(void)
{
	D3DXCreateTextureFromFile(mDevice, (d3d::mediaPath + "crate.jpg").c_str(), &pBoxTexture);

	mDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	mDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	mDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	LPD3DXMESH pTempBoxMesh = NULL;
	D3DXCreateBox(
		mDevice,
		BoxWidth, // Width
		BoxHeight, // Height
		BoxDepth, // Depth
		&pTempBoxMesh,
		0);
	pCamera->setBoxSize(BoxHeight, BoxWidth, BoxDepth);
	LPDIRECT3DVERTEXBUFFER9 pTempVertexBuffer;

	pTempBoxMesh->CloneMeshFVF(0, Vertex::FVF_Flags, mDevice, &pBoxMesh);

	if (SUCCEEDED(pBoxMesh->GetVertexBuffer(&pTempVertexBuffer)))
	{
		int nNumVerts = pBoxMesh->GetNumVertices();
		Vertex *pVertices = NULL;

		pTempVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
		{
			for (int i = 0; i < nNumVerts; ++i) {
				if (i % 4 == 0 || i % 4 == 3)
					pVertices[i]._u = 0.0;
				else
					pVertices[i]._u = 1.0;
				if (i % 4 == 0 || i % 4 == 1)
					pVertices[i]._v = 0.0;
				else
					pVertices[i]._v = 1.0;
			}
		}
		pTempVertexBuffer->Unlock();

		pTempVertexBuffer->Release();
	}

}

void initDirLight(D3DXVECTOR3& lightDir) {
	mlight = d3d::InitDirectionalLight(lightDir, d3d::WHITE);
	mlight.Ambient = d3d::WHITE * 0.5f;
	mlight.Diffuse = d3d::WHITE * 0.6f;
	mlight.Specular = d3d::WHITE;
	mDevice->SetLight(0, &mlight);
	mDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	mDevice->LightEnable(0, true);
}

bool Setup(){
	srand((unsigned int)time(0));
	//
	d3d::BoundingBox boundingBox;
	boundingBox._min = D3DXVECTOR3(-100.0f, -5.0f, -100.0f);
	boundingBox._max = D3DXVECTOR3(100.0f, 15.0f, 100.0f);
	pSnow = new psys::Snow(&boundingBox, 5000);
	pSnow->init(mDevice, snowTex.c_str());


	//light
	initDirLight(lightDirection);

	//Camera
	pCamera = new Camera(Camera::LANDOBJECT);
	pCamera->setPosition(&cameraPos);

	// Set the projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	mDevice->SetTransform(D3DTS_PROJECTION, &proj);


	D3DXMATRIX view;
	pCamera->getViewMatrix(&view);
	//Terrain
	pTerrain = new Terrain(mDevice, proj, view, mlight);
	//Skybox
	pSkybox = new Skybox(mDevice, proj, view);

	//snowman 1 & 2
	pSnowman = new Snowman(mDevice);
	pSnowmanM = new Snowman(mDevice);

	//Box
	createBox();

	//font
	int nHeight;
	HDC hDC;
	int nPointSize = 14;
	hDC = GetDC(NULL);
	nHeight = -(MulDiv(nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72));
	D3DXCreateFont(mDevice, nHeight, 0, 0, 0, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
		&p3dxFont);

	createPlane();

	return TRUE;
}

void InputViewChange(float g_fElpasedTime)
{
	
	// Get mouse input...
	POINT mousePosit;
	GetCursorPos(&mousePosit);
	ScreenToClient(g_hWnd, &mousePosit);

	g_ptCurrentMousePosit.x = mousePosit.x;
	g_ptCurrentMousePosit.y = mousePosit.y;

	//D3DXMATRIX matRotation;

	if (g_bMousing)
	{
		int nXDiff = (g_ptCurrentMousePosit.x - g_ptLastMousePosit.x);
		int nYDiff = (g_ptCurrentMousePosit.y - g_ptLastMousePosit.y);

		if (nYDiff != 0)
			pCamera->pitch(D3DXToRadian((float)nYDiff / ViewMouseSense));

		if (nXDiff != 0)
			pCamera->yaw(D3DXToRadian((float)nXDiff / ViewMouseSense));
	}

	g_ptLastMousePosit.x = g_ptCurrentMousePosit.x;
	g_ptLastMousePosit.y = g_ptCurrentMousePosit.y;

	//
	// Get keyboard input...
	//
	unsigned char keys[256];
	GetKeyboardState(keys);

	// Up Arrow Key
	if (keys['W'] & 0x80)
		pCamera->walk(ViewMoveSpeed*g_fElpasedTime);

	// Down Arrow Key
	if (keys['S'] & 0x80)
		pCamera->walk(-ViewMoveSpeed*g_fElpasedTime);
		
	// Left Arrow Key
	if (keys['A'] & 0x80)
		pCamera->strafe(-ViewMoveSpeed*g_fElpasedTime);
		
	// Right Arrow Key
	if (keys['D'] & 0x80)
		pCamera->strafe(ViewMoveSpeed*g_fElpasedTime);
		
	// View elevates up
	if (keys['Q'] & 0x80)
		pCamera->fly(ViewMoveSpeed*g_fElpasedTime);
		
	// View elevates down
	if (keys['E'] & 0x80)
		pCamera->fly(-ViewMoveSpeed*g_fElpasedTime);

}

void drawPlane() {
	for (int i = 0; i < planeMtrls.size(); i++)
	{
		mDevice->SetMaterial(&planeMtrls[i]);
		mDevice->SetTexture(0, planeTextures[i]);
		pPlaneMesh->DrawSubset(i);
	}
}
void createPlane() {
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
		&pPlaneMesh);

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return;
	}

	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for (int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			planeMtrls.push_back(mtrls[i].MatD3D);

			// check if the ith material has an associative texture
			if (mtrls[i].pTextureFilename != 0)
			{
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					mDevice,
					(d3d::mediaPath + string(mtrls[i].pTextureFilename)).c_str(),
					&tex);

				// save the loaded texture
				planeTextures.push_back(tex);
			}
			else
			{
				// no texture for the ith subset
				planeTextures.push_back(0);
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer); // done w/ buffer

											//
											// Optimize the mesh.
											//

	hr = pPlaneMesh->OptimizeInplace(
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



