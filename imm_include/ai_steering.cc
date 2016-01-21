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
	ste;
	//PTR->m_Inst.m_Troll[ste->index].order |= ORDER_JUMP;
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[PTR->m_Control.player1].get_World();
	ste->desired_pos.x = world._41;
	ste->desired_pos.z = world._43;
	static float cnt = -1.0f;
	static int touch = 0;
	if (cnt < 0.0f) {
		PTR->m_Inst.m_Troll[ste->index].order |= ORDER_MOVE_HIT;
		cnt = 1.0f;
	}
	if (ste->sensor[PTR->m_Inst.m_NameMap["sinon"]]) touch = 1;
	if (PTR->m_Control.map_stop[ste->index].is_stop) {
		if (touch == 0) cnt -= PTR->m_Timer.delta_time();
	}
}
//
void ai_Standby::exit(steering *ste)
{
	ste;
}
//
}