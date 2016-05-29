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
	XMFLOAT4X4 &world_A,
	XMFLOAT4X4 &rot_inv_A,
	const XMFLOAT3 &center_A,
	const XMFLOAT3 &center_B,
	XMFLOAT3 &avoid_pos,
	const float &scale_inv_A,
	float radius_B)
{
	XMMATRIX w_A = XMLoadFloat4x4(&world_A);
	XMMATRIX ri_A = XMLoadFloat4x4(&rot_inv_A);
	XMMATRIX w_A_inv = XMMatrixInverse(nullptr, w_A);
	w_A_inv = XMMatrixMultiply(ri_A, w_A_inv);
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	radius_B = radius_B*scale_inv_A;
	//
	XMVECTOR AtoB = XMVectorSubtract(c_B, c_A);
	c_B = XMVector3Transform(AtoB, w_A_inv);
	//
	float cbx = XMVectorGetX(c_B);
	cbx;
	radius_B *= 2.0f;
	if (cbx > 0.0f) cbx = cbx-radius_B;
	else cbx = cbx+radius_B;
	c_B = XMVectorSetX(c_B, cbx);
	//
	XMMATRIX w_A_inv_inv = XMMatrixInverse(nullptr, w_A_inv);
	c_B = XMVector3Transform(c_B, w_A_inv_inv);
	c_B = XMVectorAdd(c_B, c_A);
	XMStoreFloat3(&avoid_pos, c_B);
}
//
}
#endif