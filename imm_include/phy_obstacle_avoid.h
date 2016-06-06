////////////////
// phy_obstacle_avoid.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
// method:
// http://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777
////////////////
////////////////
#ifndef PHY_OBSTACLE_AVOID_H
#define PHY_OBSTACLE_AVOID_H
#include "phy_prepare.h"
namespace imm
{
////////////////
// phy_collision_avoidance
////////////////
////////////////
void phy_collision_avoidance(
	const phy_property &prop_A,
	CXMVECTOR &center_A,
	CXMVECTOR &center_B,
	const float max_see_ahead,
	XMFLOAT3 &avoidance_out
)
{
	XMVECTOR ahead = XMLoadFloat3(&prop_A.vel_indirect);
	ahead = XMVector3Normalize(ahead);
	XMVECTOR ahead2 = XMVectorScale(ahead, max_see_ahead*0.5f);
	ahead = XMVectorScale(ahead, max_see_ahead);
	ahead = XMVectorAdd(ahead, center_A);
	ahead2 = XMVectorAdd(ahead2, center_A);
	//
	XMVECTOR avoidance = XMVectorSubtract(ahead, center_B);
	avoidance = XMVector3Normalize(avoidance);
	avoidance = XMVectorScale(avoidance, max_see_ahead);
	XMStoreFloat3(&avoidance_out, avoidance);
	
	
	
	
	
	
	
	
	

}
////////////////
// phy_collision_avoidance2
////////////////
////////////////
void phy_collision_avoidance2(
	const XMMATRIX &to_A_local,
	const XMMATRIX &to_A_local_inv,
	const XMFLOAT3 &center_A,
	const XMFLOAT3 &center_B,
	const float &scale_inv_A,
	const float &radius_A,
	const float &radius_B)
{
	to_A_local;
	to_A_local_inv;
	center_A;
	center_B;
	scale_inv_A;
	radius_A;
	radius_B;
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	XMVECTOR AtoB = XMVectorSubtract(c_B, c_A);
	c_B = XMVector3Transform(AtoB, to_A_local);

}
////////////////
// phy_to_inst_local
////////////////
////////////////
void phy_to_inst_local(
	const XMFLOAT4X4 &world_A,
	const XMFLOAT4X4 &rot_inv_A,
	XMMATRIX &to_A_local,
	XMMATRIX &to_A_local_inv)
{
	XMMATRIX w_A = XMLoadFloat4x4(&world_A);
	XMMATRIX ri_A = XMLoadFloat4x4(&rot_inv_A);
	to_A_local = XMMatrixMultiply(ri_A, XMMatrixInverse(nullptr, w_A));
	to_A_local_inv = XMMatrixInverse(nullptr, to_A_local);
}
//
}
#endif