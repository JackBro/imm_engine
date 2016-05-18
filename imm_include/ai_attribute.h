////////////////
// ai_attribute.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_ATTRIBUTE_H
#define AI_ATTRIBUTE_H
#include "phy_magic.h"
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
	float hp_max;
	float ap_max;
	float hp;
	float ap;
	float str;
	float mgc;
	float def;
	float res;
};
//
ai_points::ai_points():
	hp_max(20.0f),
	ap_max(20.0f),
	hp(20.0f),
	ap(20.0f),
	str(5.0f),
	mgc(5.0f),
	def(5.0f),
	res(5.0f)
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
	float p1_hp;
	float p1_ap;
	float p1_hp_max;
	float p1_ap_max;
	float tar_hp;
	float tar_hp_max;
	size_t tar_ix;
	size_t tar_flush;
	float count_down;
	bool need_resize;
};
//
template <typename T_app>
ui_attr<T_app>::ui_attr():
	app(nullptr),
	p1_hp(1.0f),
	p1_ap(1.0f),
	p1_hp_max(1.0f),
	p1_ap_max(1.0f),
	tar_hp(1.0f),
	tar_hp_max(1.0f),
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
	count_down = -1.0f;
}
//
template <typename T_app>
void ui_attr<T_app>::update()
{
	if (!app->m_UiMgr.status.m_IsActive) return;
	need_resize = false;
	if (!math::float_is_equal(p1_hp_max, app->m_AiAttr.points[app->m_Control.player1].hp_max)) {
		p1_hp_max = app->m_AiAttr.points[app->m_Control.player1].hp_max;
		app->m_UiMgr.status.define_set_hp_rect(p1_hp_max);
		need_resize = true;
	}
	if (!math::float_is_equal(p1_ap_max, app->m_AiAttr.points[app->m_Control.player1].ap_max)) {
		p1_ap_max = app->m_AiAttr.points[app->m_Control.player1].ap_max;
		app->m_UiMgr.status.define_set_ap_rect(p1_ap_max);
		need_resize = true;
	}
	if (!math::float_is_equal(p1_hp, app->m_AiAttr.points[app->m_Control.player1].hp)) {
		p1_hp = app->m_AiAttr.points[app->m_Control.player1].hp;
		float z = p1_hp / p1_hp_max;
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_hp_bar(z);
		need_resize = true;
	}
	if (!math::float_is_equal(p1_ap, app->m_AiAttr.points[app->m_Control.player1].ap)) {
		p1_ap = app->m_AiAttr.points[app->m_Control.player1].ap;
		float z = p1_ap / p1_ap_max;
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_ap_bar(z);
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
	size_t tmp_ix = app->m_Inst.m_Steering[app->m_Control.player1].attack.back();
	if (app->m_Inst.m_Stat[tmp_ix].is_controllable) tar_ix = tmp_ix;
	else return;
	if (!math::float_is_equal(tar_hp_max, app->m_AiAttr.points[tar_ix].hp_max) ||
		!math::float_is_equal(tar_hp, app->m_AiAttr.points[tar_ix].hp)) {
		tar_hp_max = app->m_AiAttr.points[tar_ix].hp_max;
		tar_hp = app->m_AiAttr.points[tar_ix].hp;
		float z = tar_hp / tar_hp_max;
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_tar_hp_bar(z);
		app->m_UiMgr.status.define_set_tar_hp_rect(tar_hp_max);
		need_resize = true;
	}
	count_down = 3.0f;
	app->m_UiMgr.status.group_active("tar", true);
	app->m_UiMgr.status.define_set_tar_name(
		*app->m_Status.get_name(*app->m_Inst.m_Stat[tar_ix].get_ModelName()));
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
	void update(const float &dt);
	void calc_skill(const SKILL_SPECIFY &specify, const size_t &ix_atk, const size_t &ix_dmg);
	T_app *app;
	float delta_time;
	ui_attr<T_app> ui;
	std::map<size_t, ai_points> points;
	std::map<std::string, size_t> points_name;
};
//
template <typename T_app>
ai_attr<T_app>::ai_attr():
	app(nullptr),
	delta_time(0.0f),
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
		if (stat.is_controllable) {
			points[ix];
			app->m_Status.assign_attr(points[ix], *(app->m_Inst.m_Stat[ix].get_ModelName()));
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
void ai_attr<T_app>::update(const float &dt)
{
	delta_time += dt;
	if (delta_time < AI_DELTA_TIME_MIN) return;
	else delta_time -= AI_DELTA_TIME_MIN;
	ui.update();
}
//
template <typename T_app>
void ai_attr<T_app>::calc_skill(const SKILL_SPECIFY &specify, const size_t &ix_atk, const size_t &ix_dmg)
{
	ix_atk;
	switch (specify) {
	case SKILL_MELEE_STANDARD:
		points[ix_dmg].hp -= 3;
		if (points[ix_dmg].hp < 0.0f) points[ix_dmg].hp = points[ix_dmg].hp_max;
		break;
	}
}
}
#endif