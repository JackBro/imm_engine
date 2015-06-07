////////////////
// ui_dialogue.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_DIALOGUE_H
#define UI_DIALOGUE_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_dialogue
////////////////
////////////////
template <class T_app>
struct ui_dialogue: public ui_base<T_app>
{
	ui_dialogue() {;}
	~ui_dialogue() {;}
	void define_style();
	bool define_apply_ix_if(int &index);
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_on_pad_keydown(const WORD &vkey);
	void define_update();
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_sprite_build_buffer();
	void define_on_resize_sprite();
	void define_txt_str();
};
//
template <typename T_app>
void ui_dialogue<T_app>::define_style()
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
	set_Brush(D2D1::ColorF::Red, 0.5f, "red");
	////////////////
	// 
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "test";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::sprite;
	m_Rect.back().brush_sel = {"black_00a"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "test_background";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Test!!!";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.75f, 0.0f, 0.0f);
	////////////////
	//
	////////////////
	////////////////
	build_rect_map();
	//
	for (auto &rect: m_Rect) {
		rect.brush_ix = rect.brush_sel[0];
		if (rect.tp == ui_rect::type::button) {
			if (rect.group == "entrance") rect.brush_sel.emplace_back("black");
			else rect.brush_sel.emplace_back("red");
		}
		rect.active = false;
	}
}
//
template <typename T_app>
bool ui_dialogue<T_app>::define_apply_ix_if(int &index)
{
	DUMMY(index);
	return false;
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	DUMMY(w_param);
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	DUMMY(vkey);
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_update()
{
	;
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_deactivate_all_default()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_sprite_build_buffer()
{
	std::map<std::string, std::string> get_dds;
	get_dds["test"] = "";
	std::string describe = GLOBAL["path_lua"]+"scene_common\\ui_dialogue.lua";
	lua_reader l_reader;
	l_reader.loadfile(describe);
	l_reader.map_from_global(get_dds);
	m_Sprite.build_buffer(get_dds);
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_on_resize_sprite()
{
	float height = static_cast<float>(m_RcHWND.bottom - m_RcHWND.top);
	float width = static_cast<float>(m_RcHWND.right - m_RcHWND.left);
	float scale = height/REF_RESOLUTION_HEIGHT*0.6f;
	std::map<std::string, XMFLOAT2> get_resize;
	get_resize["test"] = XMFLOAT2(width*0.0f*scale, height-(1024.0f*scale));
	m_Sprite.on_resize(scale, get_resize);
}
//
template <typename T_app>
void ui_dialogue<T_app>::define_txt_str()
{
	;
}
//
}
#endif