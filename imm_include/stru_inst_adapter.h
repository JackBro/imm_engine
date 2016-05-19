////////////////
// stru_inst_adapter.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_INST_ADAPTER_H
#define STRU_INST_ADAPTER_H
//#include "stru_lua_help.h"

#include "stru_model_mgr.h"

namespace imm
{
////////////////
// inst_attachment
////////////////
////////////////
struct inst_attachment
{
	inst_attachment();
	std::string name;
	std::string owner_name;
	size_t ix;
	size_t owner_ix;
	XMFLOAT4X4 to_bone;
	size_t bone_ix;
	bool is_enable;
};
//
inst_attachment::inst_attachment():
	ix(0),
	owner_ix(0),
	bone_ix(0),
	is_enable(false)
{
	XMStoreFloat4x4(&to_bone, XMMatrixIdentity());
}
////////////////
// inst_adapter
////////////////
////////////////
template <typename T_app>
struct inst_adapter
{
	inst_adapter();
	void init_load(T_app *app_in);
	void read_lua();
	void flush();
	void update_world();
	T_app *app;
	std::vector<inst_attachment> attach;
};
//
template <typename T_app>
inst_adapter<T_app>::inst_adapter():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void inst_adapter<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	read_lua();
}
template <typename T_app>
void inst_adapter<T_app>::read_lua()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_weapon_map", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		attach.emplace_back();
		attach.back().name = vec2d[ix][0];
		attach.back().owner_name = vec2d[ix][1];
		attach.back().bone_ix = stoi(vec2d[ix][2]);
		XMMATRIX offset = XMMatrixTranslation(
			stof(vec2d[ix][3]),
			stof(vec2d[ix][4]),
			stof(vec2d[ix][5]));
		XMMATRIX rot = rotation_xyz(vec2d[ix][6]).get_Matrix();
		XMMATRIX to_bone = XMMatrixMultiply(rot, offset);
		XMStoreFloat4x4(&attach.back().to_bone, to_bone);
		//
	}
}
//
template <typename T_app>
void inst_adapter<T_app>::flush()
{
	for (auto &att: attach) {
		if (app->m_Inst.m_NameMap.count(att.name) && app->m_Inst.m_NameMap.count(att.owner_name)) {
			att.is_enable = true;
			att.ix = app->m_Inst.m_NameMap[att.name];
			att.owner_ix = app->m_Inst.m_NameMap[att.owner_name];
			app->m_Inst.m_Stat[att.ix].property |= MODEL_IS_ATTACH;
		}
		else {
			att.is_enable = false;
		}
	}
}
//
template <typename T_app>
void inst_adapter<T_app>::update_world()
{
	for (auto &att: attach) {
		if (!att.is_enable) continue;
		XMMATRIX world = XMLoadFloat4x4(app->m_Inst.m_Stat[att.owner_ix].get_World());
		XMMATRIX bone_trans = XMLoadFloat4x4(
			app->m_Inst.m_Stat[att.owner_ix].get_FinalTransform(att.bone_ix)
		);
		XMMATRIX to_bone = XMLoadFloat4x4(&att.to_bone);
		world = XMMatrixMultiply(bone_trans, world);
		world = XMMatrixMultiply(to_bone, world);
		app->m_Inst.m_Stat[att.ix].set_World(world);
	}
}
//
}
#endif