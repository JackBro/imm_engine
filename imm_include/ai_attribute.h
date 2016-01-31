////////////////
// ai_attribute.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_ATTRIBUTE_H
#define AI_ATTRIBUTE_H
#include <string>
#include <map>
namespace imm
{
////////////////
// ai_points
////////////////
////////////////
struct ai_points
{
	ai_points();
	int hp_max;
	int mp_max;
	int hp;
	int mp;
	int str;
	int mgc;
	int def;
	int res;
};
//
ai_points::ai_points():
	hp_max(20),
	mp_max(20),
	hp(20),
	mp(20),
	str(5),
	mgc(5),
	def(5),
	res(5)
{
	;
}
////////////////
// ui_attr
////////////////
////////////////
template <typename T_app>
struct ui_attr
{
	ui_attr();
	void init(T_app *app_in);
	void reset();
	void update();
	void update_target();
	T_app *app;
	int p1_hp;
	int p1_mp;
	int p1_hp_max;
	int p1_mp_max;
	int tar_hp;
	int tar_hp_max;
	size_t tar_ix;
	size_t tar_flush;
	float count_down;
	bool need_resize;
};
//
template <typename T_app>
ui_attr<T_app>::ui_attr():
	app(nullptr),
	p1_hp(1),
	p1_mp(1),
	p1_hp_max(1),
	p1_mp_max(1),
	tar_hp(1),
	tar_hp_max(1),
	tar_ix(0),
	tar_flush(0),
	count_down(-1.0f),
	need_resize(false)
{
	;
}
//
template <typename T_app>
void ui_attr<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ui_attr<T_app>::reset()
{
	p1_hp = 1;
	p1_mp = 1;
}
//
template <typename T_app>
void ui_attr<T_app>::update()
{
	if (!app->m_UiMgr.status.m_IsActive) return;
	need_resize = false;
	if (p1_hp_max != app->m_AiAttr.points[app->m_Control.player1].hp_max) {
		p1_hp_max = app->m_AiAttr.points[app->m_Control.player1].hp_max;
		app->m_UiMgr.status.define_set_hp_rect(p1_hp_max);
		need_resize = true;
	}
	if (p1_mp_max != app->m_AiAttr.points[app->m_Control.player1].mp_max) {
		p1_mp_max = app->m_AiAttr.points[app->m_Control.player1].mp_max;
		app->m_UiMgr.status.define_set_mp_rect(p1_mp_max);
		need_resize = true;
	}
	if (p1_hp != app->m_AiAttr.points[app->m_Control.player1].hp) {
		p1_hp = app->m_AiAttr.points[app->m_Control.player1].hp;
		float z = static_cast<float>(p1_hp) / static_cast<float>(p1_hp_max);
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_hp_bar(z);
		need_resize = true;
	}
	if (p1_mp != app->m_AiAttr.points[app->m_Control.player1].mp) {
		p1_mp = app->m_AiAttr.points[app->m_Control.player1].mp;
		float z = static_cast<float>(p1_mp) / static_cast<float>(p1_mp_max);
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_mp_bar(z);
		need_resize = true;
	}
	update_target();
	if (need_resize) app->m_UiMgr.status.on_resize();
}
//
template <typename T_app>
void ui_attr<T_app>::update_target()
{
	if (count_down > 0.0f) {
		count_down -= app->m_Timer.delta_time();
	}
	else {
		app->m_UiMgr.status.m_IsTarShow = false;
		app->m_UiMgr.status.group_active("tar", false);
	}
	if (tar_flush == app->m_Inst.m_Steering[app->m_Control.player1].attack.size()) return;
	tar_flush = app->m_Inst.m_Steering[app->m_Control.player1].attack.size();
	if (tar_flush == 0) return;
	tar_ix = app->m_Inst.m_Steering[app->m_Control.player1].attack.back();
	if (app->m_Inst.m_Stat[tar_ix].type != MODEL_SKINNED) return;
	if (tar_hp_max != app->m_AiAttr.points[tar_ix].hp_max ||
		tar_hp != app->m_AiAttr.points[tar_ix].hp) {
		tar_hp_max = app->m_AiAttr.points[tar_ix].hp_max;
		tar_hp = app->m_AiAttr.points[tar_ix].hp;
		float z = static_cast<float>(tar_hp) / static_cast<float>(tar_hp_max);
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_tar_hp_bar(z);
		app->m_UiMgr.status.define_set_tar_hp_rect(tar_hp_max);
		need_resize = true;
	}
	count_down = 3.0f;
	app->m_UiMgr.status.group_active("tar", true);
	app->m_UiMgr.status.define_set_tar_name(
		*app->m_Ingot.get_name(*app->m_Inst.m_Stat[tar_ix].get_ModelName()));
}
////////////////
// ai_attr
////////////////
////////////////
template <typename T_app>
struct ai_attr
{
	ai_attr();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void rebuild_points();
	void rebuild_troll();
	void update();
	T_app *app;
	ui_attr<T_app> ui;
	std::map<size_t, ai_points> points;
	std::map<std::string, size_t> points_name;
};
//
template <typename T_app>
ai_attr<T_app>::ai_attr():
	app(nullptr),
	ui(),
	points(),
	points_name()
{
	;
}
//
template <typename T_app>
void ai_attr<T_app>::init(T_app *app_in)
{
	app = app_in;
	ui.init(app);
}
//
template <typename T_app>
void ai_attr<T_app>::reset()
{
	points.clear();
	points_name.clear();
	ui.reset();	
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild()
{
	reset();
	rebuild_points();
	rebuild_troll();
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild_points()
{
	size_t ix = 0;
	for (auto &stat: app->m_Inst.m_Stat) {
		if (stat.type == MODEL_SKINNED) {
			points[ix];
			app->m_Ingot.assign_attr(points[ix], *(app->m_Inst.m_Stat[ix].get_ModelName()));
			points_name[*(app->m_Inst.m_Stat[ix].get_ModelName())] = ix;
		}
		++ix;
	}
	if (points.count(app->m_Control.player1)) app->m_UiMgr.status.m_IsActive = true;
	else app->m_UiMgr.status.m_IsActive = false;
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild_troll()
{
	;
}
//
template <typename T_app>
void ai_attr<T_app>::update()
{
	ui.update();
}
//
}
#endif