////////////////
// ui_main_menu.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_MAIN_MENU_H
#define UI_MAIN_MENU_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_main_menu
////////////////
////////////////
template <class T_app>
struct ui_main_menu: public ui_base<T_app>
{
	ui_main_menu() {;}
	~ui_main_menu() {;}
	void define_style();
	bool define_apply_ix_if(int &index);
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_on_pad_keydown(const WORD &vkey);
	void define_update();
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_text();
};
//
template <typename T_app>
void ui_main_menu<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 32.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["24"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 24.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["22_page"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 22.0f, DWRITE_ALIG_STYLE_PAGE);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.5f, "black");
	m_Brush["yellow"];
	set_Brush(D2D1::ColorF::Yellow, 0.5f, "yellow");
	m_Brush["red"];
	set_Brush(D2D1::ColorF::Red, 0.5f, "red");
	////////////////
	// fps
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "fps";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "fps";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"FPS";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.85f, 0.0f, 0.0f, 0.9f);
	////////////////
	// menu
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_title";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Main Menu";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_help";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Help";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.2f, 0.3f, 0.72f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_title";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Retrun to Title";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.32f, 0.3f, 0.60f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_exit";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Exit Game";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.44f, 0.3f, 0.48f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_options";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Options";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.56f, 0.3f, 0.36f);
	////////////////
	// help
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "help_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "help";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "help_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "help";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.8f, 0.84f, 0.05f, 0.08f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "help_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "help";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("help");
	m_Rect.back().dwrite_ix = "22_page";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	// exit
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.15f, 0.0f, 0.15f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_yes";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Yes";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.25f, 0.5f, 0.6f, 0.42f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_no";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"No";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.6f, 0.5f, 0.25f, 0.42f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_title";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Exit Game?";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
	////////////////
	//
	////////////////
	////////////////
	build_rect_map();
	//
	for (auto &rect: m_Rect) {
		rect.brush_ix = rect.brush_sel[0];
		if (rect.tp == ui_rect::type::button) rect.brush_sel.emplace_back("red");
		rect.active = false;
	}
	m_Rect[m_MapGroup["fps"][0]].active = true;
}
//
template <typename T_app>
bool ui_main_menu<T_app>::define_apply_ix_if(int &index)
{
	// menu
	if (index == m_MapID["menu_help"]) {
		group_active("menu", false);
		group_active("help", true);
		return true;
	}
	if (index == m_MapID["menu_title"]) {
		m_App->m_Scene.reload(L"00");
		return true;
	}
	if (index == m_MapID["menu_exit"]) {
		group_active("menu", false);
		group_active("exit", true);
		return true;
	}
	if (index == m_MapID["menu_options"]) {
		return true;
	}	
	// exit
	if (index == m_MapID["exit_yes"]) {
		PostQuitMessage(0);
		return true;
	}
	if (index == m_MapID["exit_no"]) {
		group_active("exit", false);
		return true;
	}
	// help
	if (index == m_MapID["help_close"]) {
		group_active("help", false);
		return true;
	}
	return false;	
}
//
template <typename T_app>
void ui_main_menu<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	if (w_param == KEY_UI_ESC) {
		if (is_ui_appear()) define_deactivate_all_default();
		else group_active("menu", true);
		return;
	}
	if (w_param == KEY_UI_FPS1 && GetKeyState(KEY_UI_FPS2)) {
		group_active_switch("fps");
		return;
	}
}
//
template <typename T_app>
void ui_main_menu<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	if (vkey == PAD_UI_MENU) {
		group_active_switch("menu");
		return;
	}
	if (vkey == PAD_UI_DEACTIVATE) {
		define_deactivate_all_default();
		return;
	}
}
//
template <typename T_app>
void ui_main_menu<T_app>::define_update()
{
	m_Rect[m_MapGroup["fps"][0]].text = m_App->m_Fps;
}
//
template <typename T_app>
void ui_main_menu<T_app>::define_deactivate_all_default()
{
	bool fps = m_Rect[m_MapGroup["fps"][0]].active;
	deactivate_all();
	if (fps) group_active_switch("fps");
}
//
template <typename T_app>
void ui_main_menu<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
	group_active("fps", true);
}
////////////////
// define_txt_str
////////////////
////////////////
template <typename T_app>
void ui_main_menu<T_app>::define_text()
{
	std::map<std::string, std::string> get_welcome;
	std::string describe = GLOBAL["path_txt"]+"main_menu.lua";
	lua_reader l_reader;
	l_reader.loadfile(describe);
	std::string var_name = "main_menu";
	if (!l_reader.is_not_nil(var_name)) {assert(false); abort();}
	l_reader.map_from_table(get_welcome, var_name);
	for (auto it = get_welcome.begin(); it != get_welcome.end(); ++it) {
		m_DefineTxt[it->first] = str_to_wstr(it->second);
	}
}}
#endif