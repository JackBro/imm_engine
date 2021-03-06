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
	if (tro->order & ORDER_ENGAGE) {
		tro->order ^= ORDER_ENGAGE;
		tro->A.cd_Idle = 3.0f+math::calc_randf(-1.0f, 1.0f);
		tro->order_stat |= ORDER_IS_ENGAGE;
	}
	if (tro->order & ORDER_IDLE) {
		tro->order ^= ORDER_IDLE;
	}
	if (!PTR->m_Inst.m_Stat[tro->index].is_in_switch_clip()) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Idle());
	}
}
//
void pose_Idle::execute(troll *tro)
{
	if (tro->order_stat & ORDER_IS_ENGAGE) {
		tro->A.cd_Idle -= PTR->m_Timer.delta_time();
		if (tro->A.cd_Idle < 0.0f) {
			tro->order_stat ^= ORDER_IS_ENGAGE;
			PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
		}
	}
	if (tro->order & ORDER_JUMP) {
		tro->change_state_execute(pose_Jump::instance());
		return;
	}
	if (tro->order & ORDER_ATK_X || tro->order & ORDER_ATK_Y) {
		tro->change_state_execute(pose_Atk::instance());
		return;
	}
	if (tro->order & ORDER_DMG) {
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	if (tro->order & ORDER_GUARD) {
		tro->order = ORDER_NONE;
		tro->A.cd_GuardMin -= PTR->m_Timer.delta_time();
		if (tro->A.cd_GuardMin > 0.0f) return;
		if (tro->order_stat & ORDER_IS_GUARD) return;
		tro->order_stat |= ORDER_IS_GUARD;
		tro->A.cd_GuardMin = FPS60_1DIV;
		PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
		return;
	}
	if (tro->order & ORDER_GUARD_NO) {
		tro->order = ORDER_NONE;
		if (!(tro->order_stat & ORDER_IS_GUARD)) return;
		tro->order_stat ^= ORDER_IS_GUARD;
		tro->order_stat |= ORDER_IS_ENGAGE;
		tro->A.cd_Idle = 3.0f;
		PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
		tro->A.cd_GuardMin = -1.0f;
		return;
	}
	if (tro->order & ORDER_MOVE_HIT) {
		tro->change_state_execute(pose_Move::instance());
		return;
	}
	if (tro->order & ORDER_MOVE_TOWARD) {
		tro->change_state_execute(pose_Move::instance());
		return;
	}
	if (tro->order & ORDER_MOVE_WASD) {
		tro->change_state_execute(pose_Move::instance());
		return;
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
	tro;
}
//
void pose_Move::execute(troll *tro)
{
	if (tro->order & ORDER_DMG) {
		math::ai_move_pos_stop(tro->index);
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	if (tro->order & ORDER_IDLE) {
		if (tro->is_ai()) math::ai_move_pos_stop(tro->index);
		tro->change_state(pose_Idle::instance());
		return;
	}
	// just for test
	if (tro->order & ORDER_MOVE_HIT) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		tro->order = ORDER_NONE;
		if (tro->is_ai()) math::ai_move_pos(tro->index, tro->speed_move());
		else math::mouse_inst_move(tro->index, tro->speed_move());
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
		return;
	}
	if (tro->order & ORDER_MOVE_TOWARD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		// roll
		if (tro->order & ORDER_ROLL) {
			tro->change_state_execute(pose_Roll::instance());
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
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		// roll
		if (tro->order & ORDER_ROLL) {
			tro->change_state_execute(pose_Roll::instance());
			return;
		}
		//
		tro->order ^= ORDER_MOVE_WASD;
		if (!math::key_move_wasd(tro->index, tro->speed_move())) tro->change_state(pose_Idle::instance());
		else PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
	}
	if (tro->order & ORDER_JUMP) {
		tro->change_state_execute(pose_Jump::instance());
		return;
	}
	if (tro->order & ORDER_ATK_X || tro->order & ORDER_ATK_Y) {
		tro->change_state(pose_Atk::instance());
		return;
	}
}
//
void pose_Move::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Roll
////////////////
////////////////
pose_Roll *pose_Roll::instance()
{
	static pose_Roll instance;
	return &instance;
}
//
void pose_Roll::enter(troll *tro)
{
	tro;
}
//
void pose_Roll::execute(troll *tro)
{
	if (tro->order & ORDER_DMG) {
		tro->order ^= ORDER_DMG;
	}
	if (tro->order & ORDER_MOVE_WASD || tro->order & ORDER_MOVE_TOWARD) {
		// roll
		if (tro->A.cd_RollStep > 0.0f) {
			tro->A.cd_RollStep -= PTR->m_Timer.delta_time();
			return;
		}
		if (tro->A.cd_RollToIdle > 0.0f) {
			tro->A.cd_RollToIdle -= PTR->m_Timer.delta_time();
			math::key_move_wasd(tro->index, 0.0f);
			tro->A.cd_RollStep = tro->A.cd_RollToIdle;
			tro->A.cd_RollToIdle = -1.0f;
			tro->battle_stat ^= BATTLE_STAT_DODGE;
			tro->order |= ORDER_ENGAGE;
			return;
		}
		// roll
		if (tro->order & ORDER_ROLL) {
			tro->order ^= ORDER_ROLL;
			if (!PTR->m_AiAttr.is_required_ap(SKILL_STAMINA_ROLL, tro->index)) {
				tro->revert_previous_state();
				return;
			}
			if (tro->order & ORDER_MOVE_WASD) {
				if (!math::key_move_wasd(tro->index, tro->A.speed_Roll)) {
					tro->change_state(pose_Idle::instance());
					return;
				}
			}
			if (tro->order & ORDER_MOVE_TOWARD) {
				if (PTR->m_Control.pad.is_L_active()) {
					math::pad_move_toward(tro->index, tro->A.speed_Roll);
				}
				else {
					tro->change_state(pose_Idle::instance());
					return;
				}
			}
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Roll(), true);
			tro->A.cd_RollStep = tro->A.frame_RollStep;
			tro->A.cd_RollToIdle = tro->A.frame_RollToIdle;
			tro->battle_stat |= BATTLE_STAT_DODGE;
			return;
		}
		if (tro->order & ORDER_ENGAGE) {
			tro->change_state(pose_Idle::instance());
			return;
		}
		return;
	}
}
//
void pose_Roll::exit(troll *tro)
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
	tro->A.cd_Jump = TIME_1_MINITE;
}
//
void pose_Jump::execute(troll *tro)
{
	if (tro->order & ORDER_DMG) {
		tro->order ^= ORDER_DMG;
	}
	//
	bool is_on_land = PTR->m_Inst.m_Stat[tro->index].phy.is_on_land;
	if (!tro->is_ON_AIR && !is_on_land && tro->A.cd_Jump > TIME_59_SECONDS) {
		tro->is_ON_AIR = true;
	}
	if (tro->is_ON_AIR && is_on_land) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.JumpLand(), true);
		tro->is_ON_AIR = false;
		PTR->m_Inst.m_Stat[tro->index].phy.friction_rev_give = 0.01f;
		// Idle do full JumpLand
		if (tro->previous_state == pose_Idle::instance()) tro->A.cd_Jump = tro->A.frame_JumpLand;
		else tro->A.cd_Jump = 0.1f;
	}
	if (!tro->is_ON_AIR && is_on_land && tro->A.cd_Jump < TIME_59_SECONDS) {
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
		return;
	}
	if (tro->order & ORDER_ATK_Y) {
		tro->order = ORDER_NONE;
		PTR->m_Control.atk.execute(tro->index, 'B');
		return;
	}
	if (tro->order & ORDER_IDLE) {
		PTR->m_Inst.m_Stat[tro->index].set_sequence_ClipName(tro->act.Engage());
		tro->change_state(pose_Idle::instance());
		return;
	}
	if (tro->order & ORDER_DMG) {
		tro->change_state_execute(pose_Damage::instance());
		return;
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
	if (tro->order & ORDER_HITFLY) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.DamageFly(), true);
		tro->A.cd_Damage = tro->A.frame_DamageFly;
		tro->is_DOWN = true;
	}
	else {
		if (tro->order_stat & ORDER_IS_GUARD) {
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), true);
			tro->A.cd_Damage = tro->A.frame_Damage;
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), true);
			tro->A.cd_Damage = tro->A.frame_Damage;
		}
	}
	tro->order = ORDER_NONE;
}
//
void pose_Damage::execute(troll *tro)
{
	tro->A.cd_Damage -= PTR->m_Timer.delta_time();
	if (tro->A.cd_Damage < 0.0f) {
		if (tro->is_DOWN) {
			tro->change_state(pose_FallDown::instance());
		}
		else {
			tro->order |= ORDER_ENGAGE;
			tro->change_state(pose_Idle::instance());
		}
	}
}
//
void pose_Damage::exit(troll *tro)
{
	tro;
}
////////////////
// pose_FallDown
////////////////
////////////////
pose_FallDown *pose_FallDown::instance()
{
	static pose_FallDown instance;
	return &instance;
}
//
void pose_FallDown::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.LieDown());
	tro->is_DOWN = true;
	tro->is_GET_UP = false;
	tro->A.cd_LieDown = 1.0f;
	tro->A.cd_GetUp = tro->A.frame_GetUp;
}
//
void pose_FallDown::execute(troll *tro)
{
	if (tro->A.cd_LieDown > 0.0f) {
		tro->A.cd_LieDown -= PTR->m_Timer.delta_time();
	}
	else if (tro->A.cd_GetUp > 0.0f) {
		// get up
		tro->A.cd_GetUp -= PTR->m_Timer.delta_time();
		if (!tro->is_GET_UP) {
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.GetUp(), true);
			tro->is_GET_UP = true;
		}
	}
	if (tro->A.cd_GetUp < 0.0f) {
		// return idle
		tro->order = ORDER_NONE;
		tro->order |= ORDER_ENGAGE;
		tro->change_state(pose_Idle::instance());
	}
}
//
void pose_FallDown::exit(troll *tro)
{
	tro->is_DOWN = false;
}
//
}
