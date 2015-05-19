////////////////
// control_mov.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_MOV_H
#define CONTROL_MOV_H
#include "stru_instance_mgr.h"
#include "imm_camera.h"
#include "phy_prepare.h"
#include "XInput.h"
namespace imm
{
////////////////
// control style
////////////////
////////////////
static const int CONTORL_CAM_FREE = 128;
static const int CONTORL_MOVE_BY_MOUSE = 64;
////////////////
// control_xinput
////////////////
////////////////
struct control_xinput
{
	control_xinput();
	bool is_enable();
	bool is_L_active();
	bool is_R_active();
	bool is_on_keydown(WORD &vkey);
	float L_radians();
	float R_radians();
	bool is_force_disable;
	XINPUT_STATE state;
	XINPUT_KEYSTROKE key;
	float deadzone;
};
//
control_xinput::control_xinput():
	deadzone(5000.0f),
	is_force_disable(false)
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	key.UserIndex = 0;
}
//
bool control_xinput::is_enable()
{
	if (is_force_disable) return false;
	DWORD dw_result;
	dw_result = XInputGetState(0, &state);
	if (dw_result == ERROR_SUCCESS) return true;
	return false;
}
//
bool control_xinput::is_L_active()
{
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;
	float magnitude = sqrt(LX*LX + LY*LY);
	if (magnitude > deadzone) return true;
	return false;
}
//
bool control_xinput::is_R_active()
{
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;
	float magnitude = sqrt(RX*RX + RY*RY);
	if (magnitude > deadzone) return true;
	return false;
}
//
bool control_xinput::is_on_keydown(WORD &vkey)
{
	if (XInputGetKeystroke(0, 0, &key) == ERROR_SUCCESS) {
		if (key.Flags & XINPUT_KEYSTROKE_KEYDOWN) {
			vkey = key.VirtualKey;
			return true;
		}
	}
	return false;
}
//
float control_xinput::L_radians()
{
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;
	return atan2(LX, LY);
}
//
float control_xinput::R_radians()
{
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;
	return atan2(RX, RY);
}
////////////////
// control_stop
////////////////
////////////////
struct control_stop
{
	control_stop();
	void set_aabb(CXMVECTOR &pos, const float &half_y);
	bool contains(const XMFLOAT3 &center);
	bool is_stop;
	BoundingBox bbox;
	BoundingBox bbox_inst;
	XMFLOAT3 plane_pos;
	float dt_cd;
	float speed;
};
//
control_stop::control_stop():
	is_stop(true),
	plane_pos(0.0f, 0.0f, 0.0f),
	dt_cd(0.0f),
	speed(0.0f)
{
	;
}
//
void control_stop::set_aabb(CXMVECTOR &pos, const float &half_y)
{
	XMStoreFloat3(&bbox.Center, pos);
	bbox.Extents = XMFLOAT3(0.2f, half_y*2.0f, 0.2f);
	plane_pos = bbox.Center;
}
//
bool control_stop::contains(const XMFLOAT3 &center)
{
	XMVECTOR C = XMLoadFloat3(&center);
	if (bbox.Contains(C) == 2) return true;
	else return false;
}
////////////////
// control_motion
////////////////
////////////////
struct control_motion
{
	control_motion();
	std::string walk_run;
	std::string idle;
	std::string jump;
	float speed;
	float speed_walk;
	float speed_run;
	float jump_velocity;
	void switch_walk_run();
	void make_run();
	void make_walk();
};
//
control_motion::control_motion():
	walk_run("Run"),
	idle("Idle"),
	jump("Jump"),
	speed(13.5f),
	speed_walk(4.5f),
	speed_run(13.5),
	jump_velocity(15.0f)
{
	;
}
//
void control_motion::switch_walk_run()
{
	if (walk_run == "Walk") {
		walk_run = "Run";
		speed = speed_run;
	}
	else {
		walk_run = "Walk";
		speed = speed_walk;
	}
}
//
void control_motion::make_run()
{
	if (walk_run != "Run") {
		walk_run = "Run";
		speed = speed_run;
	}
}
//
void control_motion::make_walk()
{
	if (walk_run != "Walk") {
		walk_run = "Walk";
		speed = speed_walk;
	}
}
////////////////
// control_mov
////////////////
////////////////
template <typename T_app>
struct control_mov
{
	control_mov();
	void init(T_app *app_in);
	void remove();
	void update_loading_stat();
	void mouse_instance_move(const int &pos_x, const int &pos_y);
	void pad_instance_move();
	void mouse_move_toward_hit(CXMVECTOR &plane_pos, const size_t &index, const float &speed = -1.0f);
	void mouse_face_rot_y(XMMATRIX &W, XMMATRIX &RF, CXMVECTOR &direction);
	void pad_face_rot_y(XMMATRIX &W, XMMATRIX &RF, XMVECTOR &direction, const float &rot_cam);
	void mouse_hit_plane_y0(const int &pos_x, const int &pos_y, XMVECTOR &plane_pos);
	void pad_move_toward();
	void common_jump();
	void mouse_pick(const int &pos_x, const int &pos_y);
	void update_scene_instance(const float &dt);
	void update_scene_stop(const float &dt);
	void update_keydown_and_pad(const float &dt);
	void on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_pad_down(const float &dt);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_mouse_wheel(const short &z_delta);
	// include
	void pad_camera_free(const float &dt);
	void on_pad_camera_follow(const WORD &vkey);
	void key_camera_free(const float &dt);
	void mouse_camera_wheel(const short &z_delta);
	void mouse_camera_move(const int &pos_x, const int &pos_y);
	void cam_follow_update();
	//
	T_app *app;
	int picked1;
	int player1;
	int style_p1;
	bool is_pad_cam_follow_reset;
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
	cam_follow_walk(-30.0f),
	cam_follow_up(4.0f),
	wait_ui_disappear(0.0f),
	pad()
{
	;
}
//
template <typename T_app>
void control_mov<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void control_mov<T_app>::remove()
{
	map_stop.clear();
	map_rot_front_c.clear();
	player1 = -1;
	picked1 = -1;
}
//
template <typename T_app>
void control_mov<T_app>::update_loading_stat()
{
	if (player1 == -1 && !app->m_Inst.m_IsLoading) {
		player1 = static_cast<int>(app->m_Inst.get_index(app->m_Scene.misc_info["player1"]));
	}
	if (app->m_Inst.m_IsLoading && player1 != -1) {
		remove();
	}
}
//
template <typename T_app>
void control_mov<T_app>::mouse_instance_move(const int &pos_x, const int &pos_y)
{
	if (picked1 == -1) return;
	if (!app->m_Inst.m_Stat[picked1].phy.is_touch_ground) return;
	XMVECTOR plane_pos;
	mouse_hit_plane_y0(pos_x, pos_y, plane_pos);
	mouse_move_toward_hit(plane_pos, picked1);
	//
	map_stop[picked1].is_stop = false;
	map_stop[picked1].speed = motion.speed;
	map_stop[picked1].set_aabb(plane_pos, app->m_Inst.m_BoundW.half_y(picked1));
	app->m_Inst.m_Stat[picked1].check_set_ClipName(motion.walk_run);
}
//
template <typename T_app>
void control_mov<T_app>::pad_instance_move()
{
	if (picked1 == -1) return;
	if (!app->m_Inst.m_Stat[picked1].phy.is_touch_ground) return;
	// walk or run
	if (pad.state.Gamepad.bRightTrigger > 50) motion.make_walk();
	else motion.make_run();
	if (pad.is_L_active()) {
		pad_move_toward();
	}
	else {
		app->m_Inst.m_Stat[picked1].phy.velocity_nm = XMFLOAT3(0.0f, 0.0f, 0.0f);
		app->m_Inst.m_Stat[picked1].check_set_ClipName(motion.idle);
	}
}
//
template <typename T_app>
void control_mov<T_app>::mouse_move_toward_hit(CXMVECTOR &plane_pos, const size_t &index, const float &speed = -1.0f)
{
	XMFLOAT4X4 &world = *app->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &rot_front = *app->m_Inst.m_Stat[index].get_RotFront();
	XMFLOAT3 *phy_velocity_nm = &app->m_Inst.m_Stat[index].phy.velocity_nm;
	XMVECTOR velocity_nm = XMLoadFloat3(phy_velocity_nm);
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	velocity_nm = XMVectorSubtract(plane_pos, W.r[3]);
	velocity_nm = XMVectorSetY(velocity_nm, 0.0f);
	velocity_nm = XMVector3Normalize(velocity_nm);
	// use velocity_nm as front direction
	mouse_face_rot_y(W, RF, velocity_nm);
	// when update stop, use object own speed
	if (speed < 0.0f) velocity_nm = XMVectorScale(velocity_nm, motion.speed);
	else velocity_nm = XMVectorScale(velocity_nm, speed);
	XMStoreFloat3(phy_velocity_nm, velocity_nm);
	XMStoreFloat4x4(&world, W);
}
//
template <typename T_app>
void control_mov<T_app>::mouse_face_rot_y(XMMATRIX &W, XMMATRIX &RF, CXMVECTOR &direction)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	float rot_radians = atan2(-XMVectorGetX(direction), -XMVectorGetZ(direction));
	R = XMMatrixRotationY(rot_radians);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
template <typename T_app>
void control_mov<T_app>::pad_face_rot_y(XMMATRIX &W, XMMATRIX &RF, XMVECTOR &direction, const float &rot_cam)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	// rot_quat uses as temp var
	float rot_pad = pad.L_radians();
	rot_quat =  XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rot_pad);
	direction = XMVector3Rotate(direction, rot_quat);
	//
	R = XMMatrixRotationY(rot_cam);
	R = XMMatrixMultiply(XMMatrixRotationY(rot_pad), R);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
template <typename T_app>
void control_mov<T_app>::mouse_hit_plane_y0(const int &pos_x, const int &pos_y, XMVECTOR &plane_pos)
{
	CXMMATRIX cam_proj = app->m_Cam.get_Proj();
	CXMMATRIX cam_view = app->m_Cam.get_View();
	// Compute picking ray in view space.
	float vx = (+2.0f*pos_x/app->m_ClientWidth - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*pos_y/app->m_ClientHeight + 1.0f)/cam_proj.r[1].m128_f32[1];
	// Ray definition in view space.
	XMVECTOR ray_origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR ray_dir = XMVectorSet(vx, vy, 1.0f, 0.0f);
	// Tranform ray to world.
	XMVECTOR det_view = XMMatrixDeterminant(cam_view);
	XMMATRIX inv_view = XMMatrixInverse(&det_view, cam_view);
	ray_origin = XMVector3TransformCoord(ray_origin, inv_view);
	ray_dir = XMVector3TransformNormal(ray_dir, inv_view);
	// Make the ray direction unit length for the intersection tests.
	ray_dir = XMVector3Normalize(ray_dir);
	// Ray hit y=value plane at plane_p
	float plane_y_value = 0.0f;
	plane_pos = ray_origin;
	float ratio_y = -XMVectorGetY(ray_origin)/XMVectorGetY(ray_dir)+plane_y_value;
	plane_pos = XMVectorSetY(plane_pos, plane_y_value);
	plane_pos = XMVectorSetX(plane_pos, XMVectorGetX(plane_pos)+ratio_y*XMVectorGetX(ray_dir));
	plane_pos = XMVectorSetZ(plane_pos, XMVectorGetZ(plane_pos)+ratio_y*XMVectorGetZ(ray_dir));
}
//
template <typename T_app>
void control_mov<T_app>::pad_move_toward()
{
	XMVECTOR velocity_nm = app->m_Cam.get_LookXM();
	float cam_radians = atan2(-XMVectorGetX(velocity_nm), -XMVectorGetZ(velocity_nm));
	XMFLOAT4X4 &world = *(app->m_Inst.m_Stat[picked1].get_World());
	XMFLOAT4X4 &rot_front = *(app->m_Inst.m_Stat[picked1].get_RotFront());
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	pad_face_rot_y(W, RF, velocity_nm, cam_radians);
	velocity_nm = XMVectorSetY(velocity_nm, 0.0f);
	velocity_nm = XMVector3Normalize(velocity_nm);
	velocity_nm = XMVectorScale(velocity_nm, motion.speed);
	XMStoreFloat4x4(&world, W);
	XMStoreFloat3(&app->m_Inst.m_Stat[picked1].phy.velocity_nm, velocity_nm);
	app->m_Inst.m_Stat[picked1].check_set_ClipName(motion.walk_run);
}
//
template <typename T_app>
void control_mov<T_app>::common_jump()
{
	if (picked1 == -1) return;
	if (app->m_Inst.m_Stat[picked1].phy.is_touch_ground)
		app->m_Inst.m_Stat[picked1].phy.velocity.y = motion.jump_velocity;
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
}
//
template <typename T_app>
void control_mov<T_app>::update_scene_instance(const float &dt)
{
	app->m_Inst.update_skinned(dt);
	app->m_Inst.bound_update();
	app->m_Inst.collision_update(dt);
	update_loading_stat();
	update_scene_stop(dt);
	cam_follow_update();
}
//
template <typename T_app>
void control_mov<T_app>::update_scene_stop(const float &dt)
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
			XMVECTOR plane_pos = XMLoadFloat3(&(it->second.plane_pos));
			mouse_move_toward_hit(plane_pos, it->first, it->second.speed);
		}
	}
}
//
template <typename T_app>
void control_mov<T_app>::update_keydown_and_pad(const float &dt)
{
	DUMMY(dt);
	if (pad.is_enable()) {
		picked1 = player1;
		on_pad_down(dt);
	}
	// keyborad
	else {
		if (picked1 < 0) picked1 = player1;
		if (picked1 == app->m_Inst.m_SceneGroundIx) picked1 = player1;
		if (GetKeyState(VK_SPACE) & 0x8000) common_jump();
	}
	// camera
	key_camera_free(dt);
}
//
template <typename T_app>
void control_mov<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if (btn_state & MK_RBUTTON) {
		mouse_pick(pos_x, pos_y);
	}
	if (pad.is_enable()) return;
	// player
	if (btn_state & MK_LBUTTON) {
		mouse_instance_move(pos_x, pos_y);
	}
}
//
template <typename T_app>
void control_mov<T_app>::on_pad_down(const float &dt)
{
	pad_instance_move();
	pad_camera_free(dt);
	//
	WORD get_vkey;
	if (pad.is_on_keydown(get_vkey)) app->m_UI.define_on_pad_keydown(get_vkey, dt);
	// avoid ui conflict with control
	if (app->m_UI.is_ui_appear()) wait_ui_disappear = 0.3f;
	wait_ui_disappear -= dt;
	if (wait_ui_disappear > 0.0f) return;
	wait_ui_disappear = -1.0f;
	on_pad_camera_follow(get_vkey);
	if (get_vkey == VK_PAD_A) common_jump();
}
//
template <typename T_app>
void control_mov<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	if (pad.is_enable()) return;
	if (w_param == VK_SHIFT) motion.switch_walk_run();
}
//
template <typename T_app>
void control_mov<T_app>::on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if ((btn_state & MK_MBUTTON)) {
		mouse_camera_move(pos_x, pos_y);
	}
}
//
template <typename T_app>
void control_mov<T_app>::on_mouse_wheel(const short &z_delta)
{
	mouse_camera_wheel(z_delta);
}
////////////////
// control_mov_cam.h
////////////////
////////////////
#include "control_mov_cam.h"
}
#endif