#pragma once
#ifndef __cameraH__
#define __cameraH__

#include <d3dx9.h>
#include <cmath> 
class Camera
{
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };

	Camera();
	Camera(CameraType cameraType);
	~Camera();

	void strafe(float units); // left/right
	void fly(float units);    // up/down
	void walk(float units);   // forward/backward

	void pitch(float angle); // rotate on right vector
	void yaw(float angle);   // rotate on up vector
	void roll(float angle);  // rotate on look vector

	void getViewMatrix(D3DXMATRIX* V);
	void setCameraType(CameraType cameraType);
	void getPosition(D3DXVECTOR3* pos);
	void setPosition(D3DXVECTOR3* pos);
	void setFollowPos(D3DXVECTOR3* pos, float orbit);

	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);

	bool getFollow();
	void setBoxSize(float h, float w, float d);

private:
	CameraType  _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;

	bool  outofBox();
	bool  floatAbsBigger(float a, float b);
	void judgeFollow(D3DXVECTOR3 moving, float units);
	bool followBox = false;

	//Box Coord
	D3DXVECTOR3 boxPos;
	D3DXVECTOR3 C2BoxPos;
	float boxH = 0;
	float boxW = 0;
	float boxD = 0;

	const float maxH = 50.0f;
};
#endif // __cameraH__