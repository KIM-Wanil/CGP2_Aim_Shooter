////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>

#include "AlignedAllocationPolicy.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMVECTOR GetLookAt();

	void Render();
	void GetViewMatrix(XMMATRIX&);
	float GetCamYaw() { return m_yaw; };
	float GetCamPitch() { return m_pitch; };
	XMVECTOR GetCamUp() { return XMVector3Normalize(up); }; //+(camRight/3)-(camUp/3)); };
	XMVECTOR GetCamForward() { return XMVector3Normalize(forward); };
	XMVECTOR GetCamRight() { return XMVector3Normalize(camRight); };


	void SetMoveLeftRight(float);
	void SetMoveBackForward(float);
	void SetMoveUpDown(float);
	void SetYawPitch(float, float);
	void SetPitchPlus(float);


private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMVECTOR m_lookAt;
	float moveLeftRight;
	float moveBackForward;
	float moveUpDown;

	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;
	XMVECTOR DefaultUp;
	XMVECTOR camForward;
	XMVECTOR camRight;
	XMVECTOR camUp;

	XMVECTOR forward;
	XMVECTOR up;

	float m_yaw, m_pitch, m_roll;
};

#endif