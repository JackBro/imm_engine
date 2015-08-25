////////////////
// control_cam.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_CAM_H
#define CONTROL_CAM_H
#include "control_auxiliary.h"
#include <map>
namespace imm
{
////////////////
// CONTROL_STYLE_TYPE
////////////////
////////////////
enum CONTROL_STYLE_TYPE {
	CONTROL_ZERO          = 0x0,
	CONTORL_CAM_FREE      = 0x1,
	CONTORL_MOVE_BY_MOUSE = 0x2, // absence = move by WASD
};	
////////////////
// control_cam
////////////////
////////////////
template <typename T_app>
struct control_cam
{
	control_cam();
	void init(T_app *app_in);
	void reset();
	void pad_update(const float &dt);
	void on_pad_follow(const WORD &vkey);
	void key_free_update(const float &dt);
	void mouse_wheel(const short &z_delta);
	void mouse_move();
	void follow_update();
	T_app *app;
	bool is_pad_follow_reset;
	float follow_walk_def;
	float follow_up_def;
	float follow_walk;
	float follow_up;
	std::map<size_t, XMFLOAT4> map_rot_front_c;
};
//
template <typename T_app>
control_cam<T_app>::control_cam():
	app(nullptr),
	is_pad_follow_reset(false),
	follow_walk_def(-30.0f),
	follow_up_def(3.0f)
{
	follow_walk = follow_walk_def;
	follow_up = follow_up_def;
}
//
template <typename T_app>
void control_cam<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void control_cam<T_app>::reset()
{
	map_rot_front_c.clear();	
}
//
template <typename T_app>
void control_cam<T_app>::pad_update(const float &dt)
{
	if (app->m_Control.pad.is_R_active()) {
		float dx = XMConvertToRadians(50.0f*app->m_Control.pad.state.Gamepad.sThumbRX/32767*dt);
		float dy = XMConvertToRadians(50.0f*app->m_Control.pad.state.Gamepad.sThumbRY/32767*dt);
		app->m_Cam.pitch(dy);
		app->m_Cam.rotate_y(-dx);
	}
	if (app->m_UiMgr.is_ui_appear()) return;
	WORD w_buttons = app->m_Control.pad.state.Gamepad.wButtons;
	if (app->m_Control.style1 & CONTORL_CAM_FREE) {
		if (w_buttons & XGPAD_CAM_FREE_UP) app->m_Cam.up_down(10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_DOWN) app->m_Cam.up_down(-10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_LEFT) app->m_Cam.strafe(-10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_RIGHT) app->m_Cam.strafe(10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_FORWARD) app->m_Cam.walk(10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_BACKWARD) app->m_Cam.walk(-10.0f*dt);
	}
	else {
		if (w_buttons & XGPAD_CAM_FOLLOW_FORWARD) follow_walk += 10.0f*dt;
		if (w_buttons & XGPAD_CAM_FOLLOW_BACKWARD) follow_walk += -10.0f*dt;
		follow_walk = math::calc_clamp(follow_walk, -50.0f, -10.0f);
		follow_up = follow_walk * (follow_up_def/follow_walk_def);
	}
}
//
template <typename T_app>
void control_cam<T_app>::on_pad_follow(const WORD &vkey)
{
	if (app->m_Control.style1 & CONTORL_CAM_FREE) return;
	if (vkey == PAD_CAM_FOLLOW_RESET) is_pad_follow_reset = true;
}
//
template <typename T_app>
void control_cam<T_app>::key_free_update(const float &dt)
{
	if (!(app->m_Control.style1 & CONTORL_CAM_FREE)) return;
	if (GetKeyState(KEY_CAM_FREE_LEFT) & 0x8000) app->m_Cam.strafe(-10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_RIGHT) & 0x8000) app->m_Cam.strafe(10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_UP) & 0x8000) app->m_Cam.up_down(10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_DOWN) & 0x8000) app->m_Cam.up_down(-10.0f*dt);
}
//
template <typename T_app>
void control_cam<T_app>::mouse_wheel(const short &z_delta)
{
	if (app->m_Control.style1 & CONTORL_CAM_FREE) {
		app->m_Cam.walk(z_delta/120*1.0f);
	}
	else {
		follow_walk += z_delta/120*1.0f;
		follow_walk = math::calc_clamp(follow_walk, -50.0f, -10.0f);
		follow_up = follow_walk * (follow_up_def/follow_walk_def);
	}
}
//
template <typename T_app>
void control_cam<T_app>::mouse_move()
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(app->m_Control.mouse_move.x - app->m_LastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(app->m_Control.mouse_move.y - app->m_LastMousePos.y));
	app->m_Cam.pitch(dy);
	app->m_Cam.rotate_y(dx);
	// keep move continuous
	follow_update();
}
//
template <typename T_app>
void control_cam<T_app>::follow_update()
{
	if (app->m_Control.player1 < 0 || app->m_Control.style1 & CONTORL_CAM_FREE) return;
	int player1 = app->m_Control.player1;
	XMFLOAT4X4 player_world = *(app->m_Inst.m_Stat[player1].get_World());
	XMMATRIX W = XMLoadFloat4x4(&player_world);
	XMVECTOR scale, rot_quat, rot_front_conj, pos, L, U, R;
	XMMatrixDecompose(&scale, &rot_quat, &pos, W);
	// walk, up
	scale = XMVectorReplicate(follow_walk);
	L = XMLoadFloat3(&app->m_Cam.m_Look);
	pos = XMVectorMultiplyAdd(scale, L, pos);
	scale = XMVectorReplicate(follow_up);
	U = XMLoadFloat3(&app->m_Cam.m_Up);
	pos = XMVectorMultiplyAdd(scale, U, pos);
	XMStoreFloat3(&app->m_Cam.m_Position, pos);
	if (GetKeyState(KEY_CAM_FOLLOW_RESET) & 0x8000 || is_pad_follow_reset) {
		is_pad_follow_reset = false;
		// reset
		follow_walk = follow_walk_def;
		if (!map_rot_front_c.count(app->m_Control.picked1)) {
			XMMATRIX RF = XMLoadFloat4x4(app->m_Inst.m_Stat[player1].get_RotFront());
			// L as dummy
			XMMatrixDecompose(&scale, &rot_front_conj, &L, RF);
			rot_front_conj = XMQuaternionConjugate(rot_front_conj);
			XMStoreFloat4(&map_rot_front_c[player1], rot_front_conj);
		}
		else {
			rot_front_conj = XMLoadFloat4(&map_rot_front_c[player1]);
		}
		L = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		L = XMVector3Rotate(L, rot_front_conj);
		L = XMVector3Rotate(L, rot_quat);
		L = XMVector3Normalize(L);
		U = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		R = XMVector3Normalize(XMVector3Cross(U, L));
		U = XMVector3Cross(L, R);
		// pitch, rotate cam to default angle, about 0.34f
		XMMATRIX M_R = XMMatrixRotationAxis(R, 0.34f);
		U = XMVector3TransformNormal(U, M_R);
		L = XMVector3TransformNormal(L, M_R);
		XMStoreFloat3(&app->m_Cam.m_Look, L);
		XMStoreFloat3(&app->m_Cam.m_Right, R);
		XMStoreFloat3(&app->m_Cam.m_Up, U);
	}
}
//
}
#endif