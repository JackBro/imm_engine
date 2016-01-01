////////////////
// imm_cmd.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef IMM_CMD_H
#define IMM_CMD_H
#include "imm_cmd_util.h"
#include "ui_dwrite.h"
#include "control_key_define.h"
#include <DirectXColors.h>
namespace imm
{
////////////////
// cmd_shell
////////////////
////////////////
template <typename T_app>
struct cmd_shell
{
	cmd_shell();
	std::atomic<bool> is_active;
	std::atomic<bool> is_slient;
	std::atomic<bool> is_busying;
	std::atomic<bool> is_preparing;
	FLOAT margin_factor;
	float font_factor;
	std::wstring cmd;
	atomic_wstring input;
	XMVECTORF32 back_color;
	T_app *app;
	dwrite_simple dwrite;
	void init(T_app *app_in);
	void on_input_char(WPARAM &w_param, LPARAM &l_param);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void apply();
	void on_resize();
	void draw();
	void do_slient_on();
	bool is_waiting_for_something();
};
//
template <typename T_app>
cmd_shell<T_app>::cmd_shell():
	is_active(false),
	is_slient(true),
	is_busying(false),
	is_preparing(true),
	margin_factor(0.018f),
	font_factor(16.0f),
	cmd(),
	input(),
	back_color(Colors::Blue),
	app(nullptr),
	dwrite()
{
	;
}
//
template <typename T_app>
void cmd_shell<T_app>::init(T_app *app_in)
{
	app = app_in;
	input.assign(L"> immature engine\n");
	dwrite.init_solo(app->m_D2DDC, app->m_hwnd, L"Consolas", margin_factor, font_factor);
}
//
template <typename T_app>
void cmd_shell<T_app>::on_input_char(WPARAM &w_param, LPARAM &l_param)
{
	if (!is_active) return;
	DUMMY(l_param);
	if (w_param != VK_BACK) {
		input += (wchar_t)w_param;
		if (w_param == VK_RETURN) apply();
		else cmd += (wchar_t)w_param;
	}
	else {
		if (input.size() > 0) input.pop_back();
		if (cmd.size() > 0) cmd.pop_back();
	}
}
template <typename T_app>
void cmd_shell<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	if (w_param == KEY_CMD_ACT1 && GetKeyState(KEY_CMD_ACT2)) {
		is_active = !is_active;
	}
}
//
template <typename T_app>
void cmd_shell<T_app>::apply()
{
	std::wstring cmd_get(cmd);
	cmd.clear();
	if (cmd_get == L"exit") {
		PostQuitMessage(0);
		return;
	}
	if (cmd_get == L"help") {
		input += L"\n> about                About this program.";
		input += L"\n> cls                  Clears the screen.";
		input += L"\n> color                Sets the console font and background colors.";
		input += L"\n> exit                 Quits this program.";
		input += L"\n> help                 Prints command briefs.";
		input += L"\n> slient               Switches between slient and normal.";
		input += L"\n> reload               Reload scene.";
		input += L"\n> draw_wire            Draw BoundingBox wireframe or not.";
		input += L"\n> set_player           Set an instance as a player.";
		input += L"\n";
		return;
	}
	if (cmd_get == L"cls") {
		input.clear();
		return;
	}
	if (cmd_get == L"about") {
		input += L"\n> immature engine Demo [Development testing 001]";
		input += L"\n> Copyright 2015 Huang Yiting (http://endrollex.com)";
		input += L"\n";
		return;
	}
	if (cmd_get.substr(0, 5) == L"color") {
		if (cmd_get.size() < 7) {
			input += L"\n> Specify following command to sets a color:";
			input += L"\n> color 0              Font White";
			input += L"\n> color 1              Font Black";
			input += L"\n> color 2              Font Purple";
			input += L"\n> color 3              Font Yellow";
			input += L"\n> color a              Background Blue";
			input += L"\n> color b              Background Black";
			input += L"\n";
			return;
		}
		if (cmd_get == L"color 0") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::White);
			return;
		}
		if (cmd_get == L"color 1") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::Black);
			return;
		}
		if (cmd_get == L"color 2") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::Purple);
			return;
		}
		if (cmd_get == L"color 3") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::Yellow);
			return;
		}
		if (cmd_get == L"color a") {
			back_color = Colors::Blue;
			return;
		}
		if (cmd_get == L"color b") {
			back_color = Colors::Black;
			return;
		}
	}
	if (cmd_get == L"slient") {
		is_slient = !is_slient;
		if (is_slient) do_slient_on();
		else is_active = false;
		return;
	}
	if (cmd_get.substr(0, 6) == L"reload") {
		if (cmd_get.size() < 8) {
			input += L"\n> Usage: reload [scene_index]";
			input += L"\n> scene_index is a string indicate which scene.";
			input += L"\n";
			return;
		}
		std::wstring scene_ix = cmd_get.substr(7);
		std::string concrete = IMM_PATH["script"]+"scene"+wstr_to_str(scene_ix)+"\\concrete_instance.lua";
		if (data_is_file_exist(concrete)) {
			app->m_Scene.reload(scene_ix);
			is_slient = false;
			is_active = false;
		}
		else {
			input += L"\n> Scene "+scene_ix+L" not found.";
			input += L"\n";
		}
		return;
	}
	if (cmd_get == L"draw_wire") {
		app->m_Scene.phy_wire.is_drawing = !app->m_Scene.phy_wire.is_drawing;
		std::wstring info_mes = L"off.";
		if (app->m_Scene.phy_wire.is_drawing) info_mes = L"on, but AABB is not displayed correctly.";
		input += L"\n> Drawing BoundingBox wireframe is "+info_mes;
		input += L"\n";
	}
	if (cmd_get.substr(0, 10) == L"set_player") {
		if (cmd_get.size() < 12) {
			input += L"\n> Usage: set_player [instance_name]";
			input += L"\n";
			return;
		}
		std::wstring inst_name_w = cmd_get.substr(11);
		std::string inst_name = wstr_to_str(inst_name_w);
		if (app->m_Inst.m_NameMap.count(inst_name)) {
			size_t inst_ix = app->m_Inst.m_NameMap[inst_name];
			app->m_Control.set_player1(inst_ix);
			input += L"\n> Now player is "+inst_name_w+L".";
			input += L"\n";
		}
		else {
			input += L"\n> Instance "+inst_name_w+L" not found.";
			input += L"\n";
		}
	}
}
//
template <typename T_app>
void cmd_shell<T_app>::on_resize()
{
	if (app == nullptr) return;
	dwrite.on_resize_CreateTextFormat(app->m_hwnd);
	dwrite.on_resize_LayoutRc(app->m_hwnd, margin_factor);
}
//
template <typename T_app>
void cmd_shell<T_app>::draw()
{
	dwrite.draw(app->m_D2DDC, input);
}
//
template <typename T_app>
void cmd_shell<T_app>::do_slient_on()
{
	is_active = true;
	app->m_UiMgr.define_deactivate_all_cmd_slient();
}
//
template <typename T_app>
bool cmd_shell<T_app>::is_waiting_for_something()
{
	if (is_slient || is_preparing) return true;
	return false;
}
//
}
#endif