////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_position.z = 0.0f;

	m_rotation.x = 0.0f;
	m_rotation.y = 0.0f;
	m_rotation.z = 0.0f;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;

	DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DefaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	/*m_pitch = m_rotation.x * 0.0174532925f;
	m_yaw = m_rotation.y * 0.0174532925f;
	m_roll = m_rotation.z * 0.0174532925f;*/

	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

void CameraClass::SetMoveLeftRight(float speed)
{
	//moveLeftRight = speed;
	m_position.x += speed;
}

void CameraClass::SetMoveBackForward(float speed)
{
	moveBackForward = speed;
}

void CameraClass::SetMoveUpDown(float speed)
{
	moveUpDown = speed;
}

void CameraClass::SetYawPitch(float yaw, float pitch)
{
	m_yaw = yaw;
	m_pitch = pitch;
	//m_roll += roll;
}
void CameraClass::SetPitchPlus(float plus)
{
	m_pitch += plus;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}


XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}

// This uses the position and rotation of the camera to build and to update the view matrix.
void CameraClass::Render()
{
	XMMATRIX rotationMatrix;
	
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0);

	m_lookAt = XMVector3TransformCoord(DefaultForward, rotationMatrix);
	m_lookAt = XMVector3Normalize(m_lookAt);


	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_yaw);


	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	XMVECTOR position = XMLoadFloat3(&m_position);
	
	position += moveLeftRight * camRight;
	position += moveBackForward * camForward;
	position += moveUpDown * camUp;
	
	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;

	m_lookAt = position + m_lookAt;
	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, m_lookAt, camUp);

	forward = m_lookAt-position;
	up = XMVector3Cross(forward, camRight);

	XMStoreFloat3(&m_position, position);


}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}
XMVECTOR CameraClass::GetLookAt()
{
	return m_lookAt;
}