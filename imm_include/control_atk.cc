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
combo_data::combo_data()
{
	;
}
//
void combo_data::build(const std::string &name)
{
	// game date
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
	if (name == "pepper") {
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
	for (auto &end: frame_end) end /= FRAME_RATE;
	for (auto &turn: frame_turn) turn /= FRAME_RATE;
}
//
void combo_data::current_apply(combo_para &pa)
{
	// deactive the previous box
	if (pa.combo_ix > 0) PTR->m_Attack.set_active_box(pa.inst_ix, atk_box[pa.combo_ix-1], false);
	pa.count_down = frame_end[pa.combo_ix];
	PTR->m_Inst.m_Stat[pa.inst_ix].check_set_ClipName(atk[pa.combo_ix], true);
	math::set_inst_speed(pa.inst_ix, frame_speed[pa.combo_ix]);
	pa.is_busy = true;
	pa.current_ix = pa.combo_ix;
	PTR->m_Attack.set_active_box(pa.inst_ix, atk_box[pa.combo_ix], true);
}
//
void combo_data::current_over(combo_para &pa)
{
	pa.is_busy = false;
	PTR->m_Control.atk.hits[pa.inst_ix].clear();
	PTR->m_Attack.deactive_box(pa.inst_ix);
}
//
void combo_data::strike(combo_para &pa)
{
	if (pa.combo_ix < 0 && pa.count_down > 0.0f) return;
	if (pa.combo_ix == -1) {
		++pa.combo_ix;
		current_apply(pa);
		return;
	}
	if (!pa.is_turn_next && pa.count_down > frame_turn[pa.combo_ix]) {
		++pa.combo_ix;
		pa.is_turn_next = true;
		return;
	}
}
//
void combo_data::update(const float &dt, combo_para &pa)
{
	auto &tro = PTR->m_Inst.m_Troll[pa.inst_ix];
	if (pa.count_down > -5.0f) pa.count_down -= dt;
	if (pa.is_busy && pa.count_down < 0.0f) {
		tro.order |= ORDER_IDLE;
		current_over(pa);
		pa.combo_ix = -1;
		return;
	}
	if (!pa.is_busy && pa.count_down < -2.0f) {
		if (tro.current_state == pose_Idle::instance())
			PTR->m_Inst.m_Stat[pa.inst_ix].check_set_ClipName(act::Idle);
		pa.count_down = -6.0f;
		return;
	}
	if (pa.is_turn_next && pa.count_down < frame_turn[pa.combo_ix]) {
		current_apply(pa);
		pa.is_turn_next = false;
		if (pa.combo_ix == atk.size()-1) pa.combo_ix = -1;
		return;
	}
}
////////////////
// damage_data
////////////////
////////////////
damage_data::damage_data():
	ix_atk(0),
	ix_dmg(0),
	combo_ix(-2),
	count_down(-1.0f),
	is_calculated(true)
{
	;
}
//
void damage_data::update(const float &dt)
{
	if (count_down > 0.0f) count_down -= dt;
	if (!is_calculated) {
		PTR->m_Inst.m_Troll[ix_dmg].order |= ORDER_DMG;
		math::set_inst_speed(ix_dmg, 0.0f);
		if (PTR->m_Inst.m_Stat[ix_dmg].type == skinned) {
			PTR->m_Inst.m_Troll[ix_atk].focus = static_cast<int>(ix_dmg);
			math::set_face_to_face(ix_atk, ix_dmg);
			PTR->m_Scene.audio.play_effect("punch0");
		}
		else {
			int focus = PTR->m_Inst.m_Troll[ix_atk].focus;
			if (!PTR->m_Control.atk.hits[ix_atk].count(focus)) {
				math::set_inst_face_to_inst2(ix_atk, ix_dmg);
			}
		}
		is_calculated = true;
	}
}
//
void damage_data::stamp()
{
	if (count_down > 0.0f) {
		return;
	}
	else {
		count_down = PTR->m_Control.atk.c_para[ix_atk].count_down;
		is_calculated = false;
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
	combo_d["sinon"].build("sinon");
	combo_d["pepper"].build("pepper");
}
//
template <typename T_app>
void control_atk<T_app>::reset()
{
	c_para.clear();
	damage.clear();
}
//
template <typename T_app>
void control_atk<T_app>::init_combo_para(const size_t &index_in)
{
	c_para[index_in];
	c_para[index_in].model_name = *app->m_Inst.m_Stat[index_in].get_ModelName();
	c_para[index_in].inst_ix = index_in;
}
//
template <typename T_app>
void control_atk<T_app>::cause_damage(const size_t &inst_ix_atk, const size_t &inst_ix_dmg)
{
	assert(inst_ix_atk < 1000);
	assert(inst_ix_dmg < 1000);
	assert(c_para[inst_ix_atk].current_ix < 100);
	assert(c_para[inst_ix_atk].current_ix > -1);
	int index =
		c_para[inst_ix_atk].current_ix +
		static_cast<int>(inst_ix_atk)*1000 +
		static_cast<int>(inst_ix_dmg)*1000000;
	if (!damage.count(index)) {
		// init
		damage[index].ix_atk = inst_ix_atk;
		damage[index].ix_dmg = inst_ix_dmg;
		damage[index].combo_ix = c_para[inst_ix_atk].current_ix;
	}
	damage[index].stamp();
}
//
template <typename T_app>
void control_atk<T_app>::execute(const size_t &index_in)
{
	if (!combo_d.count(*app->m_Inst.m_Stat[index_in].get_ModelName())) {
		PTR->m_Inst.m_Troll[index_in].revert_previous_state();
		return;
	}
	if (!c_para.count(index_in)) init_combo_para(index_in);
	combo_d[c_para[index_in].model_name].strike(c_para[index_in]);
}
//
template <typename T_app>
void control_atk<T_app>::update(const float &dt)
{
	for (auto &para_it: c_para) {
		combo_d[para_it.second.model_name].update(dt, para_it.second);
	}
	for (auto &dmg: damage) {
		dmg.second.update(dt);
	}
}
//
}