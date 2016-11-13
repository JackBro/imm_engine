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
// phy_position
////////////////
////////////////
template <typename T_app>
struct phy_position
{
	phy_position();
	void init(T_app *app_in);
	void update(
		const float &dt,
		XMFLOAT4X4 &world,
		phy_property &prop,
		phy_property &prop_land,
		const XMFLOAT3 &center,
		const float &extents_y,
		const float &land_y);
	void impulse_casual(
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
		const bool &is_touch);
	void attack_impulse(
		phy_property &prop_B,
		const XMFLOAT3 &center_Hit,
		const XMFLOAT3 &center_B,
		const XMFLOAT3 &center_A,
		const bool &is_touch,
		const float &impulse_scale,
		const size_t &ix_dmg);
	T_app *app;
};
//
template <typename T_app>
phy_position<T_app>::phy_position():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void phy_position<T_app>::init(T_app *app_in)
{
	app = app_in;
}
////////////////
// phy_position::update
////////////////
////////////////
template <typename T_app>
void phy_position<T_app>::update(
	const float &dt,
	XMFLOAT4X4 &world,
	phy_property &prop,
	phy_property &prop_land,
	const XMFLOAT3 &center,
	const float &extents_y,
	const float &land_y)
{
	if (prop.is_abnormal) return;
	bool is_fps60_dt = false;
	prop.dt += dt;
	// Game FPS must >= 60
	if (prop.dt > FPS60_1DIV) {
		prop.dt = 0.0f;
		is_fps60_dt = true;
	}
	if (prop.absolute_alt > 0.0f) prop.absolute_alt -= dt;
	if (is_fps60_dt) {
		float decay = 0.5f;
		if (prop.absolute_alt > 0.0f) decay = 1.0f;
		prop.vel_absolute.x *= decay;
		prop.vel_absolute.y *= decay;
		prop.vel_absolute.z *= decay;
	}
	if (!prop.is_on_land) {
		prop.velocity.x += prop.acceleration.x*dt;
		prop.velocity.y += (PHY_GRAVITY+prop.acceleration.y)*dt;
		prop.velocity.z += prop.acceleration.z*dt;
		world._41 += (prop.velocity.x + prop.vel_indirect.x + prop.vel_absolute.x)*dt;
		world._42 += (prop.velocity.y + prop.vel_indirect.y)*dt;
		world._43 += (prop.velocity.z + prop.vel_indirect.z + prop.vel_absolute.z)*dt;
	}
	else {
		float bounce = prop.bounce*prop_land.bounce;
		float friction_rev = prop.friction_rev*prop_land.friction_rev;
		prop.acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (prop.velocity.y < 0.0f) prop.velocity.y = -prop.velocity.y*bounce*FPS60*dt;
		if (is_fps60_dt) {
			prop.velocity.x *= friction_rev;
			prop.velocity.z *= friction_rev;
		}
		// use center compare stand
		float offset_y = world._42-center.y;
		float center_y = center.y;
		// phy_impulse_casual() produce fake velocity of PHY_INTERA_FIXED
		// but not adjust PHY_INTERA_FIXED's full stand on land
		if (*prop.intera_tp & PHY_INTERA_FIXED) return;
		// stand_adjust keep object full stand on land
		// guarantee object.intersects(land) return true, exclude if they have a little gap
		float stand_adjust = -0.01f;
		float stand = extents_y+land_y+stand_adjust;
		center_y += (prop.velocity.y + prop.vel_indirect.y)*dt;
		world._41 += (prop.velocity.x + prop.vel_indirect.x + prop.vel_absolute.x)*dt;
		world._43 += (prop.velocity.z + prop.vel_indirect.z + prop.vel_absolute.z)*dt;
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
// phy_position::impulse_casual
// method reference 1:
// (http://gamedevelopment.tutsplus.com/tutorials/
// how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331)
// method reference 2:
// \Microsoft DirectX SDK (June 2010)\Samples\C++\Direct3D\ConfigSystem\main.cpp
// it is originally used with two spheres, but there is used with two AABB or others, inaccuracy solution
////////////////
////////////////
template <typename T_app>
void phy_position<T_app>::impulse_casual(
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
	const bool &is_touch)
{
	if (!is_touch) return;
	if ((*prop_A.intera_tp & PHY_INTERA_FIXED) && (*prop_B.intera_tp & PHY_INTERA_FIXED)) return;
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	XMMATRIX w_A = XMLoadFloat4x4(&world_A);
	XMMATRIX w_B = XMLoadFloat4x4(&world_B);
	// AtoB is the collision normal vector.
	XMVECTOR AtoB = XMVectorSubtract(c_B, c_A);
	float relative_size = 1.1f;
	if (prop_A.p_aabb3 && prop_B.p_aabb3) {
	relative_size = abs(prop_A.avg_extent-prop_B.avg_extent)/(std::min)(prop_A.avg_extent, prop_B.avg_extent);
	if (relative_size > 1.0f) {
		float big_x = (std::max)(prop_A.p_aabb3->x, prop_B.p_aabb3->x);
		if (big_x < abs(XMVectorGetX(AtoB)) ) {
			AtoB = XMVectorSetZ(AtoB, 0.0f);
			AtoB = XMVectorSetY(AtoB, 0.0f);
		}
		float big_z = (std::max)(prop_A.p_aabb3->z, prop_B.p_aabb3->z);
		if (big_z < abs(XMVectorGetZ(AtoB)) ) {
			AtoB = XMVectorSetX(AtoB, 0.0f);
			AtoB = XMVectorSetY(AtoB, 0.0f);
		}
		float big_y = (std::max)(prop_A.p_aabb3->y, prop_B.p_aabb3->y);
		if (big_y < abs(XMVectorGetY(AtoB)) ) {
			AtoB = XMVectorSetX(AtoB, 0.0f);
			AtoB = XMVectorSetZ(AtoB, 0.0f);
		}
	}}
	AtoB = XMVector3Normalize(AtoB);
	//
	XMVECTOR vel_A = XMLoadFloat3(&prop_A.velocity);
	XMVECTOR vel_B = XMLoadFloat3(&prop_B.velocity);
	// empirical formula, vel_indirect is different from velocity
	XMVECTOR vel_A_indir = XMLoadFloat3(&prop_A.vel_indirect);
	XMVECTOR vel_B_indir = XMLoadFloat3(&prop_B.vel_indirect);
	XMVECTOR vel_A_all = XMVectorAdd(vel_A, vel_A_indir);
	XMVECTOR vel_B_all = XMVectorAdd(vel_B, vel_B_indir);
	vel_A_all = XMVectorAdd(vel_A_all, XMLoadFloat3(&prop_A.vel_absolute));
	vel_B_all = XMVectorAdd(vel_B_all, XMLoadFloat3(&prop_B.vel_absolute));
	//
	if (prop_B.bring_ix != prop_A.ix) {
		XMVECTOR vel_B_bring = XMLoadFloat3(&prop_B.vel_bring);
		vel_B_all = XMVectorAdd(vel_B_all, vel_B_bring);
	}
	if (prop_A.bring_ix != prop_B.ix) {
		XMVECTOR vel_A_bring = XMLoadFloat3(&prop_A.vel_bring);
		vel_A_all = XMVectorAdd(vel_A_all, vel_A_bring);
	}
	// bounce
	float bounce = prop_A.bounce*prop_B.bounce;
	XMVECTOR vel_AB_all = XMVectorSubtract(vel_A_all, vel_B_all);
	// ignore mass
	XMVECTOR injected_impulse = XMVectorScale(
		AtoB, XMVectorGetX(XMVector3Dot(XMVectorScale(vel_AB_all, -1.0f*bounce), AtoB)));
	injected_impulse = XMVectorScale(injected_impulse, FPS60*dt);
	vel_A = XMVectorAdd(vel_A, injected_impulse);
	vel_B = XMVectorSubtract(vel_B, injected_impulse);
	// translate force as estimated velocity value
	if (*prop_A.intera_tp & PHY_INTERA_STATIC) {
		XMStoreFloat3(&prop_A.vel_bring, XMVectorScale(vel_B_all, 1.5f));
		prop_A.bring_ix = prop_B.ix;
	}
	if (*prop_B.intera_tp & PHY_INTERA_STATIC) {
		XMStoreFloat3(&prop_B.vel_bring, XMVectorScale(vel_A_all, 1.5f));
		prop_B.bring_ix = prop_A.ix;
	}
	// penetration depth estimate as vel_absolute
	// fix the positions so that the two objects are apart, not accurate
	XMVECTOR vel_absolute_A = XMLoadFloat3(&prop_A.vel_absolute);
	XMVECTOR vel_absolute_B = XMLoadFloat3(&prop_B.vel_absolute);
	// small object need more penetration
	if (prop_A.p_aabb3 && prop_B.p_aabb3 && relative_size < 1.0f) {
		AtoB = XMVectorSetY(AtoB, XMVectorGetY(AtoB)*0.3f);
		AtoB = XMVectorScale(AtoB, 30.0f);
	}
	// scene boundary
	else if (*prop_B.intera_tp == PHY_INTERA_FIXED_INVISILBE) {
		XMVECTOR offset_A = XMVectorSubtract(w_A.r[3], c_A);
		float penetration = XMVectorGetX(XMVector3Length(XMVectorSubtract(vel_A_all, vel_B_all)))*dt;
		XMVECTOR to_scene = XMVectorZero();
		if (abs(XMVectorGetX(c_B)) > 1.0f) {
			if (XMVectorGetX(c_B) > 0.0f) to_scene = XMVectorSetX(to_scene, -1.0f);
			else to_scene = XMVectorSetX(to_scene, 1.0f);
		}
		else {
			if (XMVectorGetZ(c_B) > 0.0f) to_scene = XMVectorSetZ(to_scene, -1.0f);
			else to_scene = XMVectorSetZ(to_scene, 1.0f);
		}
		c_A = XMVectorAdd(c_A, XMVectorScale(to_scene, penetration));
		w_A.r[3] = XMVectorAdd(c_A, offset_A);
		w_A.r[3] = XMVectorSetW(w_A.r[3], 1.0f);
	}
	else if (*prop_A.intera_tp == PHY_INTERA_FIXED_INVISILBE) {
		XMVECTOR offset_B = XMVectorSubtract(w_B.r[3], c_B);
		float penetration = XMVectorGetX(XMVector3Length(XMVectorSubtract(vel_A_all, vel_B_all)))*dt*1.0f;
		XMVECTOR to_scene = XMVectorZero();
		if (abs(XMVectorGetX(c_A)) > 1.0f) {
			if (XMVectorGetX(c_A) > 0.0f) to_scene = XMVectorSetX(to_scene, -1.0f);
			else to_scene = XMVectorSetX(to_scene, 1.0f);
		}
		else {
			if (XMVectorGetZ(c_A) > 0.0f) to_scene = XMVectorSetZ(to_scene, -1.0f);
			else to_scene = XMVectorSetZ(to_scene, 1.0f);
		}
		c_B = XMVectorAdd(c_B, XMVectorScale(to_scene, penetration));
		w_B.r[3] = XMVectorSetW(w_B.r[3], 1.0f);
		w_B.r[3] = XMVectorAdd(c_B, offset_B);
		AtoB = XMVectorScale(to_scene, 20.0f);
	}
	// big object
	// !(relative_size < 1.0f)
	else {
		AtoB = XMVectorScale(AtoB, 20.0f);
	}
	// store result
	if (!(*prop_B.intera_tp & PHY_INTERA_FIXED)) {
		vel_absolute_B = XMVectorAdd(vel_absolute_B, AtoB);
		XMStoreFloat3(&prop_B.velocity, vel_B);
		XMStoreFloat3(&prop_B.vel_absolute, vel_absolute_B);
		if (*prop_A.intera_tp == PHY_INTERA_FIXED_INVISILBE) XMStoreFloat4x4(&world_B, w_B);
	}
	if (!(*prop_A.intera_tp & PHY_INTERA_FIXED)) {
		vel_absolute_A = XMVectorSubtract(vel_absolute_A, AtoB);
		XMStoreFloat3(&prop_A.velocity, vel_A);
		XMStoreFloat3(&prop_A.vel_absolute, vel_absolute_A);
		if (*prop_B.intera_tp == PHY_INTERA_FIXED_INVISILBE) XMStoreFloat4x4(&world_A, w_A);
	}
	// check if instance stand on instance
	// c_A, c_B is updated, but center_A, center_B is not
	// currently using center_A, center_B
	static const float proportion_extens_y = 0.1f;
	if (center_A.y > center_B.y) {
		float diff = center_A.y-extents_y_A-(center_B.y+extents_y_B);
		if (abs(diff) < extents_y_B*proportion_extens_y) {
			prop_A.stand_on = static_cast<int>(ix_B);
			prop_A.is_on_land = true;
		}
	}
	else {
		float diff = center_B.y-extents_y_B-(center_A.y+extents_y_A);
		if (abs(diff) < extents_y_A*proportion_extens_y) {
			prop_B.stand_on = static_cast<int>(ix_A);
			prop_B.is_on_land = true;
		}
	}
	return;
}
////////////////
// phy_position::attack_impulse
// A is attacker
////////////////
////////////////
template <typename T_app>
void phy_position<T_app>::attack_impulse(
	phy_property &prop_B,
	const XMFLOAT3 &center_Hit,
	const XMFLOAT3 &center_B,
	const XMFLOAT3 &center_A,
	const bool &is_touch,
	const float &impulse_scale,
	const size_t &ix_dmg)
{
	if (!is_touch) return;
	if (*prop_B.intera_tp & PHY_INTERA_FIXED) return;
	if (app->m_Inst.m_Troll[ix_dmg].current_state == pose_FallDown::instance()) return;
	XMVECTOR c_Hit = XMLoadFloat3(&center_Hit);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	//
	XMVECTOR Hit_to_B = XMVectorZero();
	if (XMVectorGetX(XMVector3Length(Hit_to_B)) > 8.0f) {
		Hit_to_B = XMVectorSubtract(c_B, c_Hit);
	}
	else {
		Hit_to_B = XMVectorSubtract(c_B, c_A);
		Hit_to_B = XMVectorSetY(Hit_to_B, 0.0f);
	}
	Hit_to_B = XMVector3Normalize(Hit_to_B);
	XMVECTOR vel_absolute = XMLoadFloat3(&prop_B.vel_absolute);
	// think aimlessly
	float impulse = 3.0f;
	if (impulse_scale < 3.0f) impulse = impulse_scale;
	Hit_to_B = XMVectorScale(Hit_to_B, 20.0f * impulse);
	vel_absolute = XMVectorAdd(vel_absolute, Hit_to_B);
	XMStoreFloat3(&prop_B.vel_absolute, vel_absolute);
	if (impulse_scale > ATK_IMPULSE_PHASE) {
		float f_time = impulse_scale - 3.0f;
		prop_B.absolute_alt = f_time*FRAME_RATE_1DIV;
	}
	return;
}
//
}
#endif