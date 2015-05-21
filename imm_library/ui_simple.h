////////////////
// ui_simple.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_SIMPLE_H
#define UI_SIMPLE_H
#include "ui_sprite.h"
#include "ui_dwrite.h"
#include "control_key_define.h"
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
	void group_active(const std::string &name, const bool &is_act, const bool &is_switched = false);
	void pad_loop_button(const bool &is_down, const std::string &select_none = "");
	void pad_roll_text_layout(const bool &is_down);
	bool is_ui_appear();
	void deactivate_all();
	bool apply_ix(int &index);
	//
	void define_style();
	bool define_apply_ix_if(int &index);
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_on_pad_keydown(const WORD &vkey, const float &dt);
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
	sprite_simple m_Sprite;
	int m_ClickIxMouse;
	int m_ClickIxPad;
	bool m_IsPadUsing;
	FLOAT m_TitleFontFactor;
	D2D1_POINT_2F m_TextLayoutOrigin;
	RECT m_RcHWND;
	// if a group has no button, it is no-clickable, otherwise it is clickable
	std::string m_ClickableActived;
	std::map<std::string, std::wstring> m_DefineTxt;
	T_app *m_App;
private:
	ui_simple(const ui_simple &rhs);
	ui_simple &operator=(const ui_simple &rhs);
};
//
template <typename T_app>
ui_simple<T_app>::ui_simple():
	m_ClickIxMouse(-1),
	m_ClickIxPad(-1),
	m_IsPadUsing(false),
	m_TitleFontFactor(32.0f),
	m_RcHWND(),
	m_ClickableActived("none"),
	m_App(0)
{
	m_TextLayoutOrigin.x = 0.0f;
	m_TextLayoutOrigin.y = 0.0f;
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
	m_Sprite.init(m_App->m_D3DDevice, m_App->m_D3DDC);
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
	LONG height = m_RcHWND.bottom - m_RcHWND.top;
	LONG width = m_RcHWND.right - m_RcHWND.left;
	m_Rect[ix].rc = D2D1::RectF(
		m_RcHWND.left + m_Rect[ix].margin.x*width,
		m_RcHWND.top + m_Rect[ix].margin.y*height,
		m_RcHWND.right - m_Rect[ix].margin.z*width,
		m_RcHWND.bottom - m_Rect[ix].margin.w*height);
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
	float height = static_cast<float>(m_RcHWND.bottom - m_RcHWND.top);
	float width = static_cast<float>(m_RcHWND.right - m_RcHWND.left);
	float txt_width = (1-m_Rect[ix].margin.x-m_Rect[ix].margin.z)*width;
	// uses margin.y as origin.x
	m_Rect[ix].margin.y = (width-txt_width)/2.0f;
	auto rect = &m_Rect[ix];
	if (!is_resize) {
		// init, title len = group size + 1
		m_Dwrite[rect->dwrite_ix].on_resize_TextLayout(
			rect->text, txt_width, height, ix, m_TitleFontFactor, rect->group.size()+1);
	}	
	else {
		m_Dwrite[rect->dwrite_ix].build_resize_TextLayout(ix, txt_width, height);
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
	// if not init, no resize
	if (m_MapGroup.size() == 0) return;
	GetClientRect(m_App->m_hwnd, &m_RcHWND);
	for (auto &dwrite: m_Dwrite) dwrite.second.on_resize_CreateTextFormat(m_App->m_hwnd);
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
			m_TextLayoutOrigin.x = it->margin.y;
			m_Dwrite[it->dwrite_ix].draw_TextLayout(m_App->m_D2DDC, m_TextLayoutOrigin, it-m_Rect.begin());
		}
	}
}
//
template <typename T_app>
bool ui_simple<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if (btn_state & MOUSE_UI_PICK) {
		m_IsPadUsing = false;
		mouse_pick(pos_x, pos_y);
		return apply_ix(m_ClickIxMouse);
	}
	return false;
}
//
template <typename T_app>
bool ui_simple<T_app>::on_mouse_wheel(const short &z_delta)
{
	for (auto &map: m_MapTextLayout) {
		if (m_Rect[map.second[0]].active) {
			float restrict_y = -m_Rect[map.second[0]].margin.w+100.0f;
			m_TextLayoutOrigin.y += z_delta/120*30;
			if (m_TextLayoutOrigin.y < restrict_y) m_TextLayoutOrigin.y = restrict_y;
			if (m_TextLayoutOrigin.y > 0.0f) m_TextLayoutOrigin.y = 0.0f;
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
			m_ClickIxMouse = static_cast<int>(ix);
			break;
		}
	}
}
//
template <typename T_app>
void ui_simple<T_app>::group_active_switch(const std::string &name)
{
	// avoid active two clickable group
	if (!(m_ClickableActived == "none" || m_ClickableActived == name)) {
		define_deactivate_all_default();
		return;
	}
	for (auto &ix: m_MapGroup[name]) m_Rect[ix].active = !m_Rect[ix].active;
	group_active(name, false, true);
}
//
template <typename T_app>
void ui_simple<T_app>::group_active(const std::string &name, const bool &is_act, const bool &is_switched = false)
{
	if (!is_switched) for (auto &ix: m_MapGroup[name]) m_Rect[ix].active = is_act;
	if (m_Rect[m_MapGroup[name][0]].active) {
		m_ClickIxPad = -1;
		if (m_MapButton[name].size() > 0) {
			m_ClickableActived = name;
			pad_loop_button(true, name);
		}
		else {
			// keep no button group, as no-clickable
			m_ClickableActived = "none";
		}
	}
	else {
		m_ClickableActived = "none";
		// reset text_layout position
		m_TextLayoutOrigin.y = 0.0f;
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
		if (m_IsPadUsing) pad_loop_button(true);
		return;
	}
	// select first
	if (m_ClickIxPad == -1 && m_ClickableActived != "none") {
		assert(m_MapButton[m_ClickableActived].size() > 0);
		m_Rect[m_MapButton[m_ClickableActived][0]].brush_ix = m_Rect[m_MapButton[m_ClickableActived][0]].brush_sel[1];
		m_ClickIxPad = static_cast<int>(m_MapButton[m_ClickableActived][0]);
		return;
	}
	// select next
	if (m_ClickIxPad != -1 && m_ClickableActived != "none") {
		m_Rect[m_ClickIxPad].brush_ix = m_Rect[m_ClickIxPad].brush_sel[0];
		// point to next button
		size_t pos = 
			find(m_MapButton[m_ClickableActived].begin(), m_MapButton[m_ClickableActived].end(),
			m_ClickIxPad) - m_MapButton[m_ClickableActived].begin();
		int next = static_cast<int>(pos);
		//
		if (is_down) ++next;
		else --next;
		int int_size = static_cast<int>(m_MapButton[m_ClickableActived].size());
		if (next > int_size-1) next = 0;
		if (next < 0) next = int_size-1;
		m_ClickIxPad = static_cast<int>(m_MapButton[m_ClickableActived][next]);
		m_Rect[m_MapButton[m_ClickableActived][next]].brush_ix = m_Rect[m_MapButton[m_ClickableActived][next]].brush_sel[1];
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
			m_TextLayoutOrigin.y += delta;
			if (m_TextLayoutOrigin.y < restrict_y) m_TextLayoutOrigin.y = restrict_y;
			if (m_TextLayoutOrigin.y > 0.0f) m_TextLayoutOrigin.y = 0.0f;
			return;
		}
	}
	return;
}
//
template <typename T_app>
bool ui_simple<T_app>::is_ui_appear()
{
	if (m_ClickableActived == "none") return false;
	return true;
}
//
template <typename T_app>
void ui_simple<T_app>::deactivate_all()
{
	for (auto &map: m_MapGroup) {
		group_active(map.first, false);
	}
	m_ClickableActived = "none";
}
//
template <typename T_app>
bool ui_simple<T_app>::apply_ix(int &index)
{
	if (index != -1) {
		if (define_apply_ix_if(index)) {
			// no change m_ClickIxPad and m_ClickIxMouse value in define_ functions
			// let pad select first button immediately from another group apply, keep m_ClickIxPad
			// otherwise m_ClickIxPad will be clean by apply, the first selected buttom will be invalid
			if (!m_IsPadUsing) {
				index = -1;
			}
			return true;
		}
	}
	return false;
}

////////////////
// misc_ui_define.h
////////////////
////////////////
#include "ui_simple_define_rc.h"
#include "ui_simple_define_txt.h"
}
#endif