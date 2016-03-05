////////////////
// phy_position_logic.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
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
static const float PHY_MAX_DELTA_TIME = 0.1f;
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
	const float &extents_y,
	const float &ground_y)
{
	if (prop.is_abnormal) return;
	if (!prop.is_on_ground) {
		prop.velocity.x += prop.acceleration.x*dt;
		prop.velocity.y += (PHY_GRAVITY+prop.acceleration.y)*dt;
		prop.velocity.z += prop.acceleration.z*dt;
		world._41 += prop.velocity.x*dt + prop.vel_indirect.x*dt;
		world._42 += prop.velocity.y*dt + prop.vel_indirect.y*dt;
		world._43 += prop.velocity.z*dt + prop.vel_indirect.z*dt;
	}
	else {
		float bounce = prop.bounce*prop_ground.bounce;
		float friction_rev = prop.friction_rev*prop_ground.friction_rev;
		prop.acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (prop.velocity.y < 0.0f) prop.velocity.y = -prop.velocity.y*bounce;
		prop.velocity.x *= friction_rev*0.1f;
		prop.velocity.z *= friction_rev*0.1f;
		// use center compare stand
		float offset_y = world._42-center.y;
		float center_y = center.y;
		// stand_adjust keep object full stand on ground
		// guarantee object.intersects(ground) return true, exclude if they have a little gap
		float stand_adjust = -0.01f;
		float stand = extents_y+ground_y+stand_adjust;
		center_y += prop.velocity.y*dt + prop.vel_indirect.y*dt;
		world._41 += prop.velocity.x*dt + prop.vel_indirect.x*dt;
		world._43 += prop.velocity.z*dt + prop.vel_indirect.z*dt;
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
// phy_impulse_casual
// collision impulse method modify from
// \Microsoft DirectX SDK (June 2010)\Samples\C++\Direct3D\ConfigSystem\main.cpp
// it is originally used with two spheres, but there is used with two AABB or others, inaccuracy solution
// defect1: if objects chain impulse, object on ground will not apart enough with velocity compare to on air
////////////////
////////////////
void phy_impulse_casual(
	const float &dt,
	const float &extents_y_A,
	const float &extents_y_B,
	const size_t &ix_A,
	const size_t &ix_B,
	XMFLOAT4X4 &world_A,
	XMFLOAT4X4 &world_B,
	phy_property &prop_A,
	phy_property &prop_B,
	const XMFLOAT3 &center_A,
	const XMFLOAT3 &center_B,
	const bool &is_touch,
	const bool &is_A_atk = false)
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
	if (is_A_atk) vel_A += XMLoadFloat3(&prop_A.vel_indirect);
	XMVECTOR vel_B = XMLoadFloat3(&prop_B.velocity);
	// empirical formula, vel_indirect is different from velocity
	XMVECTOR vel_A_nm = XMLoadFloat3(&prop_A.vel_indirect);
	XMVECTOR vel_B_nm = XMLoadFloat3(&prop_B.vel_indirect);
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
	// store result
	XMStoreFloat3(&prop_B.velocity, vel_B);
	XMStoreFloat4x4(&world_B, w_B);
	// if A is attacking, do not move
	if (is_A_atk) return;
	XMStoreFloat3(&prop_A.velocity, vel_A);
	XMStoreFloat4x4(&world_A, w_A);
	// check if instance stand on instance
	// c_A, c_B is updated, but center_A, center_B is not
	// currently using center_A, center_B
	static const float proportion_extens_y = 0.1f;
	if (center_A.y > center_B.y) {
		float diff = center_A.y-extents_y_A-(center_B.y+extents_y_B);
		if (abs(diff) < extents_y_B*proportion_extens_y) {
			prop_A.stand_on = static_cast<int>(ix_B);
			prop_A.is_on_ground = true;
		}
	}
	else {
		float diff = center_B.y-extents_y_B-(center_A.y+extents_y_A);
		if (abs(diff) < extents_y_A*proportion_extens_y) {
			prop_B.stand_on = static_cast<int>(ix_A);
			prop_B.is_on_ground = true;
		}
	}
	return;
}
//
}
#endif