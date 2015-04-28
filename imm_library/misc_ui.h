////////////////
// misc_ui.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef MISC_UI_H
#define MISC_UI_H
#include "misc_dwrite.h"
#include "XInput.h"
#include <algorithm>
namespace imm
{
////////////////
// ui_rect
////////////////
////////////////
struct ui_rect
{
	enum type {
		background	= 0,
		button		= 1,
		text_pure	= 2,
		text_layout	= 3
	};
	type tp;
	std::string brush_ix;
	std::vector<std::string> brush_sel;
	D2D1_RECT_F rc;
	std::wstring text;
	std::string id_str;
	std::string parent_str;
	std::string group;
	std::string dwrite_ix;
	XMFLOAT4 margin;
	int parent;
	bool active;
};
////////////////
// ui_simple
////////////////
////////////////
template <typename T_app>
struct ui_simple
{
	ui_simple();
	~ui_simple();
	void init(T_app *app_in);
	void build_rect_map();
	void set_Brush(D2D1::ColorF::Enum color, const float &opacity, const std::string index);
	void on_resize_RectFromHWND(const size_t &ix);
	void on_resize_RectFromRect(const size_t &ix, const int &parent);
	void on_resize_TextLayout(const size_t &ix, const bool &is_resize = false);
	PCWSTR get_DefineTxt(const std::string &name);
	void on_resize();
	void draw();
	bool on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	bool on_mouse_wheel(const short &z_delta);
	void on_mouse_over(const int &pos_x, const int &pos_y);
	void mouse_pick(const int &pos_x, const int &pos_y);
	void group_active_switch(const std::string &name);
	void group_active(const std::string &name, const bool &is_act);
	void pad_loop_button(const bool &is_down, const std::string &select_none = "");
	void pad_roll_text_layout(const bool &is_down);
	bool is_ui_appear();
	void deactivate_all();
	//
	void define_style();
	bool define_pick_lb(const int &pos_x, const int &pos_y);
	bool define_apply_ix(int &index);
	bool define_apply_ix_if(int &index);
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_pad_on_keydown(const WORD &vkey, const float &dt);
	void define_update();
	void define_deactivate_all_default();
	void define_txt_str();
	std::map<std::string, ID2D1SolidColorBrush*> m_Brush;
	std::vector<ui_rect> m_Rect;
	std::map<std::string, std::vector<size_t>> m_MapGroup;
	std::map<std::string, std::vector<size_t>> m_MapButton;
	std::map<std::string, std::vector<size_t>> m_MapTextLayout;
	std::map<std::string, int> m_MapID;
	std::map<std::string, dwrite_simple> m_Dwrite;
	int m_ClickIx;
	int m_ClickIxPad;
	D2D1_POINT_2F M_TextLayoutOrigin;
	std::string m_Actived;
	std::map<std::string, std::wstring> m_DefineTxt;
	T_app *m_App;
private:
	ui_simple(const ui_simple &rhs);
	ui_simple &operator=(const ui_simple &rhs);
};
//
template <typename T_app>
ui_simple<T_app>::ui_simple():
	m_ClickIx(-1),
	m_ClickIxPad(-1),
	m_App(0),
	m_Actived("none")
{
	M_TextLayoutOrigin.x = 0.0f;
	M_TextLayoutOrigin.y = 0.0f;
}
//
template <typename T_app>
ui_simple<T_app>::~ui_simple()
{
	for (auto &brush: m_Brush) ReleaseCOM(brush.second);
}
//
template <typename T_app>
void ui_simple<T_app>::init(T_app *app_in)
{
	m_App = app_in;
	//
	define_txt_str();
	define_style();
	on_resize();
}
//
template <typename T_app>
void ui_simple<T_app>::build_rect_map()
{
	// map rect id_str for search parent
	// -1 stands for root
	m_MapID["-1"] = -1;
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		m_MapID[m_Rect[ix].id_str] = static_cast<int>(ix);
	}	
	// map rect accroding group name
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		m_Rect[ix].parent = m_MapID[m_Rect[ix].parent_str];
		m_MapGroup[m_Rect[ix].group].emplace_back(ix);
		if (m_Rect[ix].tp == ui_rect::type::button) m_MapButton[m_Rect[ix].group].emplace_back(ix);
		if (m_Rect[ix].tp == ui_rect::type::text_layout) {
			m_MapTextLayout[m_Rect[ix].group].emplace_back(ix);
			on_resize_TextLayout(ix);
		}
	}
}
//
template <typename T_app>
void ui_simple<T_app>::on_resize_RectFromHWND(const size_t &ix)
{
	RECT rc_hwnd;
	GetClientRect(m_App->m_hwnd, &rc_hwnd);
	LONG height = rc_hwnd.bottom - rc_hwnd.top;
	LONG width = rc_hwnd.right - rc_hwnd.left;
	m_Rect[ix].rc = D2D1::RectF(
		rc_hwnd.left + m_Rect[ix].margin.x*width,
		rc_hwnd.top + m_Rect[ix].margin.y*height,
		rc_hwnd.right - m_Rect[ix].margin.z*width,
		rc_hwnd.bottom - m_Rect[ix].margin.w*height);
}
//
template <typename T_app>
void ui_simple<T_app>::on_resize_RectFromRect(const size_t &ix, const int &parent)
{
	assert(parent >= 0);
	LONG height = static_cast<LONG>(m_Rect[parent].rc.bottom - m_Rect[parent].rc.top);
	LONG width = static_cast<LONG>(m_Rect[parent].rc.right - m_Rect[parent].rc.left);
	m_Rect[ix].rc = D2D1::RectF(
		m_Rect[parent].rc.left + m_Rect[ix].margin.x*width,
		m_Rect[parent].rc.top + m_Rect[ix].margin.y*height,
		m_Rect[parent].rc.right - m_Rect[ix].margin.z*width,
		m_Rect[parent].rc.bottom - m_Rect[ix].margin.w*height);
}
//
template <typename T_app>
void ui_simple<T_app>::on_resize_TextLayout(const size_t &ix, const bool &is_resize = false)
{
	// title font size
	FLOAT title_font = 32.0f;
	RECT rc_hwnd;
	GetClientRect(m_App->m_hwnd, &rc_hwnd);
	float height = static_cast<float>(rc_hwnd.bottom - rc_hwnd.top);
	float width = static_cast<float>(rc_hwnd.right - rc_hwnd.left);
	float txt_width = (1-m_Rect[ix].margin.x-m_Rect[ix].margin.z)*width;
	// uses margin.y as origin.x
	m_Rect[ix].margin.y = (width-txt_width)/2.0f;
	auto rect = &m_Rect[ix];
	if (!is_resize) {
		// init, title len = group size + 1
		m_Dwrite[rect->dwrite_ix].on_resize_TextLayout(
			rect->text, txt_width, height, ix, title_font, rect->group.size()+1);
	}	
	else {
		m_Dwrite[rect->dwrite_ix].resize_TextLayout(ix, txt_width, height);
	}
	// uses margin.w as text layout's height
	m_Dwrite[m_Rect[ix].dwrite_ix].get_TextLayout_height(ix, m_Rect[ix].margin.w);
}
//
template <typename T_app>
PCWSTR ui_simple<T_app>::get_DefineTxt(const std::string &name)
{
	return m_DefineTxt[name].c_str();
}
//
template <typename T_app>
void ui_simple<T_app>::on_resize()
{
	for (auto &dwrite: m_Dwrite)  dwrite.second.on_resize_CreateTextFormat(m_App->m_hwnd);
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		if (m_Rect[ix].tp < 3) {
			if (m_Rect[ix].parent < 0) on_resize_RectFromHWND(ix);
			else on_resize_RectFromRect(ix, m_Rect[ix].parent);
		}
		if (m_Rect[ix].tp == 3) {
			on_resize_TextLayout(ix, true);
		}
	}
}
//
template <typename T_app>
void ui_simple<T_app>::set_Brush(D2D1::ColorF::Enum color, const float &opacity, const std::string index)
{
	D2D1_BRUSH_PROPERTIES br_prop;
	br_prop.opacity = opacity;
	br_prop.transform = D2D1::IdentityMatrix();
	m_App->m_D2DDC->CreateSolidColorBrush(D2D1::ColorF(color), br_prop, &m_Brush[index]);
}
//
template <typename T_app>
void ui_simple<T_app>::draw()
{
	for (auto it = m_Rect.begin(); it != m_Rect.end(); ++it) {
		if (!it->active) continue;
		if (it->tp < 2) m_App->m_D2DDC->FillRectangle(&it->rc, m_Brush[it->brush_ix]);
		if (it->tp < 3) m_Dwrite[it->dwrite_ix].draw_Rect(m_App->m_D2DDC, it->text, it->rc);
		if (it->tp == 3) {
			M_TextLayoutOrigin.x = it->margin.y;
			m_Dwrite[it->dwrite_ix].draw_TextLayout(m_App->m_D2DDC, M_TextLayoutOrigin, it-m_Rect.begin());
		}
	}
}
//
template <typename T_app>
bool ui_simple<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if (btn_state & MK_LBUTTON) return (define_pick_lb(pos_x, pos_y));
	return false;
}
//
template <typename T_app>
bool ui_simple<T_app>::on_mouse_wheel(const short &z_delta)
{
	for (auto &map: m_MapTextLayout) {
		if (m_Rect[map.second[0]].active) {
			float restrict_y = -m_Rect[map.second[0]].margin.w+100.0f;
			M_TextLayoutOrigin.y += z_delta/120*30;
			if (M_TextLayoutOrigin.y < restrict_y) M_TextLayoutOrigin.y = restrict_y;
			if (M_TextLayoutOrigin.y > 0.0f) M_TextLayoutOrigin.y = 0.0f;
			return true;
		}
	}
	return false;
}
//
template <typename T_app>
void ui_simple<T_app>::on_mouse_over(const int &pos_x, const int &pos_y)
{
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		if (m_Rect[ix].tp == ui_rect::type::button && m_Rect[ix].active &&
			pos_y > m_Rect[ix].rc.top && pos_y < m_Rect[ix].rc.bottom &&
			pos_x > m_Rect[ix].rc.left && pos_x < m_Rect[ix].rc.right) {
			m_Rect[ix].brush_ix = m_Rect[ix].brush_sel[1];
		}
		else m_Rect[ix].brush_ix = m_Rect[ix].brush_sel[0];
	}
}
//
template <typename T_app>
void ui_simple<T_app>::mouse_pick(const int &pos_x, const int &pos_y)
{
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		if (m_Rect[ix].tp == ui_rect::type::button && m_Rect[ix].active &&
			pos_y > m_Rect[ix].rc.top && pos_y < m_Rect[ix].rc.bottom &&
			pos_x > m_Rect[ix].rc.left && pos_x < m_Rect[ix].rc.right) {
			m_ClickIx = static_cast<int>(ix);
			break;
		}
	}
}
//
template <typename T_app>
void ui_simple<T_app>::group_active_switch(const std::string &name)
{
	if (!(m_Actived == "none" || m_Actived == name)) return;
	for (auto &ix: m_MapGroup[name]) m_Rect[ix].active = !m_Rect[ix].active;
	if (m_Rect[m_MapGroup[name][0]].active) {
		m_ClickIxPad = -1;
		if (m_MapButton[name].size() > 0) {
			m_Actived = name;
			pad_loop_button(true, name);
		}
		else m_Actived = "none";
	}
	else {
		m_Actived = "none";
		M_TextLayoutOrigin.y = 0.0f;
	}
}
//
template <typename T_app>
void ui_simple<T_app>::group_active(const std::string &name, const bool &is_act)
{
	for (auto &ix: m_MapGroup[name]) m_Rect[ix].active = is_act;
	if (m_Rect[m_MapGroup[name][0]].active) {
		m_ClickIxPad = -1;
		if (m_MapButton[name].size() > 0) {
			m_Actived = name;
			pad_loop_button(true, name);
		}
		else m_Actived = "none";
	}
	else {
		m_Actived = "none";
		M_TextLayoutOrigin.y = 0.0f;
	}
}
//
template <typename T_app>
void ui_simple<T_app>::pad_loop_button(const bool &is_down, const std::string &select_none = "")
{
	// select none
	if (select_none != "") {
		assert(m_MapButton[select_none].size() > 0);
		for (auto &ix: m_MapButton[select_none]) m_Rect[ix].brush_ix = m_Rect[ix].brush_sel[0];
		return;
	}
	// select first
	if (m_ClickIxPad == -1 && m_Actived != "none") {
		assert(m_MapButton[m_Actived].size() > 0);
		m_Rect[m_MapButton[m_Actived][0]].brush_ix = m_Rect[m_MapButton[m_Actived][0]].brush_sel[1];
		m_ClickIxPad = static_cast<int>(m_MapButton[m_Actived][0]);
		return;
	}
	// select next
	if (m_ClickIxPad != -1 && m_Actived != "none") {
		m_Rect[m_ClickIxPad].brush_ix = m_Rect[m_ClickIxPad].brush_sel[0];
		// point to next button
		size_t pos = 
			find(m_MapButton[m_Actived].begin(), m_MapButton[m_Actived].end(),
			m_ClickIxPad) - m_MapButton[m_Actived].begin();
		int next = static_cast<int>(pos);
		//
		if (is_down) ++next;
		else --next;
		int int_size = static_cast<int>(m_MapButton[m_Actived].size());
		if (next > int_size-1) next = 0;
		if (next < 0) next = int_size-1;
		m_ClickIxPad = static_cast<int>(m_MapButton[m_Actived][next]);
		m_Rect[m_MapButton[m_Actived][next]].brush_ix = m_Rect[m_MapButton[m_Actived][next]].brush_sel[1];
	}
}
//
template <typename T_app>
void ui_simple<T_app>::pad_roll_text_layout(const bool &is_down)
{
	for (auto &map: m_MapTextLayout) {
		if (m_Rect[map.second[0]].active) {
			float restrict_y = -m_Rect[map.second[0]].margin.w+100.0f;
			float delta = 100.0f;
			if (is_down) delta = -delta;
			M_TextLayoutOrigin.y += delta;
			if (M_TextLayoutOrigin.y < restrict_y) M_TextLayoutOrigin.y = restrict_y;
			if (M_TextLayoutOrigin.y > 0.0f) M_TextLayoutOrigin.y = 0.0f;
			return;
		}
	}
	return;
}
//
template <typename T_app>
bool ui_simple<T_app>::is_ui_appear()
{
	if (m_Actived == "none") return false;
	return true;
}
//
template <typename T_app>
void ui_simple<T_app>::deactivate_all()
{
	for (auto &map: m_MapGroup) {
		group_active(map.first, false);
	}
	m_Actived = "none";
}
////////////////
// misc_ui_define.h
////////////////
////////////////
#include "misc_ui_define.h"
#include "misc_ui_define_txt.h"
}
#endif