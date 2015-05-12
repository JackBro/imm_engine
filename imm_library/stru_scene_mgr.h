////////////////
// stru_scene_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_SCENE_MGR_H
#define STRU_SCENE_MGR_H
#include "stru_element.h"
#include "stru_instance_mgr.h"
#include "cast_sky.h"
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
	void init(T_app *app_in);
	void update_atmosphere(float dt);
	T_app *app;
	std::map<std::string, std::string> g_map;
	lit_dir dir_lights[3];
	BoundingSphere bounds;
	sky *skybox;
	bright_aura aura;
};
//
template <typename T_app>
scene_mgr<T_app>::scene_mgr():
	skybox(nullptr),
	aura()
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
void scene_mgr<T_app>::init(T_app *app_in)
{
	app = app_in;
	g_map["ground"] = "";
	g_map["player1"] = "";
	lua_reader l_reader;
	l_reader.loadfile(GLOBAL["path_lua"]+"describe_scene.lua");
	l_reader.map_from_global(g_map);
	app->m_Inst.m_SceneGround = g_map["ground"];
	//
	std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
	skybox = new sky(app->m_D3DDevice, path_tex+L"sky_drywoods_hd.dds", 5000.0f);
	std::thread(
		&instance_mgr::init, std::ref(app->m_Inst), app->m_D3DDevice,
		std::ref(app->m_Cmd.is_loading), std::ref(app->m_Cmd.input)).detach();
	aura.init(app->m_D3DDevice, app->m_D3DDC);
	aura.is_active = false;
}
//
template <typename T_app>
void scene_mgr<T_app>::update_atmosphere(float dt)
{
	aura.update(dt, app->m_Timer.total_time());
}
//
}
#endif