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
// GetKeyState                (in function suffix _update / combination key)
// WM_KEYDOWN / WM_SYSKEYDOWN (in function suffix _keydown)
//
// Virtual-Key Codes:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
////////////////
////////////////
static int KEY_P1_JUMP          = VK_SPACE;
static int KEY_P1_WALK_RUN      = VK_SHIFT;
static int KEY_P1_W             = 'W';
static int KEY_P1_A             = 'A';
static int KEY_P1_S             = 'S';
static int KEY_P1_D             = 'D';
static int KEY_UI_ESC           = VK_ESCAPE;
static int KEY_UI_FPS1          = 'R';
static int KEY_UI_FPS2          = VK_CONTROL;
static int KEY_UI_DOWN1         = VK_DOWN;
static int KEY_UI_DOWN2         = VK_RIGHT;
static int KEY_UI_UP1           = VK_UP;
static int KEY_UI_UP2           = VK_LEFT;
static int KEY_UI_ENTER         = VK_RETURN;
static int KEY_UI_PAGEDOWN      = VK_NEXT;
static int KEY_UI_PAGEUP        = VK_PRIOR;
static int KEY_CAM_FREE_LEFT    = 'A';
static int KEY_CAM_FREE_RIGHT   = 'D';
static int KEY_CAM_FREE_UP      = 'W';
static int KEY_CAM_FREE_DOWN    = 'S';
static int KEY_CAM_FOLLOW_RESET = 'Z';
static int KEY_CMD_ACT1         = VK_DELETE;
static int KEY_CMD_ACT2         = VK_MENU;
////////////////
// mouse static
////////////////
////////////////
// CONTORL_MOVE_BY_MOUSE
static int MOUSE_CAM_MOVE = MK_MBUTTON;
static int MOUSE_P1_PICK  = MK_RBUTTON;
static int MOUSE_P1_MOVE  = MK_LBUTTON;
static int MOUSE_UI_PICK  = MK_LBUTTON;
// CONTORL_MOVE_BY_WASD
static int MOUSE_P1_ATK_X = MK_LBUTTON;
////////////////
// pad static
//
// two methods:
// XINPUT_STATE structure     (in function suffix _update)
// XINPUT_KEYSTROKE structure (in function suffix _down)
////////////////
////////////////
static int PAD_P1_JUMP               = VK_PAD_A;
static int PAD_P1_ATK_X              = VK_PAD_X;
static int PAD_CAM_FOLLOW_RESET      = VK_PAD_RSHOULDER;
static int PAD_UI_MENU               = VK_PAD_START;
static int PAD_UI_DWON1              = VK_PAD_DPAD_DOWN;
static int PAD_UI_DWON2              = VK_PAD_DPAD_RIGHT;
static int PAD_UI_UP1                = VK_PAD_DPAD_UP;
static int PAD_UI_UP2                = VK_PAD_DPAD_LEFT;
static int PAD_UI_DEACTIVATE         = VK_PAD_B;
static int PAD_UI_APPLY              = VK_PAD_A;
static int XGPAD_CAM_FREE_UP         = XINPUT_GAMEPAD_DPAD_UP;
static int XGPAD_CAM_FREE_DOWN       = XINPUT_GAMEPAD_DPAD_DOWN;
static int XGPAD_CAM_FREE_LEFT       = XINPUT_GAMEPAD_DPAD_LEFT;
static int XGPAD_CAM_FREE_RIGHT      = XINPUT_GAMEPAD_DPAD_RIGHT;
static int XGPAD_CAM_FREE_FORWARD    = XINPUT_GAMEPAD_LEFT_SHOULDER;
static int XGPAD_CAM_FREE_BACKWARD   = XINPUT_GAMEPAD_RIGHT_SHOULDER;
static int XGPAD_CAM_FOLLOW_FORWARD  = XINPUT_GAMEPAD_LEFT_THUMB;
static int XGPAD_CAM_FOLLOW_BACKWARD = XINPUT_GAMEPAD_RIGHT_THUMB;
////////////////
// control coefficient
////////////////
////////////////
static float COEF_MOUSE_MOVE  = 1.0f;
static float COEF_MOUSE_WHEEL = 1.0f;
static float COEF_PAD_THUMB   = 1.0f;
////////////////
// MOUSE_MODEL
////////////////
////////////////
enum MOUSE_MODEL
{
	CAM_MOVE_BY_BUTTON,
	HIDDEN_CURSOR,
};
//
}
#endif