////////////////
// stru_instance_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_INSTANCE_MGR_H
#define STRU_INSTANCE_MGR_H
#include "stru_model_mgr.h"
namespace imm
{
////////////////
// instance_mgr
////////////////
////////////////
template <typename T_app>
struct instance_mgr
{
	instance_mgr();
	void init(T_app *app_in);
	void reload();
	void reload_scene_instance_relate();
	template <typename instance, typename get_pos>
	void push_back(
		const std::vector<instance> &v_inst, instance_stat &inst_stat, size_t &k,
		const get_pos &get_pos_f,
		std::vector<std::string> &name);
	template <typename instance, typename get_pos>
	void push_back_pntt(
		const std::vector<instance> &v_inst, instance_stat &inst_stat, size_t &k,
		const get_pos &get_pos_f,
		std::vector<std::string> &name);
	int get_index(const std::string &name);
	void on_resize();
	void update(const float &dt);
	void bound_update();
	void collision_update(float dt_every);
	void collision_update_plane(float dt_every);
	void collision_update_terrain(float dt_every);
	void collision_update_impulse(float dt_every);
	void update_skinned(const float &dt);
	void update_frustum_culling();
	void remove_all();
	model_mgr m_Model;
	// Bounding only use BoundingBox for lazy develop
	phy_bound_mgr m_BoundL;
	phy_bound_mgr m_BoundW;
	BoundingFrustum m_CamFrustumL;
	BoundingFrustum m_CamFrustumW;
	std::vector<instance_stat> m_Stat;
	std::map<std::string, std::size_t> m_NameMap;
	bool m_IsLoading;
	bool m_IsTerrainUse;
	int m_PlaneGroundIx;
	T_app *m_App;
};
//
template <typename T_app>
instance_mgr<T_app>::instance_mgr():
	m_Model(),
	m_BoundL(),
	m_BoundW(),
	m_IsLoading(false),
	m_IsTerrainUse(false),
	m_PlaneGroundIx(-1),
	m_App(nullptr)
{
	;
}
//
template <typename T_app>
void instance_mgr<T_app>::init(T_app *app_in)
{
	m_App = app_in;
}
//
template <typename T_app>
void instance_mgr<T_app>::reload()
{
	std::string scene_ix(m_App->m_Scene.scene_ix);
	std::wstring load_done(str_to_wstr(scene_ix));
	if (!m_Model.load(m_App->m_D3DDevice, scene_ix)) {
		load_done = L"> Scene "+load_done+L" not found.\n";
		m_App->m_Cmd.input += load_done;
		return;
	}
	m_App->m_Cmd.input += L"> Loading...\n";
	m_App->m_Cmd.input += L"> Please wait...\n";	
	remove_all();
	instance_stat inst_stat;
	size_t k = 0;
	inst_stat.type = basic;
	push_back(
		m_Model.m_InstBasic, inst_stat, k,
		[](const pos_normal_tex_tan2 &x) {return &x.pos;},
		m_Model.m_NameBasic);
	push_back(
		m_Model.m_InstBasicAlpha, inst_stat, k,
		[](const pos_normal_tex_tan2 &x) {return &x.pos;},
		m_Model.m_NameBasicAlpha);
	inst_stat.type = skinned;
	push_back(
		m_Model.m_InstSkinned, inst_stat, k,
		[](const pos_normal_tex_tan_skinned &x) {return &x.pos;},
		m_Model.m_NameSkinned);
	push_back(
		m_Model.m_InstSkinnedAlpha, inst_stat, k,
		[](const pos_normal_tex_tan_skinned &x) {return &x.pos;},
		m_Model.m_NameSkinnedAlpha);
	inst_stat.type = simple_pntt;
	push_back_pntt(
		m_Model.m_InstPNTT, inst_stat, k,
		[](const pos_normal_tex_tan &x) {return &x.pos;},
		m_Model.m_NamePNTT);
	reload_scene_instance_relate();
	on_resize();
	m_IsLoading = false;
	load_done = L"> Scene "+load_done+L" load done\n";
	m_App->m_Cmd.input.assign(load_done);
}
//
template <typename T_app>
void instance_mgr<T_app>::reload_scene_instance_relate()
{	
	if (csv_value_is_empty(m_App->m_Scene.get_misc["terrain_info"])) {
		m_PlaneGroundIx = get_index(m_App->m_Scene.get_misc["plane_ground"]);
		m_IsTerrainUse = false;
	}
	else {
		m_PlaneGroundIx = -1;
		m_IsTerrainUse = true;
	}
	m_App->m_Control.rebuild_player();
	m_App->m_Attack.rebuild_bbox_from_instance();
	m_App->m_Scene.phy_wire.rebuild_buffer();
}
//
template <typename T_app>
template <typename instance, typename get_pos>
void instance_mgr<T_app>::push_back(
	const std::vector<instance> &v_inst, instance_stat &inst_stat, size_t &k,
	const get_pos &get_pos_f,
	std::vector<std::string> &name)
{
	for (size_t ix = 0; ix != v_inst.size(); ++ix) {
		m_NameMap[name[ix]] = k;
		inst_stat.p = &v_inst[ix];
		m_Stat.push_back(inst_stat);
		m_BoundL.push_back_empty(phy_bound_type::box);
		m_BoundW.push_back_empty(phy_bound_type::box);
		phy_set_aabb(
			m_BoundL.b1[k],
			v_inst[ix].model->m_Vertices,
			get_pos_f);
		++k;
	}
}
//
template <typename T_app>
template <typename instance, typename get_pos>
void instance_mgr<T_app>::push_back_pntt(
	const std::vector<instance> &v_inst, instance_stat &inst_stat, size_t &k,
	const get_pos &get_pos_f,
	std::vector<std::string> &name)
{
	for (size_t ix = 0; ix != v_inst.size(); ++ix) {
		m_NameMap[name[ix]] = k;
		inst_stat.p = &v_inst[ix];
		m_Stat.push_back(inst_stat);
		m_BoundL.push_back_empty(phy_bound_type::box);
		m_BoundW.push_back_empty(phy_bound_type::box);
		auto vert_range = v_inst[ix].model->get_VertexRange(v_inst[ix].subid);
		phy_set_aabb(
			m_BoundL.b1[k],
			v_inst[ix].model->m_Vertices,
			get_pos_f,
			vert_range.first,
			vert_range.second);
		++k;
	}
}
//
template <typename T_app>
int instance_mgr<T_app>::get_index(const std::string &name)
{
	if (!m_NameMap.count(name)) return -1;
	return static_cast<int>(m_NameMap[name]);
}
//
template <typename T_app>
void instance_mgr<T_app>::on_resize()
{
	if (m_App == nullptr) return;
	XMMATRIX proj = m_App->m_Cam.get_Proj();
	BoundingFrustum::CreateFromMatrix(m_CamFrustumL, proj);
}
//
template <typename T_app>
void instance_mgr<T_app>::update(const float &dt)
{
	update_skinned(dt);
	bound_update();
	collision_update(dt);
	update_frustum_culling();
}
//
template <typename T_app>
void instance_mgr<T_app>::bound_update()
{
	XMMATRIX world;
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		world = XMLoadFloat4x4(m_Stat[ix].get_World());
		m_BoundL.transform(ix, m_BoundW, world);
	}
}
// should use octree, temporary not implement
template <typename T_app>
void instance_mgr<T_app>::collision_update(float dt_every)
{
	// if runtime stun
	if (dt_every > PHY_MAX_DELTA_TIME) dt_every = PHY_MAX_DELTA_TIME;
	if (m_IsTerrainUse) collision_update_terrain(dt_every);
	else collision_update_plane(dt_every);
	collision_update_impulse(dt_every);
}
//
template <typename T_app>
void instance_mgr<T_app>::collision_update_plane(float dt_every)
{
	if (m_PlaneGroundIx < 0) return;
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		if (static_cast<int>(ix) == m_PlaneGroundIx) continue;
		int ix_gro = m_PlaneGroundIx;
		if (m_Stat[ix].phy.stand_from >= 0) ix_gro = m_Stat[ix].phy.stand_from;
		m_Stat[ix].phy.is_touch_ground = m_BoundW.intersects(ix_gro, ix);
		phy_position_update(
			dt_every,
			*(m_Stat[ix].get_World()),
			m_Stat[ix].phy,
			m_Stat[ix_gro].phy,
			m_BoundW.center(ix),
			m_Stat[ix].phy.is_touch_ground,
			m_BoundW.half_y(ix),
			(m_Stat[m_PlaneGroundIx].get_World())->_42);
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::collision_update_terrain(float dt_every)
{
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		int ix_gro = -1;
		if (m_Stat[ix].phy.stand_from >= 0) ix_gro = m_Stat[ix].phy.stand_from;
		XMFLOAT4X4 *world = m_Stat[ix].get_World();
		float terrain_height = m_App->m_Scene.terrain1.get_Height(world->_41, world->_43);
		XMVECTOR terrain_point = XMVectorSet(world->_41, terrain_height, world->_43, 0.0f);
		int contains = m_BoundW.contains(ix, terrain_point);
		m_Stat[ix].phy.is_touch_ground = (contains != 0);
		phy_position_update(
			dt_every,
			*world,
			m_Stat[ix].phy,
			m_App->m_Scene.terrain1_phy,
			m_BoundW.center(ix),
			m_Stat[ix].phy.is_touch_ground,
			m_BoundW.half_y(ix),
			terrain_height);
		// inaccuracy touch ground, logically assume it touches gourond, because terrain is not flat
		// this is not for physics
		float half_y = m_BoundW.half_y(ix);
		if (world->_42 - half_y - terrain_height < 0.3f) m_Stat[ix].phy.is_touch_ground = true;
		else m_Stat[ix].phy.is_touch_ground = false;
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::collision_update_impulse(float dt_every)
{
	for (int ix = 0; ix < static_cast<int>(m_Stat.size()-1); ++ix) {
		for (size_t ix2 = ix+1; ix2 != m_Stat.size(); ++ix2) {
			if (static_cast<int>(ix) == m_PlaneGroundIx || static_cast<int>(ix2) == m_PlaneGroundIx) continue;
			phy_impulse_casual(dt_every,
				*(m_Stat[ix].get_World()),
				*(m_Stat[ix2].get_World()),
				m_Stat[ix].phy,
				m_Stat[ix2].phy,
				m_BoundW.center(ix),
				m_BoundW.center(ix2),
				m_BoundW.intersects(ix, ix2));
		}
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::update_skinned(const float &dt)
{
	if (m_App->m_Cmd.is_preparing) return;
	// should be multithread?
	for (auto &skinned: m_Model.m_InstSkinned) skinned.update(dt);
	for (auto &skinned: m_Model.m_InstSkinnedAlpha) skinned.update(dt);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_frustum_culling()
{
	XMVECTOR det_view = XMMatrixDeterminant(m_App->m_Cam.get_View());
	XMMATRIX inv_view = XMMatrixInverse(&det_view, m_App->m_Cam.get_View());
	m_CamFrustumL.Transform(m_CamFrustumW, inv_view);
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		if (m_CamFrustumW.Intersects(m_BoundW.b1[m_BoundW.map[ix].second])) {
			m_Stat[ix].set_IsAppear(true);
		}
		else {
			m_Stat[ix].set_IsAppear(false);
		}
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::remove_all()
{
	m_BoundL.remove_all();
	m_BoundW.remove_all();
	m_Stat.clear();
	m_Stat.shrink_to_fit();
}
//
}
#endif