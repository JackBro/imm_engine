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
	set_Brush(D2D1::ColorF::Black, 0.0f, "black");
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
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Start Game";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.5f, 0.0f, 0.42f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_options";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Options";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.58f, 0.0f, 0.34f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_credit";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Credit";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.66f, 0.0f, 0.26f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_about";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"About";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.74f, 0.0f, 0.18f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "entrance_exit";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Exit Game";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.82f, 0.0f, 0.10f);
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
	index;
	if (index == m_MapID["entrance_start"]) {
		m_App->m_Scene.reload(L"01");
		return true;
	}
	if (index == m_MapID["entrance_options"]) {
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
	;
}
//
}
#endif