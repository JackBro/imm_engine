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
void steering::update()
{
	// clear old data
	if (attack.size() > 100) attack.erase(attack.begin()+50);
	if (damage.size() > 100) damage.erase(damage.begin()+50);
	if (PTR->m_Control.player1 == index || !is_active) return;
	current_state->execute(this);
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
	if (ste->tactics & AI_TAC_CLOSETO) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_CloseTo::instance());
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
	ste;
}
//
void ai_Patrol::exit(steering *ste)
{
	ste;
}
////////////////
// ai_CloseTo
////////////////
////////////////
ai_CloseTo *ai_CloseTo::instance()
{
	static ai_CloseTo instance;
	return &instance;
}
//
void ai_CloseTo::enter(steering *ste)
{
	ste;
}
//
void ai_CloseTo::execute(steering *ste)
{
	if (ste->count_down < 0.0f) {
		XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[ste->target].get_World();
		ste->desired_pos.x = world._41;
		ste->desired_pos.z = world._43;
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_MOVE_HIT;
		ste->count_down = 1.0f;
	}
	if (PTR->m_Control.map_stop[ste->index].is_stop) {
		if (!ste->touch[ste->target]) ste->count_down -= PTR->m_Timer.delta_time();
	}
	if (PTR->m_Inst.m_Steering[ste->index].touch[ste->target]) {
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_IDLE;
		ste->report |= AI_REP_CLOSETO;
	}
	if (ste->tactics & AI_TAC_ATK) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Atk::instance());
	}
}
//
void ai_CloseTo::exit(steering *ste)
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
	ste;
	PTR->m_Inst.m_Troll[ste->index].order |= ORDER_ATK_X;
}
//
void ai_Atk::exit(steering *ste)
{
	ste;
}
//
}