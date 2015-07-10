////////////////
// control_key_define.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_KEY_DEFINE_H
#define CONTROL_KEY_DEFINE_H
// include "Windows.h" before "XInput.h"
#include "Windows.h"
#include "XInput.h"
namespace imm
{
////////////////
// keyborad static
//
// two methods:
// GetKeyState                  (in function suffix _update / combination key)
// WM_KEYDOWN / WM_SYSKEYDOWN   (in function suffix _keydown)
//
// Virtual-Key Codes:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
////////////////
////////////////
static int KEY_P1_JUMP = VK_SPACE;
static int KEY_P1_WALK_RUN = VK_SHIFT;
static int KEY_UI_ESC = VK_ESCAPE;
static int KEY_UI_FPS1 = 'R';
static int KEY_UI_FPS2 = VK_CONTROL;
static int KEY_CAM_FREE_LEFT = 'A';
static int KEY_CAM_FREE_RIGHT = 'D';
static int KEY_CAM_FREE_UP = 'W';
static int KEY_CAM_FREE_DOWN = 'S';
static int KEY_CAM_FOLLOW_RESET = 'Z';
static int KEY_CMD_ACT1 = VK_DELETE;
static int KEY_CMD_ACT2 = VK_MENU;
// WASD
static int KEY_P1_W = 'W';
static int KEY_P1_A = 'A';
static int KEY_P1_S = 'S';
static int KEY_P1_D = 'D';
////////////////
// mouse static
////////////////
////////////////
static int MOUSE_CAM_MOVE = MK_MBUTTON;
static int MOUSE_P1_PICK = MK_RBUTTON;
static int MOUSE_P1_MOVE = MK_LBUTTON;
static int MOUSE_UI_PICK = MK_LBUTTON;
////////////////
// pad static
//
// two methods:
// XINPUT_STATE structure       (in function suffix _update)
// XINPUT_KEYSTROKE structure   (in function suffix _down)
////////////////
////////////////
static int PAD_P1_JUMP = VK_PAD_A;
static int PAD_CAM_FOLLOW_RESET = VK_PAD_RSHOULDER;
static int PAD_UI_MENU = VK_PAD_START;
static int PAD_UI_DWON1 = VK_PAD_DPAD_DOWN;
static int PAD_UI_DWON2 = VK_PAD_DPAD_RIGHT;
static int PAD_UI_UP1 = VK_PAD_DPAD_UP;
static int PAD_UI_UP2 = VK_PAD_DPAD_LEFT;
static int PAD_UI_DEACTIVATE = VK_PAD_B;
static int PAD_UI_APPLY = VK_PAD_A;
static int XGPAD_CAM_FREE_UP = XINPUT_GAMEPAD_DPAD_UP;
static int XGPAD_CAM_FREE_DOWN = XINPUT_GAMEPAD_DPAD_DOWN;
static int XGPAD_CAM_FREE_LEFT = XINPUT_GAMEPAD_DPAD_LEFT;
static int XGPAD_CAM_FREE_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT;
static int XGPAD_CAM_FREE_FORWARD = XINPUT_GAMEPAD_LEFT_SHOULDER;
static int XGPAD_CAM_FREE_BACKWARD = XINPUT_GAMEPAD_RIGHT_SHOULDER;
static int XGPAD_CAM_FOLLOW_FORWARD = XINPUT_GAMEPAD_RIGHT_THUMB;
static int XGPAD_CAM_FOLLOW_BACKWARD = XINPUT_GAMEPAD_LEFT_THUMB;
////////////////
// control coefficient
////////////////
////////////////
static float COEF_MOUSE_MOVE = 1.0f;
static float COEF_MOUSE_WHEEL = 1.0f;
static float COEF_PAD_THUMB = 1.0f;
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
	XMFLOAT3 hit_pos;
	float dt_cd;
	float speed;
	float bbox_half_y;
};
//
control_stop::control_stop():
	is_stop(true),
	hit_pos(0.0f, 0.0f, 0.0f),
	dt_cd(0.0f),
	speed(0.0f),
	bbox_half_y(100.0f)
{
	;
}
//
void control_stop::set_aabb(CXMVECTOR &pos, const float &half_y)
{
	XMStoreFloat3(&bbox.Center, pos);
	bbox.Extents = XMFLOAT3(0.2f, half_y*2.0f, 0.2f);
	hit_pos = bbox.Center;
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
	jump_velocity(20.0f)
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
//
}
#endif