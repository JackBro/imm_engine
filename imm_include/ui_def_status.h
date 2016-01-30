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
	ui_def_status();
	~ui_def_status() {;}
	void define_style();
	bool define_apply_ix_if(int &index);
	bool define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	bool define_on_pad_keydown(const WORD &vkey);
	void define_update(float dt);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_enter_and_exit() {;}
	void define_show(const bool &is_show);
	void define_text();
	bool m_IsActive;
	bool m_IsTarShow;
};
//
template <typename T_app>
ui_def_status<T_app>::ui_def_status():
	m_IsActive(true),
	m_IsTarShow(false)
{
	;
}
//
template <typename T_app>
void ui_def_status<T_app>::define_style()
{
	m_Dwrite["hp_txt"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 10.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["tar_name"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 24.0f, DWRITE_ALIG_STYLE_CENTER);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.3f, "black");
	m_Brush["hp_color"];
	set_Brush(D2D1::ColorF::Red, 1.0f, "hp_color");
	m_Brush["mp_color"];
	set_Brush(D2D1::ColorF::Blue, 1.0f, "mp_color");
	////////////////
	// hp
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "hp_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "hp";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.7f, 0.93f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "hp_rect";
	m_Rect.back().parent_str = "hp_backg";
	m_Rect.back().group = "no_draw";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"hp_color"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.175f, 0.1f, 0.475f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "mp_rect";
	m_Rect.back().parent_str = "hp_backg";
	m_Rect.back().group = "no_draw";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"mp_color"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.675f, 0.1f, 0.175f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "hp_bar";
	m_Rect.back().parent_str = "hp_rect";
	m_Rect.back().group = "hp";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"hp_color"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "mp_bar";
	m_Rect.back().parent_str = "mp_rect";
	m_Rect.back().group = "hp";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"mp_color"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "hp_txt";
	m_Rect.back().parent_str = "hp_rect";
	m_Rect.back().group = "hp";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"HP";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "mp_txt";
	m_Rect.back().parent_str = "mp_rect";
	m_Rect.back().group = "hp";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"MP";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
	////////////////
	// target
	////////////////
	////////////////	
	m_Rect.emplace_back();
	m_Rect.back().id_str = "tar_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "tar";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.93f, 0.7f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "tar_hp_rect";
	m_Rect.back().parent_str = "tar_backg";
	m_Rect.back().group = "no_draw";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"hp_color"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.475f, 0.1f, 0.175f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "tar_hp_bar";
	m_Rect.back().parent_str = "tar_hp_rect";
	m_Rect.back().group = "tar";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"hp_color"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "hp_txt";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "tar_name";
	m_Rect.back().parent_str = "tar_backg";
	m_Rect.back().group = "tar";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Enemy";
	m_Rect.back().dwrite_ix = "tar_name";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
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
}
//
template <typename T_app>
bool ui_def_status<T_app>::define_apply_ix_if(int &index)
{
	DUMMY(index);
	return false;
}
//
template <typename T_app>
bool ui_def_status<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(w_param);
	DUMMY(l_param);
	return false;
}
//
template <typename T_app>
bool ui_def_status<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	DUMMY(vkey);
	return false;
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
	if (!m_IsActive) return;
	deactivate_all();
}
//
template <typename T_app>
void ui_def_status<T_app>::define_deactivate_all_cmd_slient()
{
	if (!m_IsActive) return;
	deactivate_all();
	group_active("hp", true);
	if (m_IsTarShow) group_active("tar", true);
}
//
template <typename T_app>
void ui_def_status<T_app>::define_show(const bool &is_show)
{
	if (!m_IsActive) return;
	group_active("hp", is_show);
	group_active("tar", m_IsTarShow && is_show);
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