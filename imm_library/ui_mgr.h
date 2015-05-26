////////////////
// ui_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_MGR_H
#define UI_MGR_H
#include "ui_main_menu_rect.h"
#include "ui_welcome_rect.h"
namespace imm
{
////////////////
// ui_mgr
////////////////
////////////////
template <typename T_app>
struct ui_mgr
{
	ui_mgr() {;}
	void init(T_app *app_in);
	void on_resize();
	void define_update();
	void draw_d2d();
	void draw_d3d();
	bool on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	bool on_mouse_wheel(const short &z_delta);
	void on_mouse_over(const int &pos_x, const int &pos_y);
	bool is_ui_appear();
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_pad_keydown(const WORD &vkey);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	// reload
	void reload_active(const std::string &ui_class, const std::string &ui_group);
	void reset();
	T_app *app;
	ui_main_menu<T_app> main_menu;
	ui_welcome<T_app> welcome;
};
//
template <typename T_app>
void ui_mgr<T_app>::init(T_app *app_in)
{
	main_menu.init(app_in);
	welcome.init(app_in);
}
//
template <typename T_app>
void ui_mgr<T_app>::on_resize()
{
	main_menu.on_resize();
	welcome.on_resize();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_update()
{
	main_menu.define_update();
	welcome.define_update();
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d2d()
{
	main_menu.draw_d2d();
	welcome.draw_d2d();
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d3d()
{
	main_menu.draw_d3d();
	welcome.draw_d3d();
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	bool rt_bool = false;
	rt_bool = main_menu.on_mouse_down(btn_state, pos_x, pos_y);
	if (rt_bool) return rt_bool;
	rt_bool = welcome.on_mouse_down(btn_state, pos_x, pos_y);
	return rt_bool;
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_wheel(const short &z_delta)
{
	bool rt_bool = false;
	rt_bool = main_menu.on_mouse_wheel(z_delta);
	if (rt_bool) return rt_bool;
	rt_bool = welcome.on_mouse_wheel(z_delta);
	return rt_bool;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_mouse_over(const int &pos_x, const int &pos_y)
{
	main_menu.on_mouse_over(pos_x, pos_y);
	welcome.on_mouse_over(pos_x, pos_y);
}
//
template <typename T_app>
bool ui_mgr<T_app>::is_ui_appear()
{
	bool rt_bool = false;
	rt_bool = main_menu.is_ui_appear();
	if (rt_bool) return rt_bool;
	rt_bool = welcome.is_ui_appear();
	return rt_bool;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	main_menu.on_input_keydown(w_param, l_param);
	welcome.on_input_keydown(w_param, l_param);
}
//
template <typename T_app>
void ui_mgr<T_app>::on_pad_keydown(const WORD &vkey)
{
	main_menu.on_pad_keydown(vkey);
	welcome.on_pad_keydown(vkey);
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_default()
{
	main_menu.define_deactivate_all_default();
	welcome.define_deactivate_all_default();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_cmd_slient()
{
	main_menu.define_deactivate_all_cmd_slient();
	welcome.define_deactivate_all_cmd_slient();
}
//
template <typename T_app>
void ui_mgr<T_app>::reload_active(const std::string &ui_class, const std::string &ui_group)
{
	reset();
	if (ui_class == "welcome") {
		if (!welcome.m_MapGroup.count(ui_group)) {
			std::string err_str("UI group not found: ");
			err_str += ui_group;
			ERROR_MESA(err_str.c_str());
		}
		main_menu.m_IsMute = true;
		welcome.m_IsMute = false;
		welcome.group_active(ui_group, true);
	}
}
//
template <typename T_app>
void ui_mgr<T_app>::reset()
{
	main_menu.m_IsMute = false;
	welcome.m_IsMute = true;
	define_deactivate_all_default();
}
//
}
#endif