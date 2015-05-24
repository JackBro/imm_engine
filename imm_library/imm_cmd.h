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
	bool is_should_be_quiet();
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
		input += L"\n> util                 Development tools.";
		input += L"\n> reload               Reload scene.";
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
	if (cmd_get == L"color") {
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
	if (cmd_get == L"slient") {
		is_slient = !is_slient;
		if (is_slient) {
			is_active = true;
			app->m_UiMgr.define_deactivate_all_cmd_slient();
		}
		else is_active = false;
		return;
	}
	if (cmd_get == L"util") {
		input += L"\n> Specify following command to use a tool:";
		input += L"\n> util b3m             Convert .m3d file to binary file.";
		input += L"\n";
		return;
	}
	if (cmd_get == L"util b3m") {
		std::wstring path_lua(GLOBAL["path_lua"].begin(), GLOBAL["path_lua"].end());
		std::wstring path_out(GLOBAL["path_out"].begin(), GLOBAL["path_out"].end());
		input += L"\n> #############################################";
		input += L"\n> [util b3m] is a tool convert .m3d file to binary file .b3m, the .m3d ";
		input += L"\n> file is a custom text file format to store meshes. For more ";
		input += L"\n> information about .m3d, see d3dcoder.net, DirectX 11 Book by Frank ";
		input += L"\n> Luna.";
		input += L"\n>";
		input += L"\n> To use this tool, you need prepare lua script tell tool which files ";
		input += L"\n> to convert, the lua script is on "+path_lua;
		input += L"cmd_util_b3m.lua, ";
		input += L"\n> it should be writen correct data according a sample from the demo.";
		input += L"\n>";
		input += L"\n> The convert process will spend a lot of time depends files size. If ";
		input += L"\n> any error occur, the program will be abort, you need figure out the ";
		input += L"\n> problem and try again.";
		input += L"\n>";
		input += L"\n> The output folder is on "+path_out;
		input += L"\n> #############################################";
		input += L"\n>";
		input += L"\n> Specify following command to use util b3m:";
		input += L"\n> util b3m start       Start converting.";
		input += L"\n";
		return;
	}
	if (cmd_get == L"util b3m start") {
		is_slient = true;
		input += L"\n> util b3m processing...";
		if (!is_busying) {
			std::thread(
			misc_util_b3m<atomic_wstring>,
			app->m_D3DDevice, std::ref(input), std::ref(is_busying)).detach();
		}
		return;
	}
	if (cmd_get.substr(0, 6) == L"reload") {
		
		if (cmd_get.size() < 9 || cmd_get.size() > 11) {
			input += L"\n> Usage: reload [scene_index]";
			input += L"\n> scene_index must be two characters.";
			input += L"\n";
		}
		else {
			std::wstring scene_ix = cmd_get.substr(7, 2);
			app->m_Scene.reload(scene_ix);
			is_active = false;
		}
		return;
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
bool cmd_shell<T_app>::is_should_be_quiet()
{
	if (is_slient) return true;
	if (is_preparing) return true;
	return false;
}
//
}
#endif