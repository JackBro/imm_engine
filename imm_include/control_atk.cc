////////////////
// control_atk.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_atk.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// combo_data
////////////////
////////////////
combo_data::combo_data():
	frame_rate(24.0f)
{
	;
}
//
void combo_data::build(const std::string &name)
{
	if (name == "sinon") {
		atk.push_back("Atk01");
		atk.push_back("Atk02");
		frame_end.push_back(9.0f);
		frame_end.push_back(11.0f);
		frame_turn.push_back(4.5f-2.0f);
		frame_turn.push_back(5.5f-2.0f);
		frame_speed.push_back(1.0f);
		frame_speed.push_back(0.5f);
		std::vector<std::string> box_name;
		box_name.push_back("hand_L");
		atk_box.push_back(box_name);
		box_name.clear();
		box_name.push_back("foot_R");
		atk_box.push_back(box_name);
	}
	for (auto &end: frame_end) end /= frame_rate;
	for (auto &turn: frame_turn) turn /= frame_rate;
}
//
void combo_data::current_apply(combo_para &pa)
{
	if (pa.combo_ix > 0) PTR->m_Attack.set_active_box(pa.inst_ix, atk_box[pa.combo_ix-1], false);
	pa.time_count_down = frame_end[pa.combo_ix];
	PTR->m_Inst.m_Stat[pa.inst_ix].check_set_ClipName(atk[pa.combo_ix], true);
	math::set_instance_speed(pa.inst_ix, frame_speed[pa.combo_ix]);
	pa.is_busy = true;
	PTR->m_Attack.set_active_box(pa.inst_ix, atk_box[pa.combo_ix], true);
}
//
void combo_data::current_over(combo_para &pa)
{
	pa.is_busy = false;
	PTR->m_Attack.deactive_box(pa.inst_ix);
}
//
void combo_data::strike(combo_para &pa)
{
	if (pa.combo_ix < 0 && pa.time_count_down > 0.0f) return;
	if (pa.combo_ix == -1) {
		++pa.combo_ix;
		current_apply(pa);
		return;
	}
	if (!pa.is_turn_next && pa.time_count_down > frame_turn[pa.combo_ix]) {
		++pa.combo_ix;
		pa.is_turn_next = true;
		return;
	}
}
//
void combo_data::update(const float &dt, combo_para &pa)
{
	auto &tro = PTR->m_Inst.m_Troll[pa.inst_ix];
	if (pa.time_count_down > -5.0f) pa.time_count_down -= dt;
	if (pa.is_busy && pa.time_count_down < 0.0f) {
		tro.order |= ORDER_IDLE;
		current_over(pa);
		pa.combo_ix = -1;
		return;
	}
	if (!pa.is_busy && pa.time_count_down < -2.0f) {
		if (tro.current_state == pose_Idle::instance())
			PTR->m_Inst.m_Stat[pa.inst_ix].check_set_ClipName(act::Idle);
		pa.time_count_down = -6.0f;
		return;
	}
	if (pa.is_turn_next && pa.time_count_down < frame_turn[pa.combo_ix]) {
		current_apply(pa);
		pa.is_turn_next = false;
		if (pa.combo_ix == atk.size()-1) pa.combo_ix = -1;
		return;
	}
}
////////////////
// control_atk
////////////////
////////////////
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
	combo["sinon"].build("sinon");
}
//
template <typename T_app>
void control_atk<T_app>::init_combo_para(const size_t &index_in)
{
	combo_p[index_in];
	combo_p[index_in].model_name = *app->m_Inst.m_Stat[index_in].get_ModelName();
	combo_p[index_in].inst_ix = index_in;
}
//
template <typename T_app>
void control_atk<T_app>::perform(const size_t &index_in)
{
	if (!combo.count(*app->m_Inst.m_Stat[index_in].get_ModelName())) {
		PTR->m_Inst.m_Troll[index_in].revert_previous_state();
		return;
	}
	if (!combo_p.count(index_in)) init_combo_para(index_in);
	combo[combo_p[index_in].model_name].strike(combo_p[index_in]);
}
//
template <typename T_app>
void control_atk<T_app>::update(const float &dt)
{
	for (auto &para_it: combo_p) {
		combo[para_it.second.model_name].update(dt, para_it.second);
	}
}
//
}