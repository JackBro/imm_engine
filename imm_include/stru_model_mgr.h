////////////////
// stru_model_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_MODEL_MGR_H
#define STRU_MODEL_MGR_H
#include "phy_position_logic.h"
#include "stru_load_help.h"
#include "mesh_basic_model.h"
#include <thread>
namespace imm
{
////////////////
// MODEL_TYPE
////////////////
////////////////
enum MODEL_TYPE
{
	MODEL_BASIC,
	MODEL_SKINNED,
	MODEL_SIMPLE_P,
};
////////////////
// instance_stat
////////////////
////////////////
struct instance_stat
{
	instance_stat();
	const void *ptr;
	phy_property phy;
	MODEL_TYPE type;
	bool is_attach;
	//
	XMFLOAT4X4 *get_World();
	XMFLOAT4X4 *get_RotFront();
	XMFLOAT4X4 *get_FinalTransform(size_t ix);
	std::string *get_ModelName();
	PHY_BOUND_TYPE get_BoundType();
	void set_World(const XMFLOAT4X4 &world);
	void set_World(const XMMATRIX &world);
	void set_IsAppear(const bool &is_appear);
	void set_ClipName(const std::string &clip_name, const bool &is_reset_time);
	void check_set_ClipName(const std::string &clip_name, const bool &is_reset_time);
	bool is_invoke_physics();
};
//
instance_stat::instance_stat():
	ptr(nullptr),
	phy(),
	type(MODEL_BASIC),
	is_attach(false)
{
	;
}
//
XMFLOAT4X4 *instance_stat::get_World()
{
	switch(type) {
	case MODEL_BASIC: return &(((basic_model_instance*)ptr)->world);
	case MODEL_SKINNED: return &(((skinned_model_instance*)ptr)->world);
	case MODEL_SIMPLE_P: return &(((simple_model_instance<vertex::pntt>*)ptr)->world);
	}
	assert(false);
	// bad
	return &(((basic_model_instance*)ptr)->world);
}
//
XMFLOAT4X4 *instance_stat::get_RotFront()
{
	switch(type) {
	case MODEL_BASIC: return &(((basic_model_instance*)ptr)->rot_front);
	case MODEL_SKINNED: return &(((skinned_model_instance*)ptr)->rot_front);
	case MODEL_SIMPLE_P: return &(((simple_model_instance<vertex::pntt>*)ptr)->rot_front);
	}
	assert(false);
	// bad
	return &(((basic_model_instance*)ptr)->rot_front);
}
//
XMFLOAT4X4 *instance_stat::get_FinalTransform(size_t ix)
{
	assert(type == MODEL_SKINNED);
	return &((skinned_model_instance*)ptr)->final_transforms[ix];
}
//
std::string *instance_stat::get_ModelName()
{
	switch(type) {
	case MODEL_BASIC: return &(((basic_model_instance*)ptr)->model_name);
	case MODEL_SKINNED: return &(((skinned_model_instance*)ptr)->model_name);
	case MODEL_SIMPLE_P: return &(((simple_model_instance<vertex::pntt>*)ptr)->model_name);
	}
	assert(false);
	// bad
	return &(((basic_model_instance*)ptr)->model_name);
}
//
PHY_BOUND_TYPE instance_stat::get_BoundType()
{
	int bound_t = 0;
	switch(type) {
	case MODEL_BASIC:
		bound_t = ((basic_model_instance*)ptr)->model->m_BoundType;
		break;
	case MODEL_SKINNED:
		bound_t = ((skinned_model_instance*)ptr)->model->m_BoundType;
		break;
	case MODEL_SIMPLE_P:
		bound_t = ((simple_model_instance<vertex::pntt>*)ptr)->model->m_NameBoundType.at(
			((simple_model_instance<vertex::pntt>*)ptr)->model_name);
		break;
	}
	assert(bound_t <= 2 && bound_t >= 0);
	return static_cast<PHY_BOUND_TYPE>(bound_t);
}
//
void instance_stat::set_World(const XMFLOAT4X4 &world)
{
	switch(type) {
	case MODEL_BASIC: ((basic_model_instance*)ptr)->world = world; return;
	case MODEL_SKINNED: ((skinned_model_instance*)ptr)->world = world; return;
	case MODEL_SIMPLE_P: ((simple_model_instance<vertex::pntt>*)ptr)->world = world; return;
	}
	assert(false);
}
//
void instance_stat::set_World(const XMMATRIX &world)
{
	switch(type) {
	case MODEL_BASIC: XMStoreFloat4x4(&((basic_model_instance*)ptr)->world, world); return;
	case MODEL_SKINNED: XMStoreFloat4x4(&((skinned_model_instance*)ptr)->world, world); return;
	case MODEL_SIMPLE_P: XMStoreFloat4x4(&((simple_model_instance<vertex::pntt>*)ptr)->world, world); return;
	}
	assert(false);
}
//
void instance_stat::set_IsAppear(const bool &is_appear)
{
	switch(type) {
	case MODEL_BASIC: ((basic_model_instance*)ptr)->is_appear = is_appear; return;
	case MODEL_SKINNED: ((skinned_model_instance*)ptr)->is_appear = is_appear; return;
	case MODEL_SIMPLE_P: ((simple_model_instance<vertex::pntt>*)ptr)->is_appear = is_appear; return;
	}
	assert(false);
}
//
void instance_stat::set_ClipName(const std::string &clip_name, const bool &is_reset_time = false)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)ptr)->set_ClipName(clip_name, is_reset_time);
}
//
void instance_stat::check_set_ClipName(const std::string &clip_name, const bool &is_reset_time = false)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)ptr)->check_set_ClipName(clip_name, is_reset_time);
}
//
bool instance_stat::is_invoke_physics()
{
	if (is_attach) return false;
	return true;
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
	void pntt_init(ID3D11Device *device, lua_reader &l_reader);
	void skinned_init(ID3D11Device *device, lua_reader &l_reader);
	void basic_init(ID3D11Device *device, lua_reader &l_reader);
	void remove_all();
	texture_mgr m_TexMgr;
	bool m_IsInitialized;
	std::map<std::string, simple_model<vertex::pntt>> m_PNTT;
	std::map<std::string, skinned_model> m_Skinned;
	std::map<std::string, basic_model> m_Basic;
	std::vector<simple_model_instance<vertex::pntt>> m_InstPNTT;
	std::vector<skinned_model_instance> m_InstSkinnedAlpha;
	std::vector<skinned_model_instance> m_InstSkinned;
	std::vector<basic_model_instance> m_InstBasicAlpha;
	std::vector<basic_model_instance> m_InstBasic;
	std::vector<std::string> m_NamePNTT;
	std::vector<std::string> m_NameSkinnedAlpha;
	std::vector<std::string> m_NameSkinned;
	std::vector<std::string> m_NameBasicAlpha;
	std::vector<std::string> m_NameBasic;
};
model_mgr::model_mgr():
	m_TexMgr(),
	m_IsInitialized(false)
{
	;
}
//
void model_mgr::init(ID3D11Device *device)
{
	if (!m_IsInitialized) {
		m_TexMgr.init(device);
		m_IsInitialized = true;
	}
}
//
bool model_mgr::load(ID3D11Device *device, const std::string& scene_ix)
{
	init(device);
	std::string concrete = IMM_PATH["script"]+"scene"+scene_ix+"\\concrete_instance.lua";
	if (!data_is_file_exist(concrete)) return false;
	remove_all();
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::thread(&model_mgr::pntt_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::skinned_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::basic_init, this, device, std::ref(l_reader)).join();
	return true;
}
//
//
void model_mgr::pntt_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::map<std::string, rotation_xyz> model_rot_front;
	std::map<std::string, bool> model_is_tex;
	std::map<std::string, std::string> model_tex_trans;
	// build model
	l_reader.vec2d_str_from_table("csv_model_geometry", vec2d_model);
	std::vector<geometry::mesh_data> geo_mesh;
	geometry geo_gen;
	m_PNTT["default"];
	std::wstring path_tex(str_to_wstr(IMM_PATH["texture"]));
	for (size_t ix = 1; ix < vec2d_model.size(); ++ix) {
		std::string model_name = vec2d_model[ix][0];
		m_PNTT["default"].m_NameSubid[model_name] = static_cast<UINT>(ix-1);
		geo_mesh.push_back(geometry::mesh_data());
		std::vector<float> para;
		if (vec2d_model[ix][1] == "box") {
			para = csv_string_to_float(vec2d_model[ix][2], 3);
			geo_gen.create_box(para[0], para[1], para[2], geo_mesh.back());
		}
		else if (vec2d_model[ix][1] == "grid") {
			para = csv_string_to_float(vec2d_model[ix][2], 4);
			geo_gen.create_grid(para[0], para[1],
				static_cast<int>(para[2]), static_cast<int>(para[3]), geo_mesh.back());
		}
		else if (vec2d_model[ix][1] == "sphere") {
			para = csv_string_to_float(vec2d_model[ix][2], 3);
			geo_gen.create_sphere(para[0],
				static_cast<int>(para[1]), static_cast<int>(para[2]), geo_mesh.back());
		}
		else if (vec2d_model[ix][1] == "cylinder") {
			para = csv_string_to_float(vec2d_model[ix][2], 5);
			geo_gen.create_cylinder(para[0], para[1], para[2],
				static_cast<int>(para[3]), static_cast<int>(para[4]), geo_mesh.back());
		}
		else ERROR_MESA("model_geometry.csv load type error.");
		//
		m_PNTT["default"].m_Mat.push_back(material());
		para = csv_string_to_float(vec2d_model[ix][4], 2);
		model_material_common(
			m_PNTT["default"].m_Mat.back(), para[0], static_cast<int>(para[1]));
		model_rot_front[model_name] = rotation_xyz(vec2d_model[ix][3]);
		std::wstring tex_subpath = str_to_wstr(vec2d_model[ix][5]);
		std::wstring tex_diffuse(vec2d_model[ix][6].begin(), vec2d_model[ix][6].end());
		tex_diffuse = path_tex+tex_subpath+tex_diffuse;
		std::wstring tex_normal(vec2d_model[ix][7].begin(), vec2d_model[ix][7].end());
		tex_normal = path_tex+tex_subpath+tex_normal;
		model_is_tex[model_name] = !csv_value_is_empty(vec2d_model[ix][6]);
		m_PNTT["default"].set_MapSRV(m_TexMgr, tex_diffuse, tex_normal, model_is_tex[model_name]);
		model_tex_trans[model_name] = vec2d_model[ix][8];
		m_PNTT["default"].m_NameBoundType[model_name] = phy_bound_type_str(vec2d_model[ix][9]);
	}
	model_load_geo_mesh(device, m_PNTT["default"], geo_mesh);
	// build instance
	l_reader.vec2d_str_from_table("csv_instance_geometry", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		m_NamePNTT.push_back(vec2d_instance[ix][0]);
		assert(m_PNTT["default"].m_NameSubid.count(model_name));
		m_InstPNTT.push_back(simple_model_instance<vertex::pntt>());
		std::vector<simple_model_instance<vertex::pntt>>::iterator it;
		it = m_InstPNTT.end();
		// iterator to the last instance
		--it;
		it->model = &m_PNTT["default"];
		it->model_name = model_name;
		it->subid = m_PNTT["default"].m_NameSubid[model_name];
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
	std::map<std::string, int> model_alpha;
	// build model
	model_load_csv_basic(
		device,
		m_Skinned,
		vec2d_model,
		model_rot_front,
		l_reader,
		m_TexMgr,
		"csv_model_skinned",
		IMM_PATH["model"],
		str_to_wstr(IMM_PATH["texture"]),
		model_alpha);
	// build instance
	l_reader.vec2d_str_from_table("csv_instance_skinned", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		assert(m_Skinned.count(model_name));
		std::vector<skinned_model_instance>::iterator it;
		if (model_alpha[model_name]) {
			m_NameSkinnedAlpha.push_back(vec2d_instance[ix][0]);
			m_InstSkinnedAlpha.push_back(skinned_model_instance());
			it = m_InstSkinnedAlpha.end();
		}
		else {
			m_NameSkinned.push_back(vec2d_instance[ix][0]);
			m_InstSkinned.push_back(skinned_model_instance());
			it = m_InstSkinned.end();
		}
		// iterator to the last instance
		--it;
		it->model = &m_Skinned[model_name];
		it->model_name = model_name;
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
		IMM_PATH["model"],
		str_to_wstr(IMM_PATH["texture"]),
		model_alpha);
	// build instance
	l_reader.vec2d_str_from_table("csv_instance_basic", vec2d_instance);
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
		it->model_name = model_name;
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance);
	}
}
//
void model_mgr::remove_all()
{
	m_PNTT.clear();
	// continue to have m_Skinned m_Basic model
	m_InstPNTT.clear();
	m_InstSkinnedAlpha.clear();
	m_InstSkinned.clear();
	m_InstBasicAlpha.clear();
	m_InstBasic.clear();
	m_NamePNTT.clear();
	m_NameSkinnedAlpha.clear();
	m_NameSkinned.clear();
	m_NameBasicAlpha.clear();
	m_NameBasic.clear();
	m_InstPNTT.shrink_to_fit();
	m_InstSkinnedAlpha.shrink_to_fit();
	m_InstSkinned.shrink_to_fit();
	m_InstBasicAlpha.shrink_to_fit();
	m_InstBasic.shrink_to_fit();
	m_NamePNTT.shrink_to_fit();
	m_NameSkinnedAlpha.shrink_to_fit();
	m_NameSkinned.shrink_to_fit();
	m_NameBasicAlpha.shrink_to_fit();
	m_NameBasic.shrink_to_fit();
}
//
}
#endif