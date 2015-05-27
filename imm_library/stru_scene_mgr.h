////////////////
// stru_scene_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_SCENE_MGR_H
#define STRU_SCENE_MGR_H
#include "audio_dxtk.h"
#include "stru_element.h"
#include "stru_instance_mgr.h"
#include "render_sky.h"
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
	void draw_d3d_atmosphere();
	void reload(const std::wstring &scene_ix);
	T_app *app;
	std::map<std::string, std::string> misc_info;
	lit_dir dir_lights[3];
	BoundingSphere bounds;
	sky *skybox;
	bright_aura aura;
	audio_dxtk audio;
	phy_wireframe<T_app> phy_wire;
};
//
template <typename T_app>
scene_mgr<T_app>::scene_mgr():
	skybox(nullptr),
	aura(),
	audio()
{
	scene_dir_lights_common(dir_lights);
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
	//aura.init_load(app->m_D3DDevice, app->m_D3DDC);
	aura.is_active = false;
	audio.init_load();
	phy_wire.init(app);
	reload(L"00");
}
//
template <typename T_app>
void scene_mgr<T_app>::update_atmosphere(float dt)
{
	aura.update(dt, app->m_Timer.total_time());
	audio.aud_engine->Update();
}
//
template <typename T_app>
void scene_mgr<T_app>::draw_d3d_atmosphere()
{
	// Draw phy_wireframe
	phy_wire.draw();
	// Draw sky
	skybox->draw(app->m_D3DDC, app->m_Cam);
	// Restore default states, as the SkyFX changes them in the effect file.
	app->m_D3DDC->RSSetState(0);
	app->m_D3DDC->OMSetDepthStencilState(0, 0);
	// Draw particle systems last so it is blended with scene.
	aura.draw(app->m_D3DDC, app->m_Cam);
	// Restore default states.
	app->m_D3DDC->RSSetState(0);
	app->m_D3DDC->OMSetDepthStencilState(0, 0);
	app->m_D3DDC->OMSetBlendState(0, BLEND_FACTOR_ZERO, 0xffffffff);
}
//
template <typename T_app>
void scene_mgr<T_app>::reload(const std::wstring &scene_ix)
{
	assert(!app->m_Inst.m_IsLoading);
	std::string scene_ix_str(scene_ix.begin(), scene_ix.end());
	misc_info["ground"] = "";
	misc_info["player1"] = "";
	misc_info["skybox_file"] = "";
	misc_info["play_bgm"] = "";
	misc_info["ui_class"] = "";
	misc_info["ui_group"] = "";
	lua_reader l_reader;
	std::string describe = GLOBAL["path_lua"]+"scene"+scene_ix_str+"\\describe_instance.lua";
	l_reader.loadfile(describe);
	l_reader.map_from_global(misc_info);
	// Instance
	app->m_Control.reset();
	std::thread(
		&instance_mgr::load,
		std::ref(app->m_Inst),
		app->m_D3DDevice,
		scene_ix_str,
		misc_info["ground"],
		std::ref(app->m_Cmd.is_preparing),
		std::ref(app->m_Cmd.input)).detach();
	// Skybox
	if (skybox != nullptr) delete skybox;
	if (csv_value_is_empty(misc_info["skybox_file"])) {
		// Empty sky no draw
		skybox = new sky();
	}
	else {
		std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
		path_tex += str_to_wstr(misc_info["skybox_file"]);
		skybox = new sky(app->m_D3DDevice, path_tex, 5000.0f);
	}
	// Audio
	audio.stop_bgm();
	if (!csv_value_is_empty(misc_info["play_bgm"])) audio.play_bgm(misc_info["play_bgm"]);
	// UI
	app->m_UiMgr.reload_active(misc_info["ui_class"], misc_info["ui_group"]);
}
//
}
#endif