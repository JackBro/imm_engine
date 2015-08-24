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
// control_cam
////////////////
////////////////
template <typename T_app>
struct control_cam
{
	control_cam();
	void init(T_app *app_in);
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
}
#endif