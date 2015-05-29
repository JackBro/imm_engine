////////////////
// stru_model_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_MODEL_MGR_H
#define STRU_MODEL_MGR_H
#include "phy_position_logic.h"
#include "stru_scene_help.h"
#include "mesh_basic_model.h"
#include <thread>
namespace imm
{
////////////////
// instance_type
////////////////
////////////////
enum instance_type
{
	basic,
	skinned,
	simple_b32,
	simple_pntt
};
////////////////
// instance_stat
////////////////
////////////////
struct instance_stat
{
	instance_stat();
	const void *p;
	phy_property phy;
	instance_type type;
	//
	XMFLOAT4X4 *get_World();
	XMFLOAT4X4 *get_RotFront();
	void set_World(const XMFLOAT4X4 world);
	void set_IsAppear(const bool is_appear);
	void set_ClipName(const std::string &clip_name);
	void check_set_ClipName(const std::string &clip_name);
};
//
instance_stat::instance_stat():
	p(nullptr),
	phy(),
	type(basic)
{
	;	
}
//
XMFLOAT4X4 *instance_stat::get_World()
{
	switch(type) {
		case basic: return &(((basic_model_instance*)p)->world);
		case skinned: return &(((skinned_model_instance*)p)->world);
		case simple_b32: return &(((simple_model_instance<basic32>*)p)->world);
		case simple_pntt: return &(((simple_model_instance<pos_normal_tex_tan>*)p)->world);
	}
	assert(false);
	// bad
	return &(((basic_model_instance*)p)->world);
}
//
XMFLOAT4X4 *instance_stat::get_RotFront()
{
	switch(type) {
		case basic: return &(((basic_model_instance*)p)->rot_front);
		case skinned: return &(((skinned_model_instance*)p)->rot_front);
		case simple_b32: return &(((simple_model_instance<basic32>*)p)->rot_front);
		case simple_pntt: return &(((simple_model_instance<pos_normal_tex_tan>*)p)->rot_front);
	}
	assert(false);
	// bad
	return &(((basic_model_instance*)p)->rot_front);
}
//
void instance_stat::set_World(const XMFLOAT4X4 world)
{
	switch(type) {
		case basic: ((basic_model_instance*)p)->world = world; return;
		case skinned: ((skinned_model_instance*)p)->world = world; return;
		case simple_b32: ((simple_model_instance<basic32>*)p)->world = world; return;
		case simple_pntt: ((simple_model_instance<pos_normal_tex_tan>*)p)->world = world; return;
	}
	assert(false);
}
//
void instance_stat::set_IsAppear(const bool is_appear)
{
	switch(type) {
		case basic: ((basic_model_instance*)p)->is_appear = is_appear; return;
		case skinned: ((skinned_model_instance*)p)->is_appear = is_appear; return;
		case simple_b32: ((simple_model_instance<basic32>*)p)->is_appear = is_appear; return;
		case simple_pntt: ((simple_model_instance<pos_normal_tex_tan>*)p)->is_appear = is_appear; return;
	}
	assert(false);
}
//
void instance_stat::set_ClipName(const std::string &clip_name)
{
	if (type != skinned) return;
	((skinned_model_instance*)p)->set_ClipName(clip_name);
}
//
void instance_stat::check_set_ClipName(const std::string &clip_name)
{
	if (type != skinned) return;
	((skinned_model_instance*)p)->check_set_ClipName(clip_name);
}
////////////////
// model_mgr
////////////////
////////////////
struct model_mgr
{
	model_mgr();
	void init(ID3D11Device *device);
	bool load(ID3D11Device *device, const std::string& scene_ix);
	void b32_init(ID3D11Device *device, lua_reader &l_reader);
	void pntt_init(ID3D11Device *device, lua_reader &l_reader);
	void skinned_init(ID3D11Device *device, lua_reader &l_reader);
	void basic_init(ID3D11Device *device, lua_reader &l_reader);
	void remove_instance();
	texture_mgr m_TexMgr;
	bool is_initialized;
	std::map<std::string, simple_model<basic32>> m_B32;
	std::map<std::string, simple_model<pos_normal_tex_tan>> m_PNTT;
	std::map<std::string, skinned_model> m_Skinned;
	std::map<std::string, basic_model> m_Basic;
	std::vector<simple_model_instance<basic32>> m_InstB32;
	std::vector<simple_model_instance<pos_normal_tex_tan>> m_InstPNTT;	
	std::vector<skinned_model_instance> m_InstSkinned;
	std::vector<basic_model_instance> m_InstBasicAlpha;
	std::vector<basic_model_instance> m_InstBasic;
	std::vector<std::string> m_NameB32;
	std::vector<std::string> m_NamePNTT;
	std::vector<std::string> m_NameSkinned;
	std::vector<std::string> m_NameBasicAlpha;
	std::vector<std::string> m_NameBasic;
};
model_mgr::model_mgr():
	m_TexMgr(),
	is_initialized(false)
{
	;
}
//
void model_mgr::init(ID3D11Device *device)
{
	if (!is_initialized) {
		m_TexMgr.init(device);
		is_initialized = true;
	}
}
//
bool model_mgr::load(ID3D11Device *device, const std::string& scene_ix)
{
	init(device);
	std::string describe = GLOBAL["path_lua"]+"scene"+scene_ix+"\\describe_instance.lua";
	if (!data_is_file_exist(describe)) return false;
	remove_instance();
	lua_reader l_reader;
	l_reader.loadfile(describe);
	std::thread(&model_mgr::b32_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::pntt_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::skinned_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::basic_init, this, device, std::ref(l_reader)).join();
	return true;
}
//
void model_mgr::b32_init(ID3D11Device *device, lua_reader &l_reader)
{
	// simple_b32 not implement
	DUMMY(device);
	DUMMY(l_reader);
}
//
void model_mgr::pntt_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::map<std::string, rotation_xyz> model_rot_front;
	std::map<std::string, UINT> model_index;
	std::map<std::string, bool> model_is_tex;
	std::map<std::string, std::string> model_tex_trans;
	// build model
	l_reader.vec2d_str_from_global("csv_model_geometry", vec2d_model);
	std::vector<geometry::mesh_data> geo_mesh;
	geometry geo_gen;
	m_PNTT["default"];
	std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
	for (size_t ix = 1; ix < vec2d_model.size(); ++ix) {
		std::string model_name = vec2d_model[ix][0];
		model_index[model_name] = static_cast<UINT>(ix-1);
		geo_mesh.push_back(geometry::mesh_data());
		std::vector<float> para;
		if (vec2d_model[ix][1] == "box") {
			para = csv_string_to_float(vec2d_model[ix][2], 3);
			geo_gen.create_box(para[0], para[1], para[2], geo_mesh.back());
		}
		if (vec2d_model[ix][1] == "grid") {
			para = csv_string_to_float(vec2d_model[ix][2], 4);
			geo_gen.create_grid(para[0], para[1],
				static_cast<int>(para[2]), static_cast<int>(para[3]), geo_mesh.back());
		}
		if (vec2d_model[ix][1] == "sphere") {
			para = csv_string_to_float(vec2d_model[ix][2], 3);
			geo_gen.create_sphere(para[0],
				static_cast<int>(para[1]), static_cast<int>(para[2]), geo_mesh.back());
		}
		if (vec2d_model[ix][1] == "cylinder") {
			para = csv_string_to_float(vec2d_model[ix][2], 5);
			geo_gen.create_cylinder(para[0], para[1], para[2],
				static_cast<int>(para[3]), static_cast<int>(para[4]), geo_mesh.back());
		}
		//
		m_PNTT["default"].m_Mat.push_back(material());
		para = csv_string_to_float(vec2d_model[ix][4], 2);
		model_material_common(
			m_PNTT["default"].m_Mat.back(), para[0], static_cast<int>(para[1]));
		model_rot_front[model_name] = rotation_xyz(vec2d_model[ix][3]);
		std::wstring tex_diffuse(vec2d_model[ix][5].begin(), vec2d_model[ix][5].end());
		tex_diffuse = path_tex+tex_diffuse;
		std::wstring tex_normal(vec2d_model[ix][6].begin(), vec2d_model[ix][6].end());
		tex_normal = path_tex+tex_normal;
		model_is_tex[model_name] = (std::stoi(vec2d_model[ix][7]) != 0);
		m_PNTT["default"].set_MapSRV(m_TexMgr, tex_diffuse, tex_normal, model_is_tex[model_name]);
		model_tex_trans[model_name] = vec2d_model[ix][8];
	}
	model_load_geo_mesh(device, m_PNTT["default"], geo_mesh);
	// build instance
	l_reader.vec2d_str_from_global("csv_instance_geometry", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		m_NamePNTT.push_back(vec2d_instance[ix][0]);
		assert(model_index.count(model_name));
		m_InstPNTT.push_back(simple_model_instance<pos_normal_tex_tan>());
		std::vector<simple_model_instance<pos_normal_tex_tan>>::iterator it;
		it = m_InstPNTT.end();
		// iterator to the last instance
		--it;
		it->model = &m_PNTT["default"];
		it->subid = model_index[model_name];
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance);
		it->is_textrue = model_is_tex[model_name];
		if (model_tex_trans[model_name].size() > 4) {
			std::vector<float> para = csv_string_to_float(model_tex_trans[model_name], 3);
			XMStoreFloat4x4(&it->tex_transform, XMMatrixScaling(para[0], para[1], para[2]));
		}
	}
}
//
void model_mgr::skinned_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::map<std::string, rotation_xyz> model_rot_front;
	std::map<std::string, int> dummy;
	// build model
	model_load_csv_basic(
		device,
		m_Skinned,
		vec2d_model,
		model_rot_front,
		l_reader,
		m_TexMgr,
		"csv_model_skinned",
		GLOBAL["path_mod"],
		std::wstring(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end()),
		dummy);
	// build instance
	l_reader.vec2d_str_from_global("csv_instance_skinned", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		assert(m_Skinned.count(model_name));
		m_NameSkinned.push_back(vec2d_instance[ix][0]);
		std::vector<skinned_model_instance>::iterator it;
		m_InstSkinned.push_back(skinned_model_instance());
		it = m_InstSkinned.end();
		// iterator to the last instance
		--it;
		it->model = &m_Skinned[model_name];
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance);
		//
		it->time_pos = stof(vec2d_instance[ix][7]);
		it->set_ClipName(vec2d_instance[ix][8]);
		it->final_transforms.resize(m_Skinned[model_name].m_SkinnedData.m_BoneCount());
	}
}
//
void model_mgr::basic_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::map<std::string, rotation_xyz> model_rot_front;
	std::map<std::string, int> model_alpha;
	// build model
	model_load_csv_basic(
		device,
		m_Basic,
		vec2d_model,
		model_rot_front,
		l_reader,
		m_TexMgr,
		"csv_model_basic",
		GLOBAL["path_mod"],
		std::wstring(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end()),
		model_alpha);
	// build instance
	l_reader.vec2d_str_from_global("csv_instance_basic", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		assert(m_Basic.count(model_name));
		std::vector<basic_model_instance>::iterator it;
		if (model_alpha[model_name]) {
			m_NameBasicAlpha.push_back(vec2d_instance[ix][0]);
			m_InstBasicAlpha.push_back(basic_model_instance());
			it = m_InstBasicAlpha.end();
		}
		else {
			m_NameBasic.push_back(vec2d_instance[ix][0]);
			m_InstBasic.push_back(basic_model_instance());
			it = m_InstBasic.end();
		}
		// iterator to the last instance
		--it;
		it->model = &m_Basic[model_name];
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance);
	}
}
//
void model_mgr::remove_instance()
{
	m_B32.clear();
	m_PNTT.clear();
	// continue to have m_Skinned m_Basic model
	m_InstB32.clear();
	m_InstPNTT.clear();
	m_InstSkinned.clear();
	m_InstBasicAlpha.clear();
	m_InstBasic.clear();
	m_NameB32.clear();
	m_NamePNTT.clear();
	m_NameSkinned.clear();
	m_NameBasicAlpha.clear();
	m_NameBasic.clear();
	m_InstB32.shrink_to_fit();
	m_InstPNTT.shrink_to_fit();
	m_InstSkinned.shrink_to_fit();
	m_InstBasicAlpha.shrink_to_fit();
	m_InstBasic.shrink_to_fit();
	m_NameB32.shrink_to_fit();
	m_NamePNTT.shrink_to_fit();
	m_NameSkinned.shrink_to_fit();
	m_NameBasicAlpha.shrink_to_fit();
	m_NameBasic.shrink_to_fit();
}
//
}
#endif