////////////////
// ui_def_status.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_DEF_STATUS_H
#define UI_DEF_STATUS_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_def_status
////////////////
////////////////
template <class T_app>
struct ui_def_status: public ui_base<T_app>
{
	ui_def_status() {;}
	~ui_def_status() {;}
	void define_style();
	bool define_apply_ix_if(int &index);
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_on_pad_keydown(const WORD &vkey);
	void define_update(float dt);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_text();
};
//
template <typename T_app>
void ui_def_status<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 32.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["24"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 24.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["22_page"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 22.0f, DWRITE_ALIG_STYLE_PAGE);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.7f, "black");
	m_Brush["yellow"];
	set_Brush(D2D1::ColorF::Orange, 0.7f, "yellow");
	m_Brush["red"];
	set_Brush(D2D1::ColorF::DarkRed, 0.7f, "red");
	m_Brush["transparent"];
	set_Brush(D2D1::ColorF::Black, 0.0f, "transparent");
	////////////////
	// hp
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "hp";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "hp";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.85f, 0.0f, 0.0f, 0.9f);
	////////////////
	//
	////////////////
	////////////////
	build_rect_map();
	//
	for (auto &rect: m_Rect) {
		rect.brush_ix = rect.brush_sel[0];
		if (rect.tp == ui_rect::type::button) {
			rect.brush_sel.emplace_back("red");
		}
		rect.active = false;
	}
	m_Rect[m_MapGroup["hp"][0]].active = true;
}
//
template <typename T_app>
bool ui_def_status<T_app>::define_apply_ix_if(int &index)
{
	index;
	return false;
}
//
template <typename T_app>
void ui_def_status<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	w_param;
	l_param;
}
//
template <typename T_app>
void ui_def_status<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	vkey;
}
//
template <typename T_app>
void ui_def_status<T_app>::define_update(float dt)
{
	DUMMY(dt);
}
//
template <typename T_app>
void ui_def_status<T_app>::define_deactivate_all_default()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_def_status<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_def_status<T_app>::define_text()
{
	;
}
//
}
#endif