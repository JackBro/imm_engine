////////////////
// imm_camera.h
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
// Simple first person style camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be constructed.
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
////////////////
////////////////
#ifndef IMM_CAMERA_H
#define IMM_CAMERA_H
#include "imm_core.h"
namespace imm
{
////////////////
// camera
////////////////
////////////////
class camera
{
public:
	camera();
	~camera();
	// get/set world camera position.
	XMVECTOR get_PositionXM() const;
	XMFLOAT3 get_Position() const;
	void set_Position(float x, float y, float z);
	void set_Position(const XMFLOAT3& v);
	// get camera basis vectors.
	XMVECTOR get_RightXM() const;
	XMFLOAT3 get_Right() const;
	XMVECTOR get_UpXM() const;
	XMFLOAT3 get_Up() const;
	XMVECTOR get_LookXM() const;
	XMFLOAT3 get_Look() const;
	// get frustum properties.
	float get_NearZ() const;
	float get_FarZ() const;
	float get_Aspect() const;
	float get_FovY() const;
	float get_FovX() const;
	// get near and far plane dimensions in view space coordinates.
	float get_NearWindowWidth() const;
	float get_NearWindowHeight() const;
	float get_FarWindowWidth() const;
	float get_FarWindowHeight() const;
	// set frustum.
	void set_Lens(float fov_y, float aspect, float zn, float zf);
	// Define camera space via look_at parameters.
	void look_at(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR world_up);
	void look_at(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
	// get View/Proj matrices.
	XMMATRIX get_View() const;
	XMMATRIX get_Proj() const;
	XMMATRIX get_ViewProj() const;
	// strafe/walk the camera a distance d.
	void strafe(float d);
	void walk(float d);
	void up_down(float d);
	// Rotate the camera.
	void pitch(float angle);
	void rotate_y(float angle);
	// After modifying camera position/orientation, call to rebuild the view matrix.
	void update_view_matrix();
	void reset(const int &preset);
//private:
	// Camera coordinate system with coordinates relative to world space.
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;
	// Cache frustum properties.
	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY;
	float m_NearWindowHeight;
	float m_FarWindowHeight;
	int m_Preset;
	// Cache View/Proj matrices.
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;
};
//
camera::camera():
	m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Look(0.001f, 0.0f, 1.0f),
	m_NearZ(0.0f),
	m_FarZ(0.0f),
	m_Aspect(0.0f),
	m_FovY(0.0f),
	m_NearWindowHeight(0.0f),
	m_FarWindowHeight(0.0f),
	m_Preset(0)
{
	set_Lens(0.25f*XM_PI, 1.0f, 1.0f, 1000.0f);
}
//
camera::~camera()
{
	;
}
//
XMVECTOR camera::get_PositionXM() const {return XMLoadFloat3(&m_Position);}
XMFLOAT3 camera::get_Position() const {return m_Position;}
void camera::set_Position(float x, float y, float z) {m_Position = XMFLOAT3(x, y, z);}
void camera::set_Position(const XMFLOAT3& v) {m_Position = v;}
XMVECTOR camera::get_RightXM() const {return XMLoadFloat3(&m_Right);}
XMFLOAT3 camera::get_Right() const {return m_Right;}
XMVECTOR camera::get_UpXM() const {return XMLoadFloat3(&m_Up);}
XMFLOAT3 camera::get_Up() const {return m_Up;}
XMVECTOR camera::get_LookXM() const {return XMLoadFloat3(&m_Look);}
XMFLOAT3 camera::get_Look() const {return m_Look;}
float camera::get_NearZ() const {return m_NearZ;}
float camera::get_FarZ() const {return m_FarZ;}
float camera::get_Aspect() const {return m_Aspect;}
float camera::get_FovY() const {return m_FovY;}
float camera::get_FovX() const {float halfWidth = 0.5f*get_NearWindowWidth(); return 2.0f*atan(halfWidth / m_NearZ);}
float camera::get_NearWindowWidth() const {return m_Aspect * m_NearWindowHeight;}
float camera::get_NearWindowHeight() const {return m_NearWindowHeight;}
float camera::get_FarWindowWidth() const {return m_Aspect * m_FarWindowHeight;}
float camera::get_FarWindowHeight() const {return m_FarWindowHeight;}
XMMATRIX camera::get_View() const {return XMLoadFloat4x4(&m_View);}
XMMATRIX camera::get_Proj() const {return XMLoadFloat4x4(&m_Proj);}
XMMATRIX camera::get_ViewProj() const {return XMMatrixMultiply(get_View(), get_Proj());}
//
void camera::set_Lens(float fov_y, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fov_y;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;
	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f*m_FovY);
	m_FarWindowHeight  = 2.0f * m_FarZ * tanf(0.5f*m_FovY);
	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}
//
void camera::look_at(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR world_up)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(world_up, L));
	XMVECTOR U = XMVector3Cross(L, R);
	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}
//
void camera::look_at(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);
	look_at(P, T, U);
}
//
void camera::strafe(float d)
{
	// m_Position += d*m_Right
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));
}
//
void camera::walk(float d)
{
	// m_Position += d*m_Look
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}
//
void camera::up_down(float d)
{
	// m_Position += d*m_Look
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Up);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}
//
void camera::pitch(float angle)
{
	// Rotate up and look vector about the right vector.
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}
//
void camera::rotate_y(float angle)
{
	// Rotate the basis vectors about the world y-axis.
	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}
//
void camera::update_view_matrix()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);
	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));
	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);
	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);
	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;
	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;
	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;
	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;
}
//
void camera::reset(const int &preset = 0)
{
	m_Preset = preset;
	if (m_Preset == 0) {
		m_Right = {1.0f, 0.0f, 0.0f};
		m_Up = {0.0f, 1.0f, 0.0f};
		m_Look = {0.001f, 0.0f, 1.0f};
	}
	else {
		m_Right = {1.0f, 0.0f, 0.0f};	
		m_Up = {0.0f, 1.0f, 0.0f};
		m_Look = {0.001f, -1.0f, 0.7f};
	}
}
//
}
#endif