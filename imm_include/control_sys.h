////////////////
// control_sys.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_SYS_H
#define CONTROL_SYS_H
#include "stru_instance_mgr.h"
#include "imm_camera.h"
#include "phy_prepare.h"
#include "control_auxiliary.h"
namespace imm
{
////////////////
// CONTROL_STYLE_TYPE
////////////////
////////////////
enum CONTROL_STYLE_TYPE {
	CONTROL_ZERO          = 0x0,
	CONTORL_CAM_FREE      = 0x1,
	CONTORL_MOVE_BY_MOUSE = 0x2,
};
////////////////
// control_sys
////////////////
////////////////
template <typename T_app>
struct control_sys
{
	control_sys();
	// logic fuction
	void init(T_app *app_in);
	void reset();
	void rebuild_player();
	void mouse_instance_move();
	void pad_instance_move_update();
	void mouse_pick();
	void update_scene(const float &dt);
	void update_scene_bounds();
	void update_stop(const float &dt);
	void update_keydown_and_pad(const float &dt);
	void on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_pad_down(const float &dt);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_mouse_wheel(const short &z_delta);
	// cam function
	void pad_camera_update(const float &dt);
	void on_pad_camera_follow(const WORD &vkey);
	void key_camera_free_update(const float &dt);
	void mouse_camera_wheel(const short &z_delta);
	void mouse_camera_move();
	void cam_follow_update();
	// member variable
	T_app *app;
	int picked1;
	int player1;
	int style1;
	bool is_pad_cam_follow_reset;
	float cam_follow_walk_def;
	float cam_follow_up_def;
	float cam_follow_walk;
	float cam_follow_up;
	float wait_ui_disappear;
	POINT mouse_down;
	POINT mouse_move;
	std::map<size_t, control_stop<T_app>> map_stop;
	std::map<size_t, XMFLOAT4> map_rot_front_c;
	control_xinput pad;
};
//
template <typename T_app>
control_sys<T_app>::control_sys():
	app(nullptr),
	picked1(-1),
	player1(-1),
	style1(CONTORL_MOVE_BY_MOUSE),
	is_pad_cam_follow_reset(false),
	cam_follow_walk_def(-30.0f),
	cam_follow_up_def(3.0f),
	wait_ui_disappear(0.0f),
	pad()
{
	cam_follow_walk = cam_follow_walk_def;
	cam_follow_up = cam_follow_up_def;
	mouse_down.x = 0;
	mouse_down.y = 0;
	mouse_move.x = 0;
	mouse_move.y = 0;
}
//
template <typename T_app>
void control_sys<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void control_sys<T_app>::reset()
{
	map_stop.clear();
	map_rot_front_c.clear();
	player1 = -1;
	picked1 = -1;
}
//
template <typename T_app>
void control_sys<T_app>::rebuild_player()
{
	player1 = static_cast<int>(app->m_Inst.get_index(app->m_Scene.get_misc["player1"]));
	assert(player1 > -1);
}
//
template <typename T_app>
void control_sys<T_app>::mouse_instance_move()
{
	if (player1 < 0) return;
	app->m_Inst.m_Troll[player1].order |= ORDER_MOVE_HIT;
}
//
template <typename T_app>
void control_sys<T_app>::pad_instance_move_update()
{
	if (player1 < 0) return;
	app->m_Inst.m_Troll[player1].order |= ORDER_MOVE_TOWARD;
	if (pad.is_RT_press()) app->m_Inst.m_Troll[player1].order_stat |= ORDER_IS_WALK;
	else app->m_Inst.m_Troll[player1].order_stat &= ~ORDER_IS_WALK;
}
//
template <typename T_app>
void control_sys<T_app>::mouse_pick()
{
	app->m_Inst.m_BoundW.pick(
		mouse_down.x,
		mouse_down.y,
		app->m_ClientWidth,
		app->m_ClientHeight,
		app->m_Cam.get_Proj(),
		app->m_Cam.get_View(),
		picked1);
	//
	if (picked1 < 0) return;
	player1 = picked1;
}
//
template <typename T_app>
void control_sys<T_app>::update_scene(const float &dt)
{
	// assert check update
	assert(player1 > -1);
	app->m_Inst.update(dt);
	update_scene_bounds();
	update_stop(dt);
	// camera follow update even m_Cmd.is_active()
	cam_follow_update();
}
//
template <typename T_app>
void control_sys<T_app>::update_scene_bounds()
{
	// bounds follow player, if distance too far, update
	XMFLOAT3 p1_center_f = app->m_Inst.m_BoundW.center(player1);
	XMVECTOR p1_center = XMLoadFloat3(&p1_center_f);
	XMVECTOR scene_center = XMLoadFloat3(&app->m_Scene.bounds.Center);
	XMVECTOR distance = XMVectorSubtract(p1_center, scene_center);
	distance = XMVector3LengthEst(distance);
	if (XMVectorGetX(distance) > 20.0f) {
		app->m_Scene.bounds.Center = app->m_Inst.m_BoundW.center(player1);
	}
}
//
template <typename T_app>
void control_sys<T_app>::update_stop(const float &dt)
{
	for (auto it = map_stop.begin(); it != map_stop.end(); ++it) {
		it->second.update(app, it->first, dt);
	}
}
//
template <typename T_app>
void control_sys<T_app>::update_keydown_and_pad(const float &dt)
{
	// player1 and camera update, if !m_Cmd.is_active()
	if (pad.is_enable()) {
		on_pad_down(dt);
		pad_instance_move_update();
		pad_camera_update(dt);
	}
	key_camera_free_update(dt);
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	mouse_down.x = pos_x;
	mouse_down.y = pos_y;
	if (app->m_UiMgr.on_mouse_down(btn_state, mouse_down.x, mouse_down.y)) return;
	if (btn_state & MOUSE_P1_PICK) mouse_pick();
	if (pad.is_enable()) return;
	// player
	if (btn_state & MOUSE_P1_MOVE) mouse_instance_move();
}
//
template <typename T_app>
void control_sys<T_app>::on_pad_down(const float &dt)
{
	WORD get_vkey;
	if (pad.is_on_keydown(get_vkey)) app->m_UiMgr.on_pad_keydown(get_vkey);
	// avoid ui conflict with control
	if (app->m_UiMgr.is_ui_appear()) wait_ui_disappear = 0.3f;
	wait_ui_disappear -= dt;
	if (wait_ui_disappear > 0.0f) return;
	wait_ui_disappear = -1.0f;
	on_pad_camera_follow(get_vkey);
	if (player1 < 0) return;
	if (get_vkey == PAD_P1_JUMP) app->m_Inst.m_Troll[player1].order |= ORDER_JUMP;
}
//
template <typename T_app>
void control_sys<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	app->m_UiMgr.on_input_keydown(w_param, l_param);
	if (pad.is_enable()) return;
	if (app->m_Cmd.is_active) return;
	if (player1 < 0) return;
	if (w_param == KEY_P1_WALK_RUN) app->m_Inst.m_Troll[player1].order_stat ^= ORDER_IS_WALK;
	if (w_param == KEY_P1_JUMP) app->m_Inst.m_Troll[player1].order |= ORDER_JUMP;
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	mouse_move.x = pos_x;
	mouse_move.y = pos_y;
	if ((btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0)) mouse_camera_move();
	app->m_UiMgr.on_mouse_over(mouse_move.x, mouse_move.y);
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_wheel(const short &z_delta)
{
	if (app->m_UiMgr.on_mouse_wheel(z_delta)) return;
	mouse_camera_wheel(z_delta);
}
////////////////
// inl
////////////////
////////////////
#include "control_sys_cam.h"
}
#endif