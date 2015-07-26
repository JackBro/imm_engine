////////////////
// control_system.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_SYSTEM_H
#define CONTROL_SYSTEM_H
#include "stru_instance_mgr.h"
#include "imm_camera.h"
#include "phy_prepare.h"
#include "control_auxiliary.h"
namespace imm
{
////////////////
// control style
////////////////
////////////////
static const int CONTORL_CAM_FREE = 128;
static const int CONTORL_MOVE_BY_MOUSE = 64;
////////////////
// control_mov
////////////////
////////////////
template <typename T_app>
struct control_mov
{
	control_mov();
	// logic fuction
	void init(T_app *app_in);
	void reset();
	void rebuild_player();
	void mouse_instance_move(const int &pos_x, const int &pos_y);
	void pad_instance_move_update();
	void common_jump();
	void mouse_pick(const int &pos_x, const int &pos_y);
	void update_scene(const float &dt);
	void update_scene_bounds();
	void update_stop(const float &dt);
	void update_keydown_and_pad(const float &dt);
	void on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_pad_down(const float &dt);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_mouse_wheel(const short &z_delta);
	// math function
	void math_mouse_move_toward_hit(
		CXMVECTOR &hit_pos,
		const size_t &index,
		const float &speed = -1.0f);
	void math_mouse_face_rot_y(
		XMMATRIX &W,
		XMMATRIX &RF,
		CXMVECTOR &direction);
	void math_pad_face_rot_y(
		XMMATRIX &W,
		XMMATRIX &RF,
		XMVECTOR &direction,
		const float &rot_cam);
	void math_mouse_hit_plane_y(
		const int &pos_x,
		const int &pos_y,
		XMVECTOR &hit_pos_out);
	void math_mouse_hit_terrain(
		const int &pos_x,
		const int &pos_y,
		XMVECTOR &hit_pos_out);
	void math_pad_move_toward();
	// cam function
	void pad_camera_update(const float &dt);
	void on_pad_camera_follow(const WORD &vkey);
	void key_camera_free_update(const float &dt);
	void mouse_camera_wheel(const short &z_delta);
	void mouse_camera_move(const int &pos_x, const int &pos_y);
	void cam_follow_update();
	// member variable
	T_app *app;
	int picked1;
	int player1;
	int style_p1;
	bool is_pad_cam_follow_reset;
	float cam_follow_walk_def;
	float cam_follow_up_def;
	float cam_follow_walk;
	float cam_follow_up;
	float wait_ui_disappear;
	std::map<size_t, control_stop> map_stop;
	std::map<size_t, XMFLOAT4> map_rot_front_c;
	control_xinput pad;
	control_motion motion;
};
//
template <typename T_app>
control_mov<T_app>::control_mov():
	app(nullptr),
	picked1(-1),
	player1(-1),
	style_p1(CONTORL_MOVE_BY_MOUSE),
	is_pad_cam_follow_reset(false),
	cam_follow_walk_def(-30.0f),
	cam_follow_up_def(3.0f),
	wait_ui_disappear(0.0f),
	pad()
{
	cam_follow_walk = cam_follow_walk_def;
	cam_follow_up = cam_follow_up_def;
}
//
template <typename T_app>
void control_mov<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void control_mov<T_app>::reset()
{
	map_stop.clear();
	map_rot_front_c.clear();
	player1 = -1;
	picked1 = -1;
}
//
template <typename T_app>
void control_mov<T_app>::rebuild_player()
{
	player1 = static_cast<int>(app->m_Inst.get_index(app->m_Scene.get_misc["player1"]));
	assert(player1 > -1);
}
//
template <typename T_app>
void control_mov<T_app>::mouse_instance_move(const int &pos_x, const int &pos_y)
{
	if (player1 < 0) return;
	if (!app->m_Inst.m_Stat[player1].phy.is_touch_ground) return;
	XMVECTOR hit_pos;
	if (app->m_Inst.m_IsTerrainUse) math_mouse_hit_terrain(pos_x, pos_y, hit_pos);
	else math_mouse_hit_plane_y(pos_x, pos_y, hit_pos);
	math_mouse_move_toward_hit(hit_pos, player1);
	//
	map_stop[player1].is_stop = false;
	map_stop[player1].speed = motion.speed;
	map_stop[player1].set_aabb(hit_pos, app->m_Inst.m_BoundW.half_y(player1));
	app->m_Inst.m_Stat[player1].check_set_ClipName(motion.walk_run);
}
//
template <typename T_app>
void control_mov<T_app>::pad_instance_move_update()
{
	if (player1 < 0) return;
	if (!app->m_Inst.m_Stat[player1].phy.is_touch_ground) return;
	// walk or run
	if (pad.state.Gamepad.bRightTrigger > 50) motion.make_walk();
	else motion.make_run();
	if (pad.is_L_active()) {
		math_pad_move_toward();
	}
	else {
		app->m_Inst.m_Stat[player1].phy.velocity_nm = XMFLOAT3(0.0f, 0.0f, 0.0f);
		app->m_Inst.m_Stat[player1].check_set_ClipName(motion.idle);
	}
}
//
template <typename T_app>
void control_mov<T_app>::common_jump()
{
	if (player1 < 0) return;
	if (app->m_Inst.m_Stat[player1].phy.is_touch_ground) {
		app->m_Inst.m_Stat[player1].phy.velocity.y = motion.jump_velocity;
		//app->m_Inst.m_Stat[player1].check_set_ClipName(motion.jump);
	}
}
//
template <typename T_app>
void control_mov<T_app>::mouse_pick(const int &pos_x, const int &pos_y)
{
	app->m_Inst.m_BoundW.pick(
		pos_x,
		pos_y,
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
void control_mov<T_app>::update_scene(const float &dt)
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
void control_mov<T_app>::update_scene_bounds()
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
void control_mov<T_app>::update_stop(const float &dt)
{
	for (auto it = map_stop.begin(); it != map_stop.end(); ++it) {
		if (it->second.is_stop) continue;
		// assume bound's center approximate instance's world
		bool is_inter = it->second.contains(app->m_Inst.m_BoundW.center(it->first));
		if (is_inter) {
			it->second.is_stop = true;
			app->m_Inst.m_Stat[it->first].phy.velocity_nm = XMFLOAT3(0.0f, 0.0f, 0.0f);
			app->m_Inst.m_Stat[it->first].check_set_ClipName(motion.idle);
		}
		// adjust the direction
		else {
			it->second.dt_cd += dt;
			if (it->second.dt_cd > 0.5f) it->second.dt_cd = 0.0f;
			else continue;
			if (!app->m_Inst.m_Stat[it->first].phy.is_touch_ground) continue;
			XMVECTOR hit_pos = XMLoadFloat3(&(it->second.hit_pos));
			math_mouse_move_toward_hit(hit_pos, it->first, it->second.speed);
		}
	}
}
//
template <typename T_app>
void control_mov<T_app>::update_keydown_and_pad(const float &dt)
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
void control_mov<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if (app->m_UiMgr.on_mouse_down(btn_state, pos_x, pos_y)) return;
	if (btn_state & MOUSE_P1_PICK) {
		mouse_pick(pos_x, pos_y);
	}
	if (pad.is_enable()) return;
	// player
	if (btn_state & MOUSE_P1_MOVE) {
		mouse_instance_move(pos_x, pos_y);
	}
}
//
template <typename T_app>
void control_mov<T_app>::on_pad_down(const float &dt)
{
	WORD get_vkey;
	if (pad.is_on_keydown(get_vkey)) app->m_UiMgr.on_pad_keydown(get_vkey);
	// avoid ui conflict with control
	if (app->m_UiMgr.is_ui_appear()) wait_ui_disappear = 0.3f;
	wait_ui_disappear -= dt;
	if (wait_ui_disappear > 0.0f) return;
	wait_ui_disappear = -1.0f;
	on_pad_camera_follow(get_vkey);
	if (get_vkey == PAD_P1_JUMP) common_jump();
}
//
template <typename T_app>
void control_mov<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	app->m_UiMgr.on_input_keydown(w_param, l_param);
	if (pad.is_enable()) return;
	if (app->m_Cmd.is_active) return;
	if (w_param == KEY_P1_WALK_RUN) motion.switch_walk_run();
	if (w_param == KEY_P1_JUMP) common_jump();
}
//
template <typename T_app>
void control_mov<T_app>::on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if ((btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0)) {
		mouse_camera_move(pos_x, pos_y);
	}
	app->m_UiMgr.on_mouse_over(pos_x, pos_y);
}
//
template <typename T_app>
void control_mov<T_app>::on_mouse_wheel(const short &z_delta)
{
	if (app->m_UiMgr.on_mouse_wheel(z_delta)) return;
	mouse_camera_wheel(z_delta);
}
////////////////
// inl
////////////////
////////////////
#include "control_system_math.h"
#include "control_system_cam.h"
}
#endif