////////////////
// phy_attack_box.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_ATTACK_BOX_H
#define PHY_ATTACK_BOX_H
#include "phy_prepare.h"
#include "stru_lua_help.h"
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
	void rebuild_bbox_from_instance();
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
	std::vector<bool> is_active;
	std::map<std::string, phy_attack_model> model;
	// demonstration: map[instance_ix][box_name] = bbox_ix
	std::map<size_t, std::map<std::string, size_t>> map;
	// demonstration: map[bbox_ix] = instance_ix
	std::map<size_t, size_t> map_inst;
	std::map<std::string, std::vector<float>> model_bound_offset;
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
	map.clear();
	map_inst.clear();
	bbox_l.clear();
	bbox_w.clear();
	is_active.clear();
	bbox_l.shrink_to_fit();
	bbox_w.shrink_to_fit();
	is_active.shrink_to_fit();
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
		model[vec2d[ix][0]].box[vec2d[ix][1]].bone_ix = stoi(vec2d[ix][2]);
		XMMATRIX offset = XMMatrixTranslation(
			stof(vec2d[ix][3]),
			stof(vec2d[ix][4]),
			stof(vec2d[ix][5]));
		XMMATRIX rot = rotation_xyz(vec2d[ix][6]).get_Matrix();
		XMMATRIX to_bone = XMMatrixMultiply(rot, offset);
		XMStoreFloat4x4(&model[vec2d[ix][0]].box[vec2d[ix][1]].to_bone, to_bone);
		//
		if (vec2d[ix][7].size() > 4) {
			std::vector<float> para = csv_string_to_float(vec2d[ix][7], 3);
			model[vec2d[ix][0]].box[vec2d[ix][1]].extents.x = para[0];
			model[vec2d[ix][0]].box[vec2d[ix][1]].extents.y = para[1];
			model[vec2d[ix][0]].box[vec2d[ix][1]].extents.z = para[2];
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
void phy_attack_arrange<T_app>::rebuild_bbox_from_instance()
{
	remove_all();
	for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
		if (app->m_Inst.m_Stat[ix].type != skinned) continue;
		std::string *model_name = app->m_Inst.m_Stat[ix].get_ModelName();
		if (model.count(*model_name)) {
			for (auto it = model[*model_name].box.begin(); it != model[*model_name].box.end(); ++it) {
				BoundingBox out;
				BoundingBox bbox(XMFLOAT3(0.0f, 0.0f, 0.0f), it->second.extents);
				XMMATRIX to_bone = XMLoadFloat4x4(&it->second.to_bone);
				bbox.Transform(out, to_bone);
				bbox_l.push_back(out);
				bbox_w.push_back(out);
				// build map
				map[ix][it->first] = bbox_l.size()-1;
				map_inst[bbox_l.size()-1] = ix;
			}
		}
	}
	is_active.resize(bbox_w.size(), false);
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
	for (auto it_map = map.begin(); it_map != map.end(); ++it_map) {
		XMMATRIX world = XMLoadFloat4x4(app->m_Inst.m_Stat[it_map->first].get_World());
		for (auto it_box = it_map->second.begin(); it_box != it_map->second.end(); ++it_box) {
			size_t bone_ix =
				app->m_Attack.model[*app->m_Inst.m_Stat[it_map->first].get_ModelName()].box[it_box->first].bone_ix;
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
	for (size_t ix = 0; ix != is_active.size(); ++ix) {
		if (!is_active[ix]) continue;
		for (size_t ix_inst = 0; ix_inst != app->m_Inst.m_Stat.size(); ++ix_inst) {
			phy_impulse_casual(
				app->m_Timer.delta_time(),
				*(app->m_Inst.m_Stat[map_inst[ix]].get_World()),
				*(app->m_Inst.m_Stat[ix_inst].get_World()),
				app->m_Inst.m_Stat[map_inst[ix]].phy,
				app->m_Inst.m_Stat[ix_inst].phy,
				bbox_w[ix].Center,
				app->m_Inst.m_BoundW.center(ix_inst),
				app->m_Inst.m_BoundW.intersects(ix_inst, bbox_w[ix]));
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
	assert(map.count(inst_ix));
	for (auto &name: box_name) {
		assert(map[inst_ix].count(name));
		is_active[map[inst_ix][name]] = active;
	}
}
//
template <typename T_app>
void phy_attack_arrange<T_app>::deactive_box(const size_t &inst_ix)
{
	assert(map.count(inst_ix));
	for (auto &box: map[inst_ix]) {
		is_active[box.second] = false;
	}
}
//
}
#endif