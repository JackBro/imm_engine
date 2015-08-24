////////////////
// stru_scene_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_SCENE_MGR_H
#define STRU_SCENE_MGR_H
#include "stru_wave.h"
#include "phy_auxiliary.h"
#include "stru_particle.h"
#include "stru_instance_mgr.h"
#include "render_sky.h"
#include "render_terrain.h"
#include "audio_dxtk.h"
namespace imm
{
////////////////
// scene_mgr
////////////////
////////////////
template <typename T_app>
struct scene_mgr
{
	scene_mgr();
	~scene_mgr();
	void init_load(T_app *app_in);
	void update_atmosphere(float dt);
	void update_listen_thread_for_reload();
	void draw_d3d_atmosphere(XMMATRIX &shadow_transform);
	void reload(const std::wstring &scene_ix_in);
	void reload_in_main_update();
	void reload_instance();
	void reload_skybox();
	void reload_terrain(lua_reader &l_reader);
	void relaod_terrain_after_instance();
	void reload_media();
	T_app *app;
	std::map<std::string, std::string> get_misc;
	light_dir dir_lights[3];
	XMFLOAT3 dir_lights_orignial[3];
	BoundingSphere bounds;
	sky *skybox;
	terrain terrain1;
	phy_property terrain1_phy;
	state_plasma plasma;
	envi_liquid liquid;
	audio_dxtk audio;
	phy_wireframe<T_app> phy_wire;
	std::string scene_ix;
	float begin_time;
	float shadow_light_pos_far;
	bool is_reload_schedule;
	bool is_loading_atmosphere;
	bool is_reload_done;
};
//
template <typename T_app>
scene_mgr<T_app>::scene_mgr():
	skybox(nullptr),
	terrain1(),
	terrain1_phy(),
	plasma(),
	liquid(),
	audio(),
	phy_wire(),
	scene_ix(),
	begin_time(FLT_MAX),
	shadow_light_pos_far(100.0f),
	is_reload_schedule(false),
	is_loading_atmosphere(false),
	is_reload_done(false)
{
	scene_dir_lights_common(dir_lights);
	dir_lights_orignial[0] = dir_lights[0].direction;
	dir_lights_orignial[1] = dir_lights[1].direction;
	dir_lights_orignial[2] = dir_lights[2].direction;
	bounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	bounds.Radius = sqrtf(80.0f*80.0f + 80.0f*80.0f);
}
template <typename T_app>
scene_mgr<T_app>::~scene_mgr()
{
	delete skybox;
}
//
template <typename T_app>
void scene_mgr<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	plasma.init_load(app->m_D3DDevice, app->m_D3DDC);
	audio.init_load();
	phy_wire.init(app);
	app->m_Attack.init_load(app);
	reload(L"00");
}
//
template <typename T_app>
void scene_mgr<T_app>::update_atmosphere(float dt)
{
	float total_time = app->m_Timer.total_time();
	plasma.update(dt, total_time);
	liquid.update(app->m_D3DDC, dt, total_time);
	audio.update();
	app->m_Attack.update();
}
//
template <typename T_app>
void scene_mgr<T_app>::draw_d3d_atmosphere(XMMATRIX &shadow_transform)
{
	// Draw phy_wireframe
	phy_wire.draw();
	// Draw sky
	skybox->draw(app->m_D3DDC, app->m_Cam);
	// Restore default states, as the SkyFX changes them in the effect file.
	app->m_D3DDC->RSSetState(0);
	app->m_D3DDC->OMSetDepthStencilState(0, 0);
	// Draw terrain
	terrain1.draw(app->m_D3DDC, app->m_Cam, dir_lights, app->m_Smap, shadow_transform);
	// Draw liquid
	liquid.draw(app->m_D3DDC, dir_lights, app->m_Cam);
	// Draw particle systems last so it is blended with scene.
	plasma.draw(app->m_D3DDC, app->m_Cam);
	// Restore default states.
	app->m_D3DDC->RSSetState(0);
	app->m_D3DDC->OMSetDepthStencilState(0, 0);
	app->m_D3DDC->OMSetBlendState(0, BLEND_FACTOR_ZERO, 0xffffffff);
}
//
template <typename T_app>
void scene_mgr<T_app>::update_listen_thread_for_reload()
{
	// Clean update in main,
	// that guarantee no other update when reload,
	// avoid thread(instance_mgr::reload).detach and instance_mgr::update conflict
	if (is_reload_schedule) {
		reload_in_main_update();
		is_reload_schedule = false;
	}
	if (!is_reload_done && !app->m_Inst.m_IsLoading && !is_loading_atmosphere) {
		relaod_terrain_after_instance();
		app->m_Cmd.is_preparing = false;
		begin_time = app->m_Timer.total_time();
		is_reload_done = true;
	}
}
//
template <typename T_app>
void scene_mgr<T_app>::reload(const std::wstring &scene_ix_in)
{
	assert(!is_reload_schedule);
	if (is_reload_schedule) return;
	is_reload_schedule = true;
	is_loading_atmosphere = true;
	is_reload_done = false;
	begin_time = FLT_MAX;
	app->m_Cmd.is_preparing = true;
	assert(!app->m_Inst.m_IsLoading);
	app->m_Inst.m_IsLoading = true;
	scene_ix = wstr_to_str(scene_ix_in);
}
//
template <typename T_app>
void scene_mgr<T_app>::reload_in_main_update()
{
	get_misc["plane_ground"] = "";
	get_misc["player1"] = "";
	get_misc["skybox_dds"] = "";
	get_misc["play_bgm"] = "";
	get_misc["ui_class"] = "";
	get_misc["ui_group"] = "";
	get_misc["ui_group"] = "";
	get_misc["terrain_info"] = "";
	lua_reader l_reader;
	std::string describe = IMM_PATH["script"]+"scene"+scene_ix+"\\describe_instance.lua";
	l_reader.loadfile(describe);
	l_reader.map_from_string(get_misc);
	reload_instance();
	reload_skybox();
	reload_terrain(l_reader);
	reload_media();
	is_loading_atmosphere = false;
}
//
template <typename T_app>
void scene_mgr<T_app>::reload_instance()
{
	// Stop control first
	app->m_Control.reset();
	std::thread(&instance_mgr<T_app>::reload, &app->m_Inst).detach();
	liquid.reload(app->m_D3DDevice, scene_ix);
}
//
template <typename T_app>
void scene_mgr<T_app>::reload_skybox()
{
	if (skybox != nullptr) delete skybox;
	if (csv_value_is_empty(get_misc["skybox_dds"])) {
		// Empty sky no draw
		skybox = new sky();
	}
	else {
		std::wstring path_tex(str_to_wstr(IMM_PATH["texture"]));
		path_tex += L"skybox\\";
		path_tex += str_to_wstr(get_misc["skybox_dds"]);
		skybox = new sky(app->m_D3DDevice, path_tex, 5000.0f);
	}
}
//
template <typename T_app>
void scene_mgr<T_app>::reload_terrain(lua_reader &l_reader)
{
	terrain1 = terrain();
	if (get_misc["terrain_info"] == "") return;
	std::map<std::string, std::string> tii_map;
	l_reader.map_from_table(get_misc["terrain_info"], tii_map);
	assert(tii_map.size() > 10);
	auto it = tii_map.begin();
	terrain::init_info tii;
	std::wstring path_tex(str_to_wstr(IMM_PATH["texture"]));
	path_tex += L"terrain\\";
	tii.height_map_filename = path_tex+str_to_wstr(it++->second);
	tii.layer_map_filename0 = path_tex+str_to_wstr(it++->second);
	tii.layer_map_filename1 = path_tex+str_to_wstr(it++->second);
	tii.layer_map_filename2 = path_tex+str_to_wstr(it++->second);
	tii.layer_map_filename3 = path_tex+str_to_wstr(it++->second);
	tii.layer_map_filename4 = path_tex+str_to_wstr(it++->second);
	tii.blend_map_filename  = path_tex+str_to_wstr(it++->second);
	tii.height_scale        = stof(it++->second);
	tii.heightmap_width     = stoi(it++->second);
	tii.heightmap_height    = stoi(it++->second);
	tii.cell_spacing        = stof(it++->second);
	terrain1.init(app->m_D3DDevice, app->m_D3DDC, tii);
}
template <typename T_app>
void scene_mgr<T_app>::relaod_terrain_after_instance()
{
	if (!terrain1.is_initialized()) return;
	// fix instance height
	size_t ix = 0;
	for (auto &inst: app->m_Inst.m_Stat) {
		XMFLOAT4X4 *world = inst.get_World();
		XMMATRIX W = XMLoadFloat4x4(world);
		app->m_Inst.m_BoundL.transform(ix, app->m_Inst.m_BoundW, W);
		float half_y = app->m_Inst.m_BoundW.half_y(ix);
		float height = terrain1.get_Height(world->_41, world->_43) + half_y*2.0f;
		if (world->_42 < height) world->_42 = height+1.0f;
		ix++;
	}
}
template <typename T_app>
void scene_mgr<T_app>::reload_media()
{
	// Audio
	audio.stop_bgm();
	if (!csv_value_is_empty(get_misc["play_bgm"])) audio.play_bgm(get_misc["play_bgm"]);
	// UI
	app->m_UiMgr.reload_active(get_misc["ui_class"], get_misc["ui_group"]);
	app->m_Condition.reset();
	app->m_UiMgr.dialogue.rebuild_text();
}
//
}
#endif