////////////////
// ui_start_rect.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_START_RECT_H
#define UI_START_RECT_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_start
////////////////
////////////////
template <class T_app>
struct ui_start: public ui_base<T_app>
{
	ui_start() {;}
	~ui_start() {;}
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
void ui_start<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, 32.0f, 2);
	m_Dwrite["24"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, 24.0f, 2);
	m_Dwrite["20_alig"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, 20.0f, 3);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.5f, "black");
	m_Brush["yellow"];
	set_Brush(D2D1::ColorF::Yellow, 0.5f, "yellow");
	m_Brush["red"];
	set_Brush(D2D1::ColorF::Red, 0.5f, "red");
	////////////////
	// 
	////////////////
	////////////////
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
bool ui_start<T_app>::define_apply_ix_if(int &index)
{
	index;
	return false;	
}
//
template <typename T_app>
void ui_start<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	m_IsPadUsing = false;
	w_param;
}
//
template <typename T_app>
void ui_start<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	m_IsPadUsing = true;
	vkey;
}
//
template <typename T_app>
void ui_start<T_app>::define_update()
{
	;
}
//
template <typename T_app>
void ui_start<T_app>::define_deactivate_all_default()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_start<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_start<T_app>::define_txt_str()
{
	;
}
//
}
#endif