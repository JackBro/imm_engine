////////////////
// control_state.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_state.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// troll
////////////////
////////////////
bool troll::is_ai()
{
	if (index == PTR->m_Control.player1) return false;
	return true;
}
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
	if (tro->order_stat & ORDER_IS_ENGAGE) {
		tro->order_stat ^= ORDER_IS_ENGAGE;
		tro->A.cd_Idle = 5.0f+tro->index%3;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Engage());
	}
	else {
		tro->A.cd_Idle = -1.0f;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Idle());
	}
	if (tro->order & ORDER_IDLE) {
		tro->order = ORDER_NONE;
	}
}
//
void pose_Idle::execute(troll *tro)
{
	if (tro->order & ORDER_MOVE_HIT) {
		tro->change_state_execute(pose_Move::instance());
	}
	if (tro->order & ORDER_MOVE_TOWARD) {
		tro->change_state_execute(pose_Move::instance());
	}
	if (tro->order & ORDER_MOVE_WASD) {
		tro->change_state_execute(pose_Move::instance());
	}
	if (tro->order & ORDER_JUMP) {
		tro->change_state_execute(pose_Jump::instance());
	}
	if (tro->order & ORDER_ATK_X || tro->order & ORDER_ATK_Y) {
		tro->change_state_execute(pose_Atk::instance());
	}
	if (tro->order & ORDER_DMG) {
		tro->change_state(pose_Damage::instance());
	}
	if (tro->order & ORDER_GUARD) {
		tro->order = ORDER_NONE;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Idle());
	}
	if (tro->order & ORDER_GUARD_NO) {
		tro->order = ORDER_NONE;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Idle());
	}
	// Engage to Idle
	if (tro->current_state != pose_Idle::instance()) return;
	if ((tro->A.cd_Idle > 0.0f)) tro->A.cd_Idle -= PTR->m_Timer.delta_time();
	if (tro->A.cd_Idle < 0.0f && tro->A.cd_Idle > -0.9f) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Idle());
		tro->A.cd_Idle = -1.0f;
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
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
}
//
void pose_Move::execute(troll *tro)
{
	if (tro->order & ORDER_DMG) {
		math::ai_move_pos_stop(tro->index);
		tro->change_state(pose_Damage::instance());
	}
	if (tro->order & ORDER_IDLE) {
		if (tro->is_ai()) math::ai_move_pos_stop(tro->index);
		tro->change_state(pose_Idle::instance());
	}
	// just for test
	if (tro->order & ORDER_MOVE_HIT) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		tro->order = ORDER_NONE;
		if (tro->is_ai()) math::ai_move_pos(tro->index, tro->speed_move());
		else math::mouse_inst_move(tro->index, tro->speed_move());
	}
	if (tro->order & ORDER_MOVE_TOWARD) {
		// roll
		if (tro->A.cd_RollStep1 > 0.0f) {
			tro->A.cd_RollStep1 -= PTR->m_Timer.delta_time();
			return;
		}
		if (tro->A.cd_RollStep2 > 0.0f) {
			tro->A.cd_RollStep2 -= PTR->m_Timer.delta_time();
			PTR->m_Inst.m_Stat[tro->index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tro->A.cd_RollStep1 = tro->A.cd_RollStep2;
			tro->A.cd_RollStep2 = -1.0f;
			tro->order_stat |= ORDER_IS_ENGAGE;
			return;
		}
		//
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		// roll
		if (PTR->m_Control.pad.is_LB_press()) {
			tro->order ^= ORDER_ROLL;
			if (PTR->m_Control.pad.is_L_active()) {
				math::pad_move_toward(tro->index, tro->A.speed_Roll);
				PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Roll(), true);
				tro->A.cd_RollStep1 = tro->A.frame_RollStep1;
				tro->A.cd_RollStep2 = tro->A.frame_RollStep2;
				return;
			}
		}
		if (tro->order_stat & ORDER_IS_ENGAGE) {
			tro->change_state(pose_Idle::instance());
			return;
		}
		//
		tro->order ^= ORDER_MOVE_TOWARD;
		if (PTR->m_Control.pad.is_L_active()) {
			math::pad_move_toward(tro->index, tro->speed_move());
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tro->change_state(pose_Idle::instance());
		}
	}
	if (tro->order & ORDER_MOVE_WASD) {
		// roll
		if (tro->A.cd_RollStep1 > 0.0f) {
			tro->A.cd_RollStep1 -= PTR->m_Timer.delta_time();
			return;
		}
		if (tro->A.cd_RollStep2 > 0.0f) {
			tro->A.cd_RollStep2 -= PTR->m_Timer.delta_time();
			math::key_move_wasd(tro->index, 0.0f);
			tro->A.cd_RollStep1 = tro->A.cd_RollStep2;
			tro->A.cd_RollStep2 = -1.0f;
			tro->order_stat |= ORDER_IS_ENGAGE;
			return;
		}
		//
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		// roll
		if (tro->order & ORDER_ROLL) {
			tro->order ^= ORDER_ROLL;
			if (!math::key_move_wasd(tro->index, tro->A.speed_Roll)) return;
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Roll(), true);
			tro->A.cd_RollStep1 = tro->A.frame_RollStep1;
			tro->A.cd_RollStep2 = tro->A.frame_RollStep2;
			return;
		}
		if (tro->order_stat & ORDER_IS_ENGAGE) {
			tro->change_state(pose_Idle::instance());
			return;
		}
		//
		tro->order ^= ORDER_MOVE_WASD;
		if (!math::key_move_wasd(tro->index, tro->speed_move())) tro->change_state(pose_Idle::instance());
		else PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
	}
	if (tro->order & ORDER_JUMP) {
		tro->change_state_execute(pose_Jump::instance());
	}
	if (tro->order & ORDER_ATK_X || tro->order & ORDER_ATK_Y) {
		tro->change_state(pose_Atk::instance());
	}
}
//
void pose_Move::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Jump
////////////////
////////////////
pose_Jump *pose_Jump::instance()
{
	static pose_Jump instance;
	return &instance;
}
//
void pose_Jump::enter(troll *tro)
{
	tro->order = ORDER_NONE;
	if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) tro->revert_previous_state();
	PTR->m_Inst.m_Stat[tro->index].phy.velocity.y = tro->A.velocity_Jump;
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Jump());
	tro->A.cd_Jump = 60.0f;
}
//
void pose_Jump::execute(troll *tro)
{
	bool is_on_land = PTR->m_Inst.m_Stat[tro->index].phy.is_on_land;
	if (!tro->is_on_air && !is_on_land && tro->A.cd_Jump > 59.0f) {
		tro->is_on_air = true;
	}
	if (tro->is_on_air && is_on_land) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.JumpLand(), true);
		tro->is_on_air = false;
		// Idle do full JumpLand
		if (tro->previous_state == pose_Idle::instance()) tro->A.cd_Jump = tro->A.frame_JumpLand;
		else tro->A.cd_Jump = 0.1f;
	}
	if (!tro->is_on_air && is_on_land && tro->A.cd_Jump < 59.0f) {
		if (tro->A.cd_Jump > 0.0f) {
			tro->A.cd_Jump -= PTR->m_Timer.delta_time();
		}
		else {
			tro->revert_previous_state();
		}
	}
	if (tro->order & ORDER_JUMP && tro->A.cd_Jump > tro->A.frame_JumpLand) {
		tro->order = ORDER_NONE;
	}
}
//
void pose_Jump::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Atk
////////////////
////////////////
pose_Atk *pose_Atk::instance()
{
	static pose_Atk instance;
	return &instance;
}
//
void pose_Atk::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Engage());
}
//
void pose_Atk::execute(troll *tro)
{
	if (tro->order & ORDER_ATK_X) {
		tro->order = ORDER_NONE;
		PTR->m_Control.atk.execute(tro->index, 'A');
	}
	if (tro->order & ORDER_ATK_Y) {
		tro->order = ORDER_NONE;
		PTR->m_Control.atk.execute(tro->index, 'B');
	}
	if (tro->order & ORDER_IDLE) {
		tro->change_state(pose_Idle::instance());
	}
	if (tro->order & ORDER_DMG) {
		tro->change_state(pose_Damage::instance());
	}
}
//
void pose_Atk::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Damage
////////////////
////////////////
pose_Damage *pose_Damage::instance()
{
	static pose_Damage instance;
	return &instance;
}
//
void pose_Damage::enter(troll *tro)
{
	tro->order = ORDER_NONE;
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), true);
	tro->A.cd_Damage = tro->A.frame_Damage;
}
//
void pose_Damage::execute(troll *tro)
{
	tro->A.cd_Damage -= PTR->m_Timer.delta_time();
	if (tro->A.cd_Damage < 0.0f) {
		tro->order_stat |= ORDER_IS_ENGAGE;
		tro->change_state(pose_Idle::instance());
	}
}
//
void pose_Damage::exit(troll *tro)
{
	tro;
}
//
}
