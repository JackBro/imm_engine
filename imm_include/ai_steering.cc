////////////////
// ai_steering.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "ai_steering.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// steering
////////////////
////////////////
//
void steering::update(const float &dt)
{
	// clear old data
	if (attack.size() > 100) attack.erase(attack.begin()+50);
	if (damage.size() > 100) damage.erase(damage.begin()+50);
	if (PTR->m_Control.player1 == index || !is_active) return;
	update_dt += dt;
	action_dt += dt;
	if (update_dt > AI_DELTA_TIME_PHY_SLOW) {
		update_dt -= AI_DELTA_TIME_PHY_SLOW;
		current_state->execute(this);
	}
}
////////////////
// ai_Standby
////////////////
////////////////
ai_Standby *ai_Standby::instance()
{
	static ai_Standby instance;
	return &instance;
}
//
void ai_Standby::enter(steering *ste)
{
	ste;
}
//
void ai_Standby::execute(steering *ste)
{
	if (ste->tactics & AI_TAC_SEEK) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Seek::instance());
	}
	if (ste->tactics & AI_TAC_ATK) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Atk::instance());
	}
	if (ste->tactics & AI_TAC_PATROL) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Patrol::instance());
	}	
}
//
void ai_Standby::exit(steering *ste)
{
	ste;
}
////////////////
// ai_Patrol
////////////////
////////////////
ai_Patrol *ai_Patrol::instance()
{
	static ai_Patrol instance;
	return &instance;
}
//
void ai_Patrol::enter(steering *ste)
{
	ste;
}
//
void ai_Patrol::execute(steering *ste)
{
	if (PTR->m_Inst.m_Steering[ste->index].alert[ste->target]) {
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_IDLE;
		ste->report |= AI_REP_ALERT;
		ste->change_state(ai_Standby::instance());
	}
	if (ste->action_dt > ste->random_t) {
		XMFLOAT3 random_pos = PTR->m_Inst.m_BoundW.center(ste->index);
		random_pos.x += math::calc_randf(5.0f, 15.0f)*(rand()%2 ? 1.0f : -1.0f);
		random_pos.z += math::calc_randf(5.0f, 15.0f)*(rand()%2 ? 1.0f : -1.0f);
		ste->desired_pos = random_pos;
		PTR->m_Inst.m_Troll[ste->index].order_stat |= ORDER_IS_WALK;
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_MOVE_HIT;
		ste->action_dt = 0.0f;
		ste->random_t = math::calc_randf(10.0f, 20.0f);
	}
}
//
void ai_Patrol::exit(steering *ste)
{
	if (PTR->m_Inst.m_Troll[ste->index].order_stat & ORDER_IS_WALK) {
		PTR->m_Inst.m_Troll[ste->index].order_stat ^= ORDER_IS_WALK;
	}
}
////////////////
// ai_Seek
////////////////
////////////////
ai_Seek *ai_Seek::instance()
{
	static ai_Seek instance;
	return &instance;
}
//
void ai_Seek::enter(steering *ste)
{
	ste;
}
//
void ai_Seek::execute(steering *ste)
{
	if (ste->count_down < 0.0f) {
		ste->desired_pos = PTR->m_Inst.m_BoundW.center(ste->target);
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_MOVE_HIT;
		ste->count_down = 1.0f;
	}
	if (PTR->m_Control.map_stop[ste->index].is_stop) {
		if (!ste->close[ste->target]) ste->count_down -= AI_DELTA_TIME_PHY_SLOW;
	}
	if (PTR->m_Control.map_stop[ste->index].last_time > 5.0f) {
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_IDLE;
		ste->count_down = -1.0f;
	}
	if (PTR->m_Inst.m_Steering[ste->index].close[ste->target]) {
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_IDLE;
		ste->report |= AI_REP_TAR_CLOSE;
		ste->change_state(ai_Standby::instance());
		return;
	}
}
//
void ai_Seek::exit(steering *ste)
{
	ste;
}
////////////////
// ai_Atk
////////////////
////////////////
ai_Atk *ai_Atk::instance()
{
	static ai_Atk instance;
	return &instance;
}
//
void ai_Atk::enter(steering *ste)
{
	ste;
}
//
void ai_Atk::execute(steering *ste)
{
	if (PTR->m_Inst.m_Troll[ste->index].current_state == pose_Damage::instance()) {
		return;
	}
	// wait pose_Idle
	if (PTR->m_Inst.m_Troll[ste->index].current_state == pose_Move::instance()) {
		return;
	}
	if (PTR->m_Control.atk.is_execute(ste->index)) {
		return;
	}
	if (PTR->m_Inst.m_Troll[ste->index].current_state != pose_Atk::instance()) {
		if (ste->action_dt > AI_DELTA_TIME_PHY_SLOW*3.0f) {
			ste->action_dt = 0.0f;
			math::set_inst_face_to_inst2(ste->index, ste->target);
		}
	}
	if (!PTR->m_Inst.m_Steering[ste->index].close[ste->target]) {
		ste->report |= AI_REP_TAR_AWAY;
		ste->change_state(ai_Standby::instance());
		return;
	}
	else {
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_ATK_X;
		return;
	}
}
//
void ai_Atk::exit(steering *ste)
{
	ste;
}
//
}