

#include "d3dUtility.h"


bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	LPDIRECT3DDEVICE9* device,
	int nCmdShow,
	HWND& g_hWnd)
{
	//
	// Create the main application window.
	//

	WNDCLASSEX winClass;
	MSG        uMsg;

	memset(&uMsg, 0, sizeof(uMsg));

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = (WNDPROC)d3d::WndProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIcon(hInstance, 0);
	winClass.hIconSm = LoadIcon(hInstance, 0);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName = NULL;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;

	if (!RegisterClassEx(&winClass))
		return FALSE;

	g_hWnd = CreateWindowEx(NULL, "MY_WINDOWS_CLASS",
		"Direct3D (DX9) - Initialization",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, width, height, NULL, NULL, hInstance, NULL);

	if (g_hWnd == NULL)
		return FALSE;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	
	//
	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.
	LPDIRECT3D9 g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	D3DDISPLAYMODE d3ddm;

	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		// TO DO: Respond to failure of GetAdapterDisplayMode
		return FALSE;
	}


	if (FAILED(hr = g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, D3DFMT_D16)))
	{
		if (hr == D3DERR_NOTAVAILABLE)
			// POTENTIAL PROBLEM: We need at least a 16-bit z-buffer!
			return FALSE;
	}


	// Step 2: Check for hardware vp.
	D3DCAPS9 d3dCaps;
	if (FAILED(g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, &d3dCaps)))
	{
		::MessageBox(0, "GetDeviceCaps() - FAILED", 0, 0);
		return false;
	}

	DWORD dwBehaviorFlags = 0;

	if (d3dCaps.VertexProcessingCaps != 0)
		dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;


	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, deviceType, g_hWnd,
		dwBehaviorFlags, &d3dpp, device)))
	{
		g_pD3D->Release(); // done with d3d9 object
		::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
		return false;
	}


	g_pD3D->Release(); // done with d3d9 object

	return true;
}

int d3d::EnterMsgLoop(bool(*ptr_display)(float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}

D3DLIGHT9 d3d::InitDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight(const D3DXVECTOR3& position, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_POINT;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Position = position;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_SPOT;
	light.Ambient = color * 0.0f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Position = position;
	light.Direction = direction;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta = 0.4f;
	light.Phi = 0.9f;

	return light;
}

D3DMATERIAL9 d3d::InitMtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, const D3DXCOLOR& s, const D3DXCOLOR& e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = a;
	mtrl.Diffuse = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power = p;
	return mtrl;
}

float d3d::Lerp(float a, float b, float t)
{
	return a - (a*t) + (b*t);
}

d3d::BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = FLT_MAX;
	_min.y = FLT_MAX;
	_min.z = FLT_MAX;

	_max.x = -FLT_MAX;
	_max.y = -FLT_MAX;
	_max.z = -FLT_MAX;
}

bool d3d::BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float d3d::GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound;
}

void d3d::GetRandomVector(
	D3DXVECTOR3* out,
	D3DXVECTOR3* min,
	D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD d3d::FtoDw(float f)
{
	return *((DWORD*)&f);
}



