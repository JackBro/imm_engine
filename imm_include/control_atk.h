////////////////
// control_atk.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_ATK_H
#define CONTROL_ATK_H
#include "control_state.h"
#include <vector>
namespace imm
{
////////////////
// combo_para
////////////////
////////////////
struct combo_para
{
	combo_para();
	std::string model_name;
	int index;
	bool is_busy;
	float time_count_down;
};
//
combo_para::combo_para():
	index(-1),
	is_busy(false),
	time_count_down(-1.0f)
{
	;
}
////////////////
// combo_data
////////////////
////////////////
struct combo_data
{
	combo_data();
	void build();
	template <typename instance>
	void loop(combo_para &pa, instance &inst);
	template <typename instance, typename troll>
	void update(const float &dt, combo_para &pa, instance &inst, troll &tro);
	std::vector<std::string> atk;
	std::vector<float> frame_end;
	std::vector<float> frame_turn;
	std::vector<float> frame_speed;
	float frame_rate;
};
//
combo_data::combo_data():
	frame_rate(24.0f)
{
	;
}
//
void combo_data::build()
{
	atk.push_back("Atk01");
	atk.push_back("Atk02");
	frame_end.push_back(9.0f);
	frame_end.push_back(11.0f);
	frame_turn.push_back(4.5f-2.0f);
	frame_turn.push_back(5.5f-2.0f);
	frame_speed.push_back(0.0f);
	frame_speed.push_back(0.0f);
	for (auto &end: frame_end) end /= frame_rate;
	for (auto &turn: frame_turn) turn /= frame_rate;
}
//
template <typename instance>
void combo_data::loop(combo_para &pa, instance &inst)
{
	if (pa.index < 0 && pa.time_count_down > 0.0f) return;
	if (pa.index == -1) {
		++pa.index;
		pa.time_count_down = frame_end[pa.index];
		inst.check_set_ClipName(atk[pa.index], true);
		pa.is_busy = true;
		return;
	}
	if (pa.index == 0 && pa.time_count_down > frame_turn[pa.index]) {
		++pa.index;
		return;
	}
}
//
template <typename instance, typename troll>
void combo_data::update(const float &dt, combo_para &pa, instance &inst, troll &tro)
{
	if (pa.time_count_down > -5.0f) pa.time_count_down -= dt;
	if (pa.is_busy && pa.time_count_down < 0.0f) {
		tro.order |= ORDER_IDLE;
		pa.index = -1;
		pa.is_busy = false;
	}
	if (!pa.is_busy && pa.time_count_down < -2.0f) {
		if (tro.current_state == pose_Idle::instance())
			inst.check_set_ClipName(act::Idle);
		pa.time_count_down = -6.0f;
	}
	if (pa.index == 1 && pa.time_count_down < frame_turn[pa.index]) {
		pa.time_count_down = 11.0f/24.0f;
		inst.check_set_ClipName("Atk02", true);
		pa.is_busy = true;
		pa.index = -1;
		return;
	}
}
////////////////
// control_atk
////////////////
////////////////
template <typename T_app>
struct control_atk
{
	control_atk();
	void init(T_app *app_in);
	void init_combo_para(const size_t &index_in);
	void perform(const size_t &index_in);
	void update(const float &dt);
	T_app *app;
	std::map<std::string, combo_data> combo;
	std::map<size_t, combo_para> combo_p;
};
//
template <typename T_app>
control_atk<T_app>::control_atk():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void control_atk<T_app>::init(T_app *app_in)
{
	app = app_in;
	combo["sinon"].build();
}
//
template <typename T_app>
void control_atk<T_app>::init_combo_para(const size_t &index_in)
{
	combo_p[index_in];
	combo_p[index_in].model_name = *app->m_Inst.m_Stat[index_in].get_ModelName();
}
//
template <typename T_app>
void control_atk<T_app>::perform(const size_t &index_in)
{
	if (!combo.count(*app->m_Inst.m_Stat[index_in].get_ModelName())) return;
	if (!combo_p.count(index_in)) init_combo_para(index_in);
	combo[combo_p[index_in].model_name].loop(combo_p[index_in], app->m_Inst.m_Stat[index_in]);
}
//
template <typename T_app>
void control_atk<T_app>::update(const float &dt)
{
	for (auto &para_it: combo_p) {
		combo[para_it.second.model_name].update(
			dt,
			para_it.second, 
			app->m_Inst.m_Stat[para_it.first],
			app->m_Inst.m_Troll[para_it.first]);
	}
}
//
}
#endif