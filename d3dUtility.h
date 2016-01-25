#pragma once
#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include <d3dx9.h>
#include <d3d9.h>
#include <string>
using std::string;

namespace d3d
{
	bool InitD3D(
		HINSTANCE hInstance,       // [in] Application instance.
		int width, int height,     // [in] Backbuffer dimensions.
		bool windowed,             // [in] Windowed (true)or full screen (false).
		D3DDEVTYPE deviceType,     // [in] HAL or REF
		LPDIRECT3DDEVICE9* device, // [out]The created device.
		int nCmdShow,
		HWND& g_hWnd);

	int EnterMsgLoop(
		bool(*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

	template<class T> void Release(T t)
	{
		if (t)
		{
			t->Release();
			t = 0;
		}
	}

	template<class T> void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
	}
	const std::string mediaPath = "media\\";	//media file path

	//bulit-in D3DXCOLOR
	const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
	const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
	const D3DXCOLOR BEACH_SAND(D3DCOLOR_XRGB(255, 249, 157));
	const D3DXCOLOR DESERT_SAND(D3DCOLOR_XRGB(250, 205, 135));
	const D3DXCOLOR LIGHTGREEN(D3DCOLOR_XRGB(60, 184, 120));
	const D3DXCOLOR  PUREGREEN(D3DCOLOR_XRGB(0, 166, 81));
	const D3DXCOLOR  DARKGREEN(D3DCOLOR_XRGB(0, 114, 54));
	const D3DXCOLOR LIGHT_YELLOW_GREEN(D3DCOLOR_XRGB(124, 197, 118));
	const D3DXCOLOR  PURE_YELLOW_GREEN(D3DCOLOR_XRGB(57, 181, 74));
	const D3DXCOLOR  DARK_YELLOW_GREEN(D3DCOLOR_XRGB(25, 123, 48));
	const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
	const D3DXCOLOR DARKBROWN(D3DCOLOR_XRGB(115, 100, 87));

	
	
	// Lights
	D3DLIGHT9 InitDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color);
	D3DLIGHT9 InitPointLight(const D3DXVECTOR3& position, const D3DXCOLOR& color);
	D3DLIGHT9 InitSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXCOLOR& color);

	// Materials
	D3DMATERIAL9 InitMtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, const D3DXCOLOR& s, 
		const D3DXCOLOR& e, float p);

	//bulit-in D3DMATERIAL9
	const D3DMATERIAL9 BLACK_MTRL = InitMtrl(BLACK, BLACK, BLACK, BLACK, 2.0f);
	const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	const D3DCOLORVALUE SnowD = { 0.95f, 0.95f, 1.f, 1.f };
	const D3DCOLORVALUE SnowA = { 0.95f, 0.95f, 1.f, 1.f };
	const D3DCOLORVALUE SnowS = { 0.2f, 0.2f,  0.2f, 1.f };
	
	const D3DMATERIAL9 SNOW_MTRL = InitMtrl(SnowA, SnowD, SnowS, WHITE*0.1f, 4.0f);
	const D3DMATERIAL9 BOX_MTRL = InitMtrl(LIGHTBROWN, LIGHTBROWN, LIGHTBROWN, BLACK, 1.0f);
	const D3DMATERIAL9 NOSE_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW*0.5f, BLACK, 2.0f);
	const D3DMATERIAL9 EYE_MTRL = InitMtrl(DARKBROWN, DARKBROWN, DARKBROWN*0.1f, DARKBROWN, 1.0f);

	float Lerp(float a, float b, float t);

	//bounding box for snow system
	struct BoundingBox
	{
		BoundingBox();

		bool isPointInside(D3DXVECTOR3& p);

		D3DXVECTOR3 _min;
		D3DXVECTOR3 _max;
	};

	//Return random float in [lowBound, highBound] interval.
	float GetRandomFloat(float lowBound, float highBound);
	
	//Returns a random vector in the bounds specified by min and max.
	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max);

	DWORD FtoDw(float f);// Conversion

}
#endif // __d3dUtilityH__
