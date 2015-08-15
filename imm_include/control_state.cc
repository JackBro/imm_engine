////////////////
// control_state.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_math.h"
#include "control_state.h"
namespace imm
{
////////////////
// pose_Idle
////////////////
////////////////
pose_Idle *pose_Idle::instance()
{
	static pose_Idle instance;
	return &instance;
}
//
void pose_Idle::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Idle);
}
//
void pose_Idle::execute(troll *tro)
{
	if (tro->order & ORDER_MOVE_HIT) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_touch_ground) return;
		tro->order = ORDER_NONE;
		math::mouse_instance_move(tro->index, tro->act_speed());
		tro->change_state(pose_Move::instance());
	}
	
	if (tro->order & ORDER_MOVE_TOWARD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_touch_ground) return;
		tro->order = ORDER_NONE;
		if (PTR->m_Control.pad.is_L_active()) {
			math::pad_move_toward(tro->index, tro->act_speed());
			tro->change_state(pose_Move::instance());
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].phy.velocity_nm = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		
		
	}
	
	
	
	
	
	
}
//
void pose_Idle::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Move
////////////////
////////////////
pose_Move *pose_Move::instance()
{
	static pose_Move instance;
	return &instance;
}
//
void pose_Move::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act_move());
}
//
void pose_Move::execute(troll *tro)
{
	if (tro->order & ORDER_IDLE) {
		tro->order = ORDER_NONE;
		tro->change_state(pose_Idle::instance());
	}
	
	
	
	
	
	
	
	if (tro->order & ORDER_MOVE_HIT) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_touch_ground) return;
		tro->order = ORDER_NONE;
		math::mouse_instance_move(tro->index, tro->act_speed());
	}
	
	if (tro->order & ORDER_MOVE_TOWARD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_touch_ground) return;
		tro->order = ORDER_NONE;
		if (PTR->m_Control.pad.is_L_active()) {
			math::pad_move_toward(tro->index, tro->act_speed());
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act_move());
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].phy.velocity_nm = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tro->change_state(pose_Idle::instance());
		}
	}
	
	
	
	
	
	
}
//
void pose_Move::exit(troll *tro)
{
	tro;
}
//
























//
}