////////////////
// ui_welcome_rect.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_WELCOME_RECT_H
#define UI_WELCOME_RECT_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_welcome
////////////////
////////////////
template <class T_app>
struct ui_welcome: public ui_base<T_app>
{
	ui_welcome() {;}
	~ui_welcome() {;}
	void define_style();
	bool define_apply_ix_if(int &index);
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_on_pad_keydown(const WORD &vkey);
	void define_update();
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_txt_str();
};
//
template <typename T_app>
void ui_welcome<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 32.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["24"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 24.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["22_page"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 22.0f, DWRITE_ALIG_STYLE_PAGE);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.5f, "black");
	m_Brush["black_00a"];
	set_Brush(D2D1::ColorF::Black, 0.0f, "black_00a");
	m_Brush["yellow"];
	set_Brush(D2D1::ColorF::Yellow, 0.5f, "yellow");
	m_Brush["red"];
	set_Brush(D2D1::ColorF::Black, 0.5f, "red");
	////////////////
	// entrance
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_start";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black_00a"};
	m_Rect.back().text = L"Start Game";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.5f, 0.0f, 0.42f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_options";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black_00a"};
	m_Rect.back().text = L"Options";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.58f, 0.0f, 0.34f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_credit";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black_00a"};
	m_Rect.back().text = L"Credit";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.66f, 0.0f, 0.26f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_about";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black_00a"};
	m_Rect.back().text = L"About";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.74f, 0.0f, 0.18f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_exit";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black_00a"};
	m_Rect.back().text = L"Exit Game";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.82f, 0.0f, 0.10f);
	////////////////
	// credit
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "credit_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "credit_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.8f, 0.84f, 0.05f, 0.08f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "credit_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("credit");
	m_Rect.back().dwrite_ix = "22_page";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	// about
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "about_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "about_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.8f, 0.84f, 0.05f, 0.08f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "about_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("about");
	m_Rect.back().dwrite_ix = "22_page";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
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
}
//
template <typename T_app>
bool ui_welcome<T_app>::define_apply_ix_if(int &index)
{
	// entrance
	if (index == m_MapID["entrance_start"]) {
		// avoid pad update case scene reload conflict
		if (!m_App->m_Cmd.is_preparing) m_App->m_Scene.reload(L"01");
		return true;
	}
	if (index == m_MapID["entrance_options"]) {
		return true;
	}
	if (index == m_MapID["entrance_credit"]) {
		group_active("entrance", false);
		group_active("credit", true);
		return true;
	}
	if (index == m_MapID["entrance_about"]) {
		group_active("entrance", false);
		group_active("about", true);
		return true;
	}
	// credit
	if (index == m_MapID["credit_close"]) {
		group_active("credit", false);
		group_active("entrance", true);
		return true;
	}
	// about
	if (index == m_MapID["about_close"]) {
		group_active("about", false);
		group_active("entrance", true);
		return true;
	}
	return false;	
}
//
template <typename T_app>
void ui_welcome<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	w_param;
}
//
template <typename T_app>
void ui_welcome<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	vkey;
}
//
template <typename T_app>
void ui_welcome<T_app>::define_update()
{
	;
}
//
template <typename T_app>
void ui_welcome<T_app>::define_deactivate_all_default()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_welcome<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_welcome<T_app>::define_txt_str()
{
	// credit
	m_DefineTxt["credit"] = L"\n";
	m_DefineTxt["credit"] += L"Credit\n";
	m_DefineTxt["credit"] += L"\n";
	m_DefineTxt["credit"] += L"The fundamental 3D rendering technique is derived from the textbook ";
	m_DefineTxt["credit"] += L"Introduction to 3D Game Programming with DirectX 11 by Frank Luna.\n";
	m_DefineTxt["credit"] += L"\n";
	m_DefineTxt["credit"] += L"Sinon (Sword art Online) 3D model by Daniel Kreuter.\n";
	m_DefineTxt["credit"] += L"\n";
	m_DefineTxt["credit"] += L"Low Poly Cactus 3D model by Richard Thorogood.\n";
	m_DefineTxt["credit"] += L"\n";
	m_DefineTxt["credit"] += L"Hand-Painted Column 3D model by xrg.\n";
	m_DefineTxt["credit"] += L"\n";
	m_DefineTxt["credit"] += L"Ammo Box 3D model by mahesh.\n";
	// about
	m_DefineTxt["about"] = L"\n";
	m_DefineTxt["about"] += L"About\n";
	m_DefineTxt["about"] += L"\n";
	m_DefineTxt["about"] += L"immature engine Demo 20150424\n";
	m_DefineTxt["about"] += L"Copyright 2015 Huang Yiting (http://endrollex.com)\n";
	m_DefineTxt["about"] += L"\n";
	m_DefineTxt["about"] += L"This demo only has a menu scene, not game logic yet. ";
	m_DefineTxt["about"] += L"You can manipulate an instance, just have a look.\n";
	m_DefineTxt["about"] += L"\n";
	m_DefineTxt["about"] += L"Notice the 3D model Sinon is a fan art, this work is noncommercial use.\n";
}
//
}
#endif