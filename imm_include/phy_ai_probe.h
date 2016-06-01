////////////////
// phy_ai_probe.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_AI_PROBE_H
#define PHY_AI_PROBE_H
#include "phy_obstacle_avoid.h"
namespace imm
{
////////////////
// ai_bound
////////////////
////////////////
struct ai_bound
{
	ai_bound();
	void init(
		CXMMATRIX &rot_inv,
		const float &radius_inst_in,
		const float &radius_add_in,
		const float &extents_z_add_in);
	void transform_sph(CXMMATRIX &world);
	void transform_obb(CXMMATRIX &world);
	BoundingSphere SphL;
	BoundingSphere SphW;
	BoundingOrientedBox ObbL;
	BoundingOrientedBox ObbW;
	bool is_active;
	float radius_inst;
	float radius_add;
	float extents_z_add;
	float dt_oph;
	float dt_obb;
	float obj_y_min;
};
//
ai_bound::ai_bound():
	is_active(false),
	radius_inst(0.0f),
	radius_add(1.0f),
	extents_z_add(2.0f),
	dt_oph(0.0f),
	dt_obb(0.0f),
	obj_y_min(-50.0f)
{
	;
}
//
void ai_bound::init(
	CXMMATRIX &rot_inv,
	const float &radius_inst_in,
	const float &radius_add_in,
	const float &extents_z_add_in)
{
	radius_inst = radius_inst_in;
	radius_add = radius_add_in;
	SphL.Radius = radius_inst+radius_add;
	SphL.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	SphW = SphL;
	//
	ObbL.Center = XMFLOAT3(0.0f, 0.0f, -extents_z_add_in-radius_inst);
	ObbL.Extents = XMFLOAT3(radius_inst, radius_inst, radius_inst+extents_z_add_in);
	ObbL.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	// Blender's mesh is right-hander, after export to left-handed,
	// mesh will face toward ground
	ObbL.Transform(ObbW, rot_inv);
	ObbL = ObbW;
	dt_oph = math::calc_randf(-AI_DELTA_TIME_PHY_FAST, AI_DELTA_TIME_PHY_FAST);
	dt_obb = math::calc_randf(-AI_DELTA_TIME_PHY_SLOW, AI_DELTA_TIME_PHY_SLOW);
}
//
void ai_bound::transform_sph(CXMMATRIX &world)
{
	SphL.Transform(SphW, world);
}
//
void ai_bound::transform_obb(CXMMATRIX &world)
{
	ObbL.Transform(ObbW, world);
}
////////////////
// ai_keep
////////////////
////////////////
struct ai_keep
{
	float obj_y;
	float scale_inv;
	XMFLOAT4X4 rot_inv;
};
////////////////
// ai_probe
////////////////
////////////////
template <typename T_app>
struct ai_probe
{
	ai_probe();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void update(const float &dt);
	void touch_test(const size_t &ix_probe, const size_t &ix_object);
	void obstacle_avoid(
		const size_t &ix_probe,
		const size_t &ix_object,
		const XMMATRIX &to_A_local,
		const XMMATRIX &to_A_local_inv,
		const float &destination_y);
	void set_active(const size_t &ix);
	std::map<size_t, ai_bound> geometry;
	std::map<size_t, ai_keep> keep;
	float max_local_y_far;
	T_app *app;
};
//
template <typename T_app>
ai_probe<T_app>::ai_probe():
	app(nullptr),
	max_local_y_far(-50.0f)
{
	;
}
//
template <typename T_app>
void ai_probe<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ai_probe<T_app>::reset()
{
	geometry.clear();
	keep.clear();
}
//
template <typename T_app>
void ai_probe<T_app>::rebuild()
{
	reset();
	for (auto &stat: app->m_Inst.m_Stat) {
		if (stat.property & MODEL_IS_CONTROLLABLE) {
			size_t ix = &stat - &app->m_Inst.m_Stat[0];
			XMMATRIX rot_front = XMLoadFloat4x4(app->m_Inst.m_Stat[ix].get_RotFront());
			XMVECTOR out_scale, out_trans, out_rot;
			assert(XMMatrixDecompose(&out_scale, &out_rot, &out_trans, rot_front));
			out_rot = XMQuaternionInverse(out_rot);
			XMMATRIX rot_inv = XMMatrixRotationQuaternion(out_rot);
			//
			float radius_inst = (app->m_Inst.m_BoundL.extents_x(ix)+app->m_Inst.m_BoundL.extents_z(ix))*0.5f;
			geometry[ix].init(rot_inv, radius_inst, 1.0f, 5.0f);
			//
			XMStoreFloat4x4(&keep[ix].rot_inv, rot_inv);
			keep[ix].scale_inv = 1.0f/XMVectorGetX(out_scale);
			keep[ix].obj_y = max_local_y_far;
		}
	}
}
//
//
template <typename T_app>
void ai_probe<T_app>::update(const float &dt)
{
	for (auto &geo: geometry) {
		if (!geo.second.is_active) continue;
		// separate update
		geo.second.dt_oph += dt;
		geo.second.dt_obb += dt;
		if (geo.second.dt_oph > AI_DELTA_TIME_PHY_FAST) {
			geo.second.dt_oph -= AI_DELTA_TIME_PHY_FAST;
			geo.second.transform_sph(XMLoadFloat4x4(app->m_Inst.m_Stat[geo.first].get_World()));
			for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
				touch_test(geo.first, ix);
			}
		}
		if (geo.second.dt_obb > AI_DELTA_TIME_PHY_SLOW) {
			geo.second.dt_obb -= AI_DELTA_TIME_PHY_SLOW;
			geo.second.transform_obb(XMLoadFloat4x4(app->m_Inst.m_Stat[geo.first].get_World()));
			app->m_Control.map_stop[geo.first].clear_avoid_pos();
			//
			XMMATRIX to_A_local;
			XMMATRIX to_A_local_inv;
			float destination_y = max_local_y_far;
			geometry[geo.first].obj_y_min = max_local_y_far;
			XMVECTOR destination = XMLoadFloat3(&app->m_Inst.m_Steering[geo.first].desired_pos);
			phy_destination_y(
				*app->m_Inst.m_Stat[geo.first].get_World(),
				keep[geo.first].rot_inv,
				to_A_local,
				to_A_local_inv,
				app->m_Inst.m_BoundW.center(geo.first),
				destination,
				destination_y
			);
			for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
				obstacle_avoid(geo.first, ix, to_A_local, to_A_local_inv, destination_y);
			}
		}
	}
}
//
template <typename T_app>
void ai_probe<T_app>::touch_test(const size_t &ix_probe, const size_t &ix_object)
{
	if (app->m_Inst.m_Stat[ix_object].property & MODEL_IS_LAND) return;
	if (!app->m_Inst.m_Stat[ix_object].is_invoke_physics()) return;
	bool is_touch = app->m_Inst.m_BoundW.intersects(ix_object, geometry[ix_probe].SphW);
	app->m_Inst.m_Steering[ix_probe].touch[ix_object] = is_touch;
}
//
template <typename T_app>
void ai_probe<T_app>::obstacle_avoid(
	const size_t &ix_probe,
	const size_t &ix_object,
	const XMMATRIX &to_A_local,
	const XMMATRIX &to_A_local_inv,
	const float &destination_y)
{
	// phy_obstacle_avoid
	if (app->m_Inst.m_Stat[ix_object].property & MODEL_IS_LAND) return;
	if (!app->m_Inst.m_Stat[ix_object].is_invoke_physics()) return;	
	if (app->m_Inst.m_Stat[ix_object].property & MODEL_IS_CONTROLLABLE) return;
	if (app->m_Control.map_stop[ix_probe].is_stop) return;
	float radius_obj = (app->m_Inst.m_BoundL.extents_x(ix_object)+app->m_Inst.m_BoundL.extents_z(ix_object))*0.5f;
	if (radius_obj < geometry[ix_probe].radius_inst*0.5f) return;
	bool is_touch = app->m_Inst.m_BoundW.intersects(ix_object, geometry[ix_probe].ObbW);
	if (!is_touch) return;
	bool is_erase = false;
	float obj_y;
	phy_obstacle_avoid(
		to_A_local,
		to_A_local_inv,
		app->m_Inst.m_BoundW.center(ix_probe),
		app->m_Inst.m_BoundW.center(ix_object),
		app->m_Control.map_stop[ix_probe].avoid_pos[ix_object],
		keep[ix_probe].scale_inv,
		radius_obj,
		destination_y,
		obj_y,
		is_erase
	);
	//
	if (is_erase) {
		app->m_Control.map_stop[ix_probe].avoid_pos.erase(ix_object);
		return;
	}
	if (obj_y > geometry[ix_probe].obj_y_min) {
		geometry[ix_probe].obj_y_min = obj_y;
	}
	else {
		app->m_Control.map_stop[ix_probe].avoid_pos.erase(ix_object);
		return;
	}
}
//
template <typename T_app>
void ai_probe<T_app>::set_active(const size_t &ix)
{
	geometry[ix].is_active = true;
}
//
}
#endif