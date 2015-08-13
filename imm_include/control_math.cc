////////////////
// control_math.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_math.h"

namespace imm {namespace math
{
void mouse_move_toward_hit(
	CXMVECTOR &hit_pos,
	const size_t &index,
	const float &speed)
{
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &rot_front = *PTR->m_Inst.m_Stat[index].get_RotFront();
	XMFLOAT3 *phy_velocity_nm = &PTR->m_Inst.m_Stat[index].phy.velocity_nm;
	XMVECTOR velocity_nm = XMLoadFloat3(phy_velocity_nm);
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	velocity_nm = XMVectorSubtract(hit_pos, W.r[3]);
	velocity_nm = XMVectorSetY(velocity_nm, 0.0f);
	velocity_nm = XMVector3Normalize(velocity_nm);
	// use velocity_nm as front direction
	mouse_face_rot_y(W, RF, velocity_nm);
	// when update stop, use object own speed
	velocity_nm = XMVectorScale(velocity_nm, speed);
	XMStoreFloat3(phy_velocity_nm, velocity_nm);
	XMStoreFloat4x4(&world, W);
}
//
void mouse_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	CXMVECTOR &direction)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	float rot_radians = atan2(-XMVectorGetX(direction), -XMVectorGetZ(direction));
	R = XMMatrixRotationY(rot_radians);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);	
}
//










//
}}