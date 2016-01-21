////////////////
// control_atk.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_atk.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// skill_data
////////////////
////////////////
skill_data::skill_data()
{
	;
}
//
void skill_data::current_apply(skill_para &pa)
{
	// deactive the previous box
	if (pa.skill_ix > 0) PTR->m_Attack.set_active_box(pa.inst_ix, atk_box[pa.skill_ix-1], false);
	pa.count_down = frame_end[pa.skill_ix];
	PTR->m_Inst.m_Stat[pa.inst_ix].check_set_ClipName(atk[pa.skill_ix], true);
	math::set_inst_speed(pa.inst_ix, frame_speed[pa.skill_ix]);
	pa.is_busy = true;
	pa.current_ix = pa.skill_ix;
	PTR->m_Attack.set_active_box(pa.inst_ix, atk_box[pa.skill_ix], true);
	if (get_skill_type(pa) == SKILL_TYPE_MAGIC) {
		PTR->m_Magic.invoke(specify[pa.current_ix]);
	}
}
//
void skill_data::current_over(skill_para &pa)
{
	pa.is_busy = false;
	PTR->m_Control.atk.hits[pa.inst_ix].clear();
	PTR->m_Attack.deactive_box(pa.inst_ix);
}
//
void skill_data::strike(skill_para &pa)
{
	if (pa.skill_ix < 0 && pa.count_down > 0.0f) return;
	if (pa.skill_ix == -1) {
		if (!chunk.count(pa.symbol)) return;
		pa.skill_ix = chunk[pa.symbol];
		current_apply(pa);
		return;
	}
	// if skill only one hit
	if (next_ix[pa.skill_ix] == -1) return;
	// if trun next hit
	if (!pa.is_turn_next && pa.count_down > 0.01f) {
		++pa.skill_ix;
		pa.is_turn_next = true;
		return;
	}
}
//
void skill_data::update(const float &dt, skill_para &pa)
{
	auto &tro = PTR->m_Inst.m_Troll[pa.inst_ix];
	if (pa.count_down > 0.0f) pa.count_down -= dt;
	if (pa.is_busy && pa.count_down < 0.0f) {
		tro.order |= ORDER_IDLE;
		current_over(pa);
		pa.skill_ix = -1;
		return;
	}
	if (pa.is_turn_next && pa.count_down < frame_turn[pa.skill_ix-1]) {
		current_apply(pa);
		pa.is_turn_next = false;
		if (next_ix[pa.skill_ix] == -1) pa.skill_ix = -1;
		return;
	}
}
//
SKILL_TYPE skill_data::get_skill_type(const skill_para &pa)
{
	if (specify[pa.current_ix] > SKILL_MELEE_UNARMED) return SKILL_TYPE_MAGIC;
	return SKILL_TYPE_MELEE;
}
////////////////
// damage_data
////////////////
////////////////
damage_data::damage_data():
	ix_atk(0),
	ix_dmg(0),
	skill_ix(-2),
	count_down(-1.0f),
	delay(-1.0f),
	is_calculated(true),
	is_delay(false),
	box_center(nullptr)
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
		if (PTR->m_Inst.m_Stat[ix_dmg].type == INST_SKINNED) {
			PTR->m_Inst.m_Troll[ix_atk].focus = static_cast<int>(ix_dmg);
			math::set_face_to_face(ix_atk, ix_dmg);
			//
			is_delay = true;
			delay = 0.03f;
		}
		is_calculated = true;
	}
	if (is_delay) {
		delay -= dt;
		if (delay < 0.0f) {
			assert(box_center);
			//hit postion roughly
			XMFLOAT3 box = *box_center;
			XMFLOAT3 center = PTR->m_Inst.m_BoundW.center(ix_dmg);
			box.x += (center.x-box.x)*0.7f;
			box.z += (center.z-box.z)*0.7f;
			PTR->m_Scene.plasma.push_back(PLASMA_STRIKE, 0.5f, box);
			PTR->m_Scene.audio.play_effect("punch00");
			is_delay = false;
		}
	}
}
//
void damage_data::stamp()
{
	if (count_down > 0.0f) {
		return;
	}
	else {
		count_down = PTR->m_Control.atk.para_ski[ix_atk].count_down;
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
void control_atk<T_app>::reset()
{
	para_ski.clear();
	damage.clear();
}
//
template <typename T_app>
void control_atk<T_app>::init_skill_para(const size_t &index_in)
{
	para_ski[index_in];
	para_ski[index_in].model_name = *app->m_Inst.m_Stat[index_in].get_ModelName();
	para_ski[index_in].inst_ix = index_in;
}
//
template <typename T_app>
void control_atk<T_app>::cause_damage(const size_t &inst_ix_atk, const size_t &inst_ix_dmg, const XMFLOAT3 &box_center)
{
	assert(inst_ix_atk < 1000);
	assert(inst_ix_dmg < 1000);
	assert(para_ski[inst_ix_atk].current_ix < 100);
	assert(para_ski[inst_ix_atk].current_ix > -1);
	int index =
		para_ski[inst_ix_atk].current_ix +
		static_cast<int>(inst_ix_atk)*1000 +
		static_cast<int>(inst_ix_dmg)*1000000;
	if (!damage.count(index)) {
		// init
		damage[index].ix_atk = inst_ix_atk;
		damage[index].ix_dmg = inst_ix_dmg;
		damage[index].skill_ix = para_ski[inst_ix_atk].current_ix;
		damage[index].box_center = &box_center;
	}
	damage[index].stamp();
	hits[inst_ix_atk].insert(inst_ix_dmg);
}
//
template <typename T_app>
void control_atk<T_app>::execute(const size_t &index_in, const char &symbol)
{
	if (!data_ski.count(*app->m_Inst.m_Stat[index_in].get_ModelName())) {
		PTR->m_Inst.m_Troll[index_in].order |= ORDER_IDLE;
		return;
	}
	if (!para_ski.count(index_in)) init_skill_para(index_in);
	if (para_ski[index_in].skill_ix == -1) para_ski[index_in].symbol = symbol;
	data_ski[para_ski[index_in].model_name].strike(para_ski[index_in]);
}
//
template <typename T_app>
void control_atk<T_app>::update(const float &dt)
{
	for (auto &para_it: para_ski) {
		data_ski[para_it.second.model_name].update(dt, para_it.second);
	}
	for (auto &dmg: damage) {
		dmg.second.update(dt);
	}
}
////////////////
// inl
////////////////
////////////////
#include "control_atk_build.cc"
}