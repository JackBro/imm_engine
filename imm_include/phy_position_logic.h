////////////////
// phy_position_logic.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_POSITION_LOGIC_H
#define PHY_POSITION_LOGIC_H
#include "phy_prepare.h"
namespace imm
{
// note if every delta time is not equal, simulation will be some distortion
////////////////
// PHY const
////////////////
////////////////
static const float PHY_FLOAT_EQUAL_1METER = 5.0f;
static const float PHY_GRAVITY_RATE = 1.5f;
static const float PHY_GRAVITY = -9.8f*PHY_FLOAT_EQUAL_1METER*PHY_GRAVITY_RATE;
// if too small velocity rebound, ignore it
static const float PHY_IGNORE_GRAVITY = 1.8f;
// if runtime stun, restrict delta time not too big
static const float PHY_MAX_DELTA_TIME = 0.3f;
// AABB six face normal
XMGLOBALCONST XMVECTORF32 PHY_AABB_NORMAL[6] = {
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, -1.0f, 0.0f},
	{0.0f, -1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f, 0.0f},
	{-1.0f, 0.0f, 0.0f, 0.0f}
};
////////////////
// phy_position_update
////////////////
////////////////
void phy_position_update(
	const float &dt,
	XMFLOAT4X4 &world,
	phy_property &prop,
	phy_property &prop_ground,
	const XMFLOAT3 &center,
	const bool &is_on_ground,
	const float &half_y,
	const float &ground_y)
{
	if (prop.is_abnormal) return;
	if (!is_on_ground) {
		prop.velocity.x += prop.acceleration.x*dt;
		prop.velocity.y += (PHY_GRAVITY+prop.acceleration.y)*dt;
		prop.velocity.z += prop.acceleration.z*dt;
		world._41 += prop.velocity.x*dt + prop.velocity_nm.x*dt;
		world._42 += prop.velocity.y*dt + prop.velocity_nm.y*dt;
		world._43 += prop.velocity.z*dt + prop.velocity_nm.z*dt;
	}
	else {
		float bounce = prop.bounce*prop_ground.bounce;
		float friction_rev = prop.friction_rev*prop_ground.friction_rev;
		prop.acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (prop.velocity.y < 0.0f) prop.velocity.y = -prop.velocity.y*bounce;
		prop.velocity.x *= friction_rev;
		prop.velocity.z *= friction_rev;
		// use center compare stand
		float offset_y = world._42-center.y;
		float center_y = center.y;
		// stand_adjust keep object full stand on ground
		// guarantee object.intersects(ground) return true, exclude if they have a little gap
		float stand_adjust = -0.01f;
		float stand = half_y+ground_y+stand_adjust;
		center_y += prop.velocity.y*dt + prop.velocity_nm.y*dt;
		world._41 += prop.velocity.x*dt + prop.velocity_nm.x*dt;
		world._43 += prop.velocity.z*dt + prop.velocity_nm.z*dt;
		if (abs(prop.velocity.y) < PHY_IGNORE_GRAVITY) prop.velocity.y = 0.0f;
		if (center_y < stand) center_y = stand;
		world._42 = center_y+offset_y;
	}
	// too big number
	if (abs(prop.velocity.x) > 1000.0f ||
		abs(prop.velocity.y) > 1000.0f ||
		abs(prop.velocity.z) > 1000.0f) {
		prop.is_abnormal = true;
	}
	return;
}
////////////////
// phy_boxA_normal
// not proper solution, for test only
////////////////
////////////////
XMVECTORF32 phy_boxA_normal(const BoundingBox &bbox_A, const BoundingBox &bbox_B)
{
	// get boxA vertex
	XMFLOAT3 corners[8];
	bbox_A.GetCorners(corners);
	std::vector<XMVECTOR> bv;
	for (size_t ix = 0; ix != 8; ++ix) bv.push_back(XMLoadFloat3(&corners[ix]));
	// get boxB center
	XMVECTOR bbox_B_center = XMLoadFloat3(&bbox_B.Center);
	// boxA six face, calculate each midpoint
	std::vector<XMVECTOR> bm(6);
	bm[0] = XMVectorScale(XMVectorAdd(bv[0], bv[2]), 0.5f);
	bm[1] = XMVectorScale(XMVectorAdd(bv[7], bv[2]), 0.5f);
	bm[2] = XMVectorScale(XMVectorAdd(bv[4], bv[6]), 0.5f);
	bm[3] = XMVectorScale(XMVectorAdd(bv[0], bv[5]), 0.5f);
	bm[4] = XMVectorScale(XMVectorAdd(bv[4], bv[3]), 0.5f);
	bm[5] = XMVectorScale(XMVectorAdd(bv[1], bv[6]), 0.5f);
	// determine boxA which face nearest boxB center
	float length;
	float length_min = FLT_MAX;
	size_t min = 0;
	for (size_t ix = 0; ix != 6; ++ix) {
		length = XMVectorGetX(XMVector3Length(XMVectorSubtract(bm[ix], bbox_B_center)));
		if (length < length_min) {
			length_min = length;
			min = ix;
		}
	}
	return PHY_AABB_NORMAL[min];
}
////////////////
// phy_impulse_casual
// collision impulse method modify from
// \Microsoft DirectX SDK (June 2010)\Samples\C++\Direct3D\ConfigSystem\main.cpp
// it is originally used with two spheres, but there is used with two AABB or others, inaccuracy solution
////////////////
////////////////
void phy_impulse_casual(
	const float &dt,
	XMFLOAT4X4 &world_A,
	XMFLOAT4X4 &world_B,
	phy_property &prop_A,
	phy_property &prop_B,
	const XMFLOAT3 &center_A,
	const XMFLOAT3 &center_B,
	const bool &is_touch = true)
{
	if (!is_touch) return;
	XMMATRIX w_A = XMLoadFloat4x4(&world_A);
	XMMATRIX w_B = XMLoadFloat4x4(&world_B);
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	XMVECTOR offset_A = XMVectorSubtract(w_A.r[3], c_A);
	XMVECTOR offset_B = XMVectorSubtract(w_B.r[3], c_B);
	// AtoB is the collision normal vector.
	XMVECTOR AtoB = XMVector3Normalize(XMVectorSubtract(c_B, c_A));
	XMVECTOR vel_A = XMLoadFloat3(&prop_A.velocity);
	XMVECTOR vel_B = XMLoadFloat3(&prop_B.velocity);
	// empirical formula, velocity_nm is different from velocity
	XMVECTOR vel_A_nm = XMLoadFloat3(&prop_A.velocity_nm);
	XMVECTOR vel_B_nm = XMLoadFloat3(&prop_B.velocity_nm);
	XMVECTOR vel_A_all = XMVectorAdd(vel_A, vel_A_nm);
	XMVECTOR vel_B_all = XMVectorAdd(vel_B, vel_B_nm);
	// penetration depth estimate, not accurate, increase its value
	float penetration_scale = 1.5f;
	float penetration = XMVectorGetX(XMVector3Length(XMVectorSubtract(vel_A_all, vel_B_all)))*dt*penetration_scale;
	// bounce
	float bounce = prop_A.bounce*prop_B.bounce;
	XMVECTOR vel_AB_all = XMVectorSubtract(vel_A_all, vel_B_all);
	// ignore mass
	XMVECTOR injected_impulse = XMVectorScale(
		AtoB, XMVectorGetX(XMVector3Dot(XMVectorScale(vel_AB_all, -1.0f*bounce), AtoB)));
	vel_A = XMVectorAdd(vel_A, injected_impulse);
	vel_B = XMVectorSubtract(vel_B, injected_impulse);
	// Fix the positions so that the two objects are apart, not accurate
	c_A = XMVectorSubtract(c_A, XMVectorScale(AtoB, penetration));
	c_B = XMVectorAdd(c_B, XMVectorScale(AtoB, penetration));
	w_A.r[3] = XMVectorAdd(c_A, offset_A);
	w_B.r[3] = XMVectorAdd(c_B, offset_B);
	w_A.r[3] = XMVectorSetW(w_A.r[3], 1.0f);
	w_B.r[3] = XMVectorSetW(w_B.r[3], 1.0f);
	XMStoreFloat3(&prop_A.velocity, vel_A);
	XMStoreFloat3(&prop_B.velocity, vel_B);
	XMStoreFloat4x4(&world_A, w_A);
	XMStoreFloat4x4(&world_B, w_B);
	return;
}
//
}
#endif