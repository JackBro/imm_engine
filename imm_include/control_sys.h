////////////////
// control_sys.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_SYS_H
#define CONTROL_SYS_H
#include "control_cam.h"
#include "control_atk.h"
namespace imm
{
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
	void key_instance_move_update();
	void key_instance_move_switch_stat();
	void instance_jump();
	void instance_atk_x();
	void mouse_pick();
	void update_scene(const float &dt);
	void update_scene_bounds();
	void update_stop(const float &dt);
	void update_keydown_and_pad(const float &dt);
	void on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_pad_down_update(const float &dt);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_mouse_wheel(const short &z_delta);
	// member variable
	T_app *app;
	int picked1;
	int player1;
	int style1;
	float wait_ui_disappear;
	POINT mouse_down;
	POINT mouse_move;
	std::map<size_t, control_stop<T_app>> map_stop;
	control_xinput pad;
	control_cam<T_app> cam;
	control_atk<T_app> atk;
	control_atk<T_app> atk_B;
};
//
template <typename T_app>
control_sys<T_app>::control_sys():
	app(nullptr),
	picked1(-1),
	player1(-1),
	style1(CONTROL_CAM_FOLLOW),
	wait_ui_disappear(0.0f),
	pad(),
	cam()
{
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
	cam.init(app);
	atk.init(app, "");
}
//
template <typename T_app>
void control_sys<T_app>::reset()
{
	map_stop.clear();
	cam.reset();
	atk.reset();
	atk_B.reset();
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
	if (player1 < 0 || !(style1 & CONTORL_CAM_FREE)) return;
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
void control_sys<T_app>::key_instance_move_update()
{
	if (player1 < 0 || (style1 & CONTORL_CAM_FREE)) return;
	app->m_Inst.m_Troll[player1].order |= ORDER_MOVE_WASD;
}
//
template <typename T_app>
void control_sys<T_app>::key_instance_move_switch_stat()
{
	app->m_Inst.m_Troll[player1].order_stat ^= ORDER_IS_WALK;
}
//
template <typename T_app>
void control_sys<T_app>::instance_jump()
{
	app->m_Inst.m_Troll[player1].order |= ORDER_JUMP;
}
//
template <typename T_app>
void control_sys<T_app>::instance_atk_x()
{
	app->m_Inst.m_Troll[player1].order |= ORDER_ATK_X;
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
	map_stop[player1].is_stop = true;
}
//
template <typename T_app>
void control_sys<T_app>::update_scene(const float &dt)
{
	// assert check update
	assert(player1 > -1);
	app->m_Inst.update_all_physics(dt);
	update_scene_bounds();
	update_stop(dt);
	// camera follow update even m_Cmd.is_active()
	cam.follow_update();
	atk.update(dt);
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
	
	if (app->m_Cmd.is_active) return;
	// player1 and camera update, if !m_Cmd.is_active()
	if (pad.is_enable()) {
		on_pad_down_update(dt);
		pad_instance_move_update();
		cam.pad_update(dt);
	}
	else {
		key_instance_move_update();
	}
	cam.key_free_update(dt);
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
	if (app->m_Cmd.is_active) return;
	// player
	if (btn_state & MOUSE_P1_MOVE) mouse_instance_move();
	if (btn_state & MOUSE_P1_ATK_X) instance_atk_x();
}
//
template <typename T_app>
void control_sys<T_app>::on_pad_down_update(const float &dt)
{
	if (app->m_Cmd.is_active) return;
	WORD get_vkey;
	if (pad.is_on_keydown(get_vkey)) app->m_UiMgr.on_pad_keydown(get_vkey);
	// avoid ui conflict with control
	if (app->m_UiMgr.is_ui_appear()) wait_ui_disappear = 0.3f;
	wait_ui_disappear -= dt;
	if (wait_ui_disappear > 0.0f) return;
	wait_ui_disappear = -1.0f;
	cam.on_pad_follow(get_vkey);
	if (player1 < 0) return;
	if (get_vkey == PAD_P1_JUMP) instance_jump();
	if (get_vkey == PAD_P1_ATK_X) instance_atk_x();
}
//
template <typename T_app>
void control_sys<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	app->m_UiMgr.on_input_keydown(w_param, l_param);
	if (pad.is_enable()) return;
	if (app->m_Cmd.is_active) return;
	if (player1 < 0) return;
	if (w_param == KEY_P1_WALK_RUN) key_instance_move_switch_stat();
	if (w_param == KEY_P1_JUMP) instance_jump();
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	mouse_move.x = pos_x;
	mouse_move.y = pos_y;
	if ((btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0)) cam.mouse_move();
	app->m_UiMgr.on_mouse_over(mouse_move.x, mouse_move.y);
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_wheel(const short &z_delta)
{
	if (app->m_UiMgr.on_mouse_wheel(z_delta)) return;
	cam.mouse_wheel(z_delta);
}
//
}
#endif