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
	void define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void define_on_pad_keydown(const WORD &vkey);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	// reload
	void reload_active(const std::string &ui_class, const std::string &ui_name);
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
}
//
template <typename T_app>
void ui_mgr<T_app>::define_update()
{
	main_menu.define_update();
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d2d()
{
	main_menu.draw_d2d();
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d3d()
{
	main_menu.draw_d3d();
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	return main_menu.on_mouse_down(btn_state, pos_x, pos_y);
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_wheel(const short &z_delta)
{
	return main_menu.on_mouse_wheel(z_delta);
}
//
template <typename T_app>
void ui_mgr<T_app>::on_mouse_over(const int &pos_x, const int &pos_y)
{
	main_menu.on_mouse_over(pos_x, pos_y);
}
//
template <typename T_app>
bool ui_mgr<T_app>::is_ui_appear()
{
	return main_menu.is_ui_appear();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	main_menu.define_on_input_keydown(w_param, l_param);
}
//
template <typename T_app>
void ui_mgr<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	main_menu.define_on_pad_keydown(vkey);
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_default()
{
	main_menu.define_deactivate_all_default();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_cmd_slient()
{
	main_menu.define_deactivate_all_cmd_slient();
}
template <typename T_app>
void ui_mgr<T_app>::reload_active(const std::string &ui_class, const std::string &ui_name)
{
	ui_class;
	ui_name;
}
//
}
#endif