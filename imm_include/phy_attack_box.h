////////////////
// phy_attack_box.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_ATTACK_BOX_H
#define PHY_ATTACK_BOX_H
#include "phy_prepare.h"
#include "stru_inst_adapter.h"
#include "phy_magic.h"
#include <map>
namespace imm
{
////////////////
// phy_attack_box
////////////////
////////////////
struct phy_attack_box
{
	phy_attack_box();
	size_t bone_ix;
	XMFLOAT4X4 to_bone;
	XMFLOAT3 extents;
};
//
phy_attack_box::phy_attack_box():
	bone_ix(0),
	extents(0.5f, 0.5f, 0.5f)
{
	XMStoreFloat4x4(&to_bone, XMMatrixIdentity());
}
////////////////
// phy_attack_model
////////////////
////////////////
struct phy_attack_model
{
	std::map<std::string, phy_attack_box> box;
};
////////////////
// phy_attack_arrange
////////////////
////////////////
template <typename T_app>
struct phy_attack_arrange
{
	phy_attack_arrange();
	~phy_attack_arrange();
	void remove_all();
	void init_load(T_app *app_in);
	void read_lua();
	void read_lua_bound_offset();
	void rebuild();
	void rebuild_bbox_from_instance();
	void rebuild_info_from_attachment();
	void update();
	void update_world();
	void update_collision();
	void set_active_box(
		const size_t &inst_ix,
		const std::vector<std::string> &box_name,
		const bool &active = true);
	void deactive_box(const size_t &inst_ix);
	std::vector<BoundingBox> bbox_l;
	std::vector<BoundingBox> bbox_w;
	std::vector<bool> is_active_box;
	std::vector<bool> is_active_att;
	// unarmed map:
	// # atk_model[model_name] = phy_attack_model
	// # map_box_active[instance_ix][box_name] = bbox_ix = is_active_box_ix
	// # map_box_owner[bbox_ix] = instance_ix
	std::map<std::string, phy_attack_model> atk_model;
	std::map<size_t, std::map<std::string, size_t>> map_box_active;
	std::map<size_t, size_t> map_box_owner;
	// bound correction:
	std::map<std::string, std::vector<float>> model_bound_offset;
	// weapon map:
	// # map_att_active[instance_ix][weapon_name] = is_active_att_ix
	// # map_att_ix[is_active_att_ix] = weapon_ix
	std::map<size_t, std::map<std::string, size_t>> map_att_active;
	std::map<size_t, size_t> map_att_ix;
	T_app *app;
};
//
template <typename T_app>
phy_attack_arrange<T_app>::phy_attack_arrange():
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_attack_arrange<T_app>::~phy_attack_arrange()
{
	;
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::remove_all()
{
	map_box_active.clear();
	map_box_owner.clear();
	bbox_l.clear();
	bbox_l.shrink_to_fit();
	bbox_w.clear();
	bbox_w.shrink_to_fit();
	is_active_box.clear();
	is_active_box.shrink_to_fit();
	map_att_active.clear();
	map_att_ix.clear();
	is_active_att.clear();
	is_active_att.shrink_to_fit();
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	read_lua();
	read_lua_bound_offset();
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::read_lua()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_attack_box", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		atk_model[vec2d[ix][0]].box[vec2d[ix][1]].bone_ix = stoi(vec2d[ix][2]);
		XMMATRIX offset = XMMatrixTranslation(
			stof(vec2d[ix][3]),
			stof(vec2d[ix][4]),
			stof(vec2d[ix][5]));
		XMMATRIX rot = rotation_xyz(vec2d[ix][6]).get_Matrix();
		XMMATRIX to_bone = XMMatrixMultiply(rot, offset);
		XMStoreFloat4x4(&atk_model[vec2d[ix][0]].box[vec2d[ix][1]].to_bone, to_bone);
		//
		if (vec2d[ix][7].size() > 4) {
			std::vector<float> para = csv_string_to_float(vec2d[ix][7], 3);
			atk_model[vec2d[ix][0]].box[vec2d[ix][1]].extents.x = para[0];
			atk_model[vec2d[ix][0]].box[vec2d[ix][1]].extents.y = para[1];
			atk_model[vec2d[ix][0]].box[vec2d[ix][1]].extents.z = para[2];
		}
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::read_lua_bound_offset()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_bound_offset", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		vec2d[ix][1] = vec2d[ix][1].substr(1);
		std::vector<float> offset = csv_string_to_percent(vec2d[ix][1]);
		model_bound_offset[vec2d[ix][0]] = offset;
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::rebuild()
{
	rebuild_bbox_from_instance();
	rebuild_info_from_attachment();
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::rebuild_bbox_from_instance()
{
	remove_all();
	for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
		if (app->m_Inst.m_Stat[ix].type != MODEL_SKINNED) continue;
		std::string *model_name = app->m_Inst.m_Stat[ix].get_ModelName();
		if (atk_model.count(*model_name)) {
			for (auto it = atk_model[*model_name].box.begin(); it != atk_model[*model_name].box.end(); ++it) {
				BoundingBox out;
				BoundingBox bbox(XMFLOAT3(0.0f, 0.0f, 0.0f), it->second.extents);
				XMMATRIX to_bone = XMLoadFloat4x4(&it->second.to_bone);
				bbox.Transform(out, to_bone);
				bbox_l.push_back(out);
				bbox_w.push_back(out);
				// build map
				map_box_active[ix][it->first] = bbox_l.size()-1;
				map_box_owner[bbox_l.size()-1] = ix;
			}
		}
	}
	is_active_box.resize(bbox_w.size(), false);
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::rebuild_info_from_attachment()
{
	app->m_Inst.m_Adapter.flush();
	std::vector<inst_attachment> (*att) = &app->m_Inst.m_Adapter.attach;
	for (size_t ix = 0; ix != (*att).size(); ++ix) {
		if ((*att)[ix].is_enable) {
			is_active_att.push_back(false);
			map_att_active[(*att)[ix].owner_ix][(*att)[ix].name] = is_active_att.size()-1;
			map_att_ix[is_active_att.size()-1] = ix;
		}
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::update()
{
	update_world();
	update_collision();
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::update_world()
{
	assert(!app->m_Cmd.is_waiting_for_something());
	for (auto it_map = map_box_active.begin(); it_map != map_box_active.end(); ++it_map) {
		XMMATRIX world = XMLoadFloat4x4(app->m_Inst.m_Stat[it_map->first].get_World());
		for (auto it_box = it_map->second.begin(); it_box != it_map->second.end(); ++it_box) {
			size_t bone_ix =
				app->m_Attack.atk_model[*app->m_Inst.m_Stat[it_map->first].get_ModelName()].box[it_box->first].bone_ix;
			XMMATRIX bone_trans = XMLoadFloat4x4(
				app->m_Inst.m_Stat[it_map->first].get_FinalTransform(bone_ix)
			);
			bbox_l[it_box->second].Transform(bbox_w[it_box->second], XMMatrixMultiply(bone_trans, world));
		}
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::update_collision()
{
	// unarmed
	for (size_t ix = 0; ix != is_active_box.size(); ++ix) {
		if (!is_active_box[ix]) continue;
		for (size_t ix_inst = 0; ix_inst != app->m_Inst.m_Stat.size(); ++ix_inst) {
			if (ix_inst == map_box_owner[ix]) continue;
			if (!app->m_Inst.m_Stat[ix_inst].is_invoke_physics()) continue;
			if (static_cast<int>(ix_inst) == app->m_Inst.m_PlaneLandIx) continue;
			bool is_touch = app->m_Inst.m_BoundW.intersects(ix_inst, bbox_w[ix]);
			if (is_touch) {
				app->m_Control.atk.cause_damage(
					map_box_owner[ix],
					ix_inst,
					bbox_w[ix].Center,
					SKILL_MELEE_UNARMED);
				//
			}
			phy_impulse_casual(
				app->m_Timer.delta_time(),
				0.0f, // following 4 parameters not use when is_A_atk
				0.0f,
				0,
				0,
				*(app->m_Inst.m_Stat[map_box_owner[ix]].get_World()),
				*(app->m_Inst.m_Stat[ix_inst].get_World()),
				app->m_Inst.m_Stat[map_box_owner[ix]].phy,
				app->m_Inst.m_Stat[ix_inst].phy,
				bbox_w[ix].Center,
				app->m_Inst.m_BoundW.center(ix_inst),
				is_touch,
				true);
			//
		}
	}
	// weapon
	std::vector<inst_attachment> (*att) = &app->m_Inst.m_Adapter.attach;
	for (size_t ix = 0; ix != is_active_att.size(); ++ix) {
		if (!is_active_att[ix]) continue;
		for (size_t ix_inst = 0; ix_inst != app->m_Inst.m_Stat.size(); ++ix_inst) {
			size_t owner_ix = (*att)[map_att_ix[ix]].owner_ix;
			size_t weapon_ix = (*att)[map_att_ix[ix]].ix;
			if (ix_inst == owner_ix) continue;
			if (!app->m_Inst.m_Stat[ix_inst].is_invoke_physics()) continue;
			if (static_cast<int>(ix_inst) == app->m_Inst.m_PlaneLandIx) continue;
			bool is_touch = app->m_Inst.m_BoundW.intersects(ix_inst, weapon_ix);
			if (is_touch) {
				app->m_Control.atk.cause_damage(
					owner_ix,
					ix_inst,
					app->m_Inst.m_BoundW.center(weapon_ix),
					SKILL_MELEE_UNARMED);
				//
			}
			phy_impulse_casual(
				app->m_Timer.delta_time(),
				0.0f, // following 4 parameters not use when is_A_atk
				0.0f,
				0,
				0,
				*(app->m_Inst.m_Stat[owner_ix].get_World()),
				*(app->m_Inst.m_Stat[ix_inst].get_World()),
				app->m_Inst.m_Stat[owner_ix].phy,
				app->m_Inst.m_Stat[ix_inst].phy,
				app->m_Inst.m_BoundW.center(ix),
				app->m_Inst.m_BoundW.center(ix_inst),
				is_touch,
				true);
			//
		}
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::set_active_box(
	const size_t &inst_ix,
	const std::vector<std::string> &box_name,
	const bool &active = true)
{
	assert(map_box_active.count(inst_ix) || map_att_active.count(inst_ix));
	// unarmed
	if (map_box_active.count(inst_ix)) {
		for (auto &name: box_name) {
			assert(map_box_active[inst_ix].count(name));
			is_active_box[map_box_active[inst_ix][name]] = active;
		}
	}
	// weapon
	if (map_att_active.count(inst_ix)) {
		for (auto &name: box_name) {
			assert(map_att_active[inst_ix].count(name));
			is_active_att[map_att_active[inst_ix][name]] = active;
		}
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::deactive_box(const size_t &inst_ix)
{
	assert(map_box_active.count(inst_ix) || map_att_active.count(inst_ix));
	// unarmed
	if (map_box_active.count(inst_ix)) {
		for (auto &box: map_box_active[inst_ix]) {
			is_active_box[box.second] = false;
		}
	}
	// weapon
	if (map_att_active.count(inst_ix)) {
		for (auto &box: map_att_active[inst_ix]) {
			is_active_att[box.second] = false;
		}
	}
}
//
}
#endif