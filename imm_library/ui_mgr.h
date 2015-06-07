////////////////
// ui_mgr.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_MGR_H
#define UI_MGR_H
#include "ui_main_menu.h"
#include "ui_welcome.h"
#include "ui_dialogue.h"
namespace imm
{
////////////////
// ui_mgr
////////////////
////////////////
template <typename T_app>
struct ui_mgr
{
	ui_mgr();
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
	// Reload
	void reload_active(const std::string &ui_class, const std::string &ui_group);
	void reset();
	T_app *app;
	ui_main_menu<T_app> main_menu;
	ui_welcome<T_app> welcome;
	ui_dialogue<T_app> dialogue;
	std::vector<ui_base<T_app>*> ui_together;
};
//
template <typename T_app>
ui_mgr<T_app>::ui_mgr():
	app(nullptr),
	main_menu(),
	welcome()
{
	;
}
//
template <typename T_app>
void ui_mgr<T_app>::init(T_app *app_in)
{
	app = app_in;
	main_menu.init(app_in);
	welcome.init(app_in);
	dialogue.init(app_in);
	ui_together.push_back(&main_menu);
	ui_together.push_back(&welcome);
	ui_together.push_back(&dialogue);
}
//
template <typename T_app>
void ui_mgr<T_app>::on_resize()
{
	for (auto &ui: ui_together) ui->on_resize();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_update()
{
	for (auto &ui: ui_together) ui->define_update();
	// Avoid UI conflict
	bool is_ui_appear = false;
	for (size_t ix = 0; ix != ui_together.size(); ++ix) {
		ui_together[ix]->m_IsOtherUIAppear = false;
		if (ui_together[ix]->is_ui_appear()) {
			is_ui_appear = true;
		}
	}
	if (is_ui_appear) {
		for (auto &ui: ui_together) {
			if (!ui->is_ui_appear()) ui->m_IsOtherUIAppear = true;
		}
	}
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d2d()
{
	for (auto &ui: ui_together) ui->draw_d2d();
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d3d()
{
	for (auto &ui: ui_together) ui->draw_d3d();
	// Restore default states.
	app->m_D3DDC->RSSetState(0);
	app->m_D3DDC->OMSetDepthStencilState(0, 0);
	app->m_D3DDC->OMSetBlendState(0, BLEND_FACTOR_ZERO, 0xffffffff);
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	bool rt_bool = false;
	for (auto &ui: ui_together) {
		rt_bool = ui->on_mouse_down(btn_state, pos_x, pos_y);
		if (rt_bool) return rt_bool;
	}
	return rt_bool;
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_wheel(const short &z_delta)
{
	bool rt_bool = false;
	for (auto &ui: ui_together) {
		rt_bool = ui->on_mouse_wheel(z_delta);
		if (rt_bool) return rt_bool;
	}
	return rt_bool;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_mouse_over(const int &pos_x, const int &pos_y)
{
	for (auto &ui: ui_together) ui->on_mouse_over(pos_x, pos_y);
}
//
template <typename T_app>
bool ui_mgr<T_app>::is_ui_appear()
{
	bool rt_bool = false;
	for (auto &ui: ui_together) {
		rt_bool = ui->is_ui_appear();
		if (rt_bool) return rt_bool;
	}
	return rt_bool;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	for (auto &ui: ui_together) ui->on_input_keydown(w_param, l_param);
}
//
template <typename T_app>
void ui_mgr<T_app>::on_pad_keydown(const WORD &vkey)
{
	for (auto &ui: ui_together) ui->on_pad_keydown(vkey);
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_default()
{
	for (auto &ui: ui_together) ui->define_deactivate_all_default();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_cmd_slient()
{
	for (auto &ui: ui_together) ui->define_deactivate_all_cmd_slient();
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