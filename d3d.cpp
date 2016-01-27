
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
#include "Decorate.h"

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
const int Width = 640; //screen width
const int Height = 480; //screen height
const float ViewMoveSpeed = 10.0f;
const float ViewMouseSense = 1.0f;
//Box
const float BoxHeight = 5.0f;
const float BoxWidth = 10.0f;
const float BoxDepth = 10.0f;
const float BoxUpBound = 5.0f;
const float BoxRSpeed = 1.0f;
const float ground = 1.0f;
//file name
const string snowTex = d3d::mediaPath + "snowflake.dds";
const string houseXFile = d3d::mediaPath + "polHouse1.x";
const string planeXFile = d3d::mediaPath + "airplane.x";
const string treeXFile = d3d::mediaPath + "tree.X";
//light & camera
D3DXVECTOR3 lightDirection(0.0f, -0.9f, 0.577f);
D3DXVECTOR3 cameraPos(10.0f, 10.0, 10.0);

//mouse
POINT  g_ptLastMousePosit;
POINT  g_ptCurrentMousePosit;
bool   g_bMousing = false;
double g_dCurTime;
double g_dLastTime;

//ptr
HWND g_hWnd = NULL;
LPDIRECT3DDEVICE9 mDevice = NULL; //d3dDevive
Camera* pCamera = NULL; //camera
Terrain* pTerrain = NULL; //
Skybox* pSkybox = NULL;
Snowman* pSnowman = NULL; //snowman 
Snowman* pSnowmanM = NULL;//moving snowman
Decorate* pHouse = NULL;
Decorate* pPlane = NULL;
Decorate* pTree = NULL;
LPD3DXMESH pBoxMesh = NULL;
LPDIRECT3DTEXTURE9 pBoxTexture = NULL;
LPD3DXFONT p3dxFont = NULL;
psys::PSystem* pSnow = NULL;
D3DLIGHT9 mlight;


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
		//case 'V':
			//pCamera->ReverseFollowFlag();
			//break;
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
	d3d::Release<LPDIRECT3DDEVICE9>(mDevice);
	d3d::Release<LPD3DXFONT>(p3dxFont);

	d3d::Delete<psys::PSystem*>(pSnow);
	d3d::Delete<Camera*>(pCamera);
	d3d::Delete<Terrain*>(pTerrain);
	d3d::Delete<Skybox*>(pSkybox);
	d3d::Delete<Snowman*>(pSnowman);
	d3d::Delete<Snowman*>(pSnowmanM);
	d3d::Delete<Decorate*>(pHouse);
	d3d::Delete<Decorate*>(pPlane);
	d3d::Delete<Decorate*>(pTree);

	d3d::Release<LPD3DXMESH>(pBoxMesh);
	d3d::Release<LPDIRECT3DTEXTURE9>(pBoxTexture);
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
	D3DXMatrixTranslation(&mBoxTranslation, 0.0f, BoxHeight / 2 + ground, 20.0f);
	D3DXMatrixRotationY(&mBoxOrbitRotation, D3DXToRadian(fBoxOrbit));
	mBoxMatrix = mBoxTranslation * mBoxOrbitRotation;

	//camera follow box
	D3DXVECTOR3 v(mBoxMatrix._41, mBoxMatrix._42 + BoxHeight / 2, mBoxMatrix._43);
	pCamera->setFollowPos(&v, D3DXToRadian(tmpR));
	
	//set ViewMat
	InputViewChange(timeDelta);
	D3DXMATRIX view;
	pCamera->getViewMatrix(&view);
	mDevice->SetTransform(D3DTS_VIEW, &view);
	
	//snow update
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
	D3DMATERIAL9 mtrl = d3d::BOX_MTRL;
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
	D3DXMatrixTranslation(&tmpTranslation, 0.0f, ground, 0.0f);
	pSnowman->Draw(&tmpTranslation);

	//plane
	D3DXMATRIX pMat, pRMat;
	D3DXMatrixTranslation(&pMat, 10.0f, 20.0f, 0.0f);
	D3DXMatrixRotationZ(&pRMat, D3DXToRadian(40));
	pMat = pRMat * pMat * mBoxOrbitRotation;
	pPlane->draw(pMat);
	
	//house
	D3DXMATRIX hMat, hRMat;
	D3DXMatrixRotationY(&hRMat, D3DXToRadian(180));
	D3DXMatrixTranslation(&hMat, -18.0f, ground, - 40.0f);
	hMat = hRMat * hMat;
	pHouse->draw(hMat);

	//tree
	D3DXMATRIX tMat, sMat, treeMat;
	D3DXMatrixTranslation(&tMat, 20.0f, ground + 8.0f, 60.0f);

	D3DXMatrixScaling(&sMat, 0.1f, 0.1f, 0.1f);
	treeMat = sMat * tMat;
	pTree->draw(treeMat);

	D3DXMatrixTranslation(&tMat, 14.0f, ground + 8.0f, 55.0f);
	treeMat = sMat * tMat;
	pTree->draw(treeMat);

	D3DXMatrixTranslation(&tMat, 10.0f, ground + 8.0f, 54.0f);
	treeMat = sMat * tMat;
	pTree->draw(treeMat);

	D3DXMatrixTranslation(&tMat, 8.0f, ground + 8.0f, 65.0f);
	treeMat = sMat * tMat;
	pTree->draw(treeMat);

	//font
	RECT destRect1;
	SetRect(&destRect1, 0, 0, 0, 0);
	string out = "Camera Status: ";
	if (pCamera->getFollow())
		out += "On Box.";
	else
		out += "Free.";
	p3dxFont->DrawText(NULL, out.c_str(), -1, &destRect1, DT_NOCLIP,d3d::CYAN);

	//snow
	D3DXMatrixIdentity(&sMat);
	mDevice->SetTransform(D3DTS_WORLD, &sMat);
	pSnow->render();

	//EndScene
	mDevice->EndScene();

	mDevice->Present(NULL, NULL, NULL, NULL);

	return TRUE;
}

void createBox()
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
		pTempVertexBuffer->Unlock();
		pTempVertexBuffer->Release();
	}

}

void initDirLight(D3DXVECTOR3& lightDir) {
	D3DXVec3Normalize(&lightDir, &lightDir);
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
	//snow init
	d3d::BoundingBox boundingBox;
	boundingBox._min = D3DXVECTOR3(-100.0f, ground - 0.5f, -100.0f);
	boundingBox._max = D3DXVECTOR3(100.0f, 40.0f, 100.0f);
	pSnow = new psys::Snow(&boundingBox, 10000);
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
	pTerrain->setLight(mlight);
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

	//decorate
	pPlane = new Decorate(mDevice, planeXFile);
	pHouse = new Decorate(mDevice, houseXFile, d3d::mediaPath + "phouse_d.jpg");
	pTree = new Decorate(mDevice, treeXFile);

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

	// Get keyboard input...
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


