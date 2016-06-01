////////////////
// phy_obstacle_avoid.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_OBSTACLE_AVOID_H
#define PHY_OBSTACLE_AVOID_H
#include "phy_prepare.h"
namespace imm
{
////////////////
// phy_obstacle_avoid
////////////////
////////////////
void phy_obstacle_avoid(
	const XMMATRIX &to_A_local,
	const XMMATRIX &to_A_local_inv,
	const XMFLOAT3 &center_A,
	const XMFLOAT3 &center_B,
	XMFLOAT3 &avoid_pos,
	const float &scale_inv_A,
	float radius_B,
	const float &destination_y,
	float &obj_y,
	bool &is_erase)
{
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	//
	XMVECTOR AtoB = XMVectorSubtract(c_B, c_A);
	c_B = XMVector3Transform(AtoB, to_A_local);
	//
	obj_y = XMVectorGetY(c_B);
	if (obj_y > 0.0f || obj_y < destination_y) {
		is_erase = true;
		return;
	}
	float cbx = XMVectorGetX(c_B);
	radius_B = radius_B*scale_inv_A;
	radius_B *= 2.0f;
	if (cbx > 0.0f) cbx = cbx-radius_B;
	else cbx = cbx+radius_B;
	c_B = XMVectorSetX(c_B, cbx);
	//
	c_B = XMVector3Transform(c_B, to_A_local_inv);
	c_B = XMVectorAdd(c_B, c_A);
	XMStoreFloat3(&avoid_pos, c_B);
}
////////////////
// phy_obstacle_destination_y
////////////////
////////////////
void phy_destination_y(
	const XMFLOAT4X4 &world_A,
	const XMFLOAT4X4 &rot_inv_A,
	XMMATRIX &to_A_local,
	XMMATRIX &to_A_local_inv,
	const XMFLOAT3 &center_A,
	const XMVECTOR &center_B,
	float &destination_y)
{
	XMMATRIX w_A = XMLoadFloat4x4(&world_A);
	XMMATRIX ri_A = XMLoadFloat4x4(&rot_inv_A);
	to_A_local = XMMatrixMultiply(ri_A, XMMatrixInverse(nullptr, w_A));
	to_A_local_inv = XMMatrixInverse(nullptr, to_A_local);
	//
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR AtoB = XMVectorSubtract(center_B, c_A);
	AtoB = XMVector3Transform(AtoB, to_A_local);
	destination_y = XMVectorGetY(AtoB);
}
//
}
#endif