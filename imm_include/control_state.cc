////////////////
// control_state.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_state.h"
#include "imm_ptr.h"
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
	if (tro->previous_state == pose_Atk::instance() ||
		tro->previous_state == pose_Damage::instance()) {
		tro->count_down = 5.0f+tro->index%3;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::BattleReady);
	}
	else {
		tro->count_down = -1.0f;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Idle);
	}
}
//
void pose_Idle::execute(troll *tro)
{
	if (tro->order & ORDER_MOVE_HIT) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) return;
		tro->order = ORDER_NONE;
		if (tro->index != PTR->m_Control.player1) math::ai_move_pos(tro->index, tro->speed_move());
		else math::mouse_inst_move(tro->index, tro->speed_move());
		tro->change_state(pose_Move::instance());
	}
	if (tro->order & ORDER_MOVE_TOWARD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) return;
		tro->order ^= ORDER_MOVE_TOWARD;
		if (PTR->m_Control.pad.is_L_active()) {
			math::pad_move_toward(tro->index, tro->speed_move());
			tro->change_state(pose_Move::instance());
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}
	if (tro->order & ORDER_MOVE_WASD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) return;
		tro->order ^= ORDER_MOVE_WASD;
		if (math::key_move_wasd(tro->index, tro->speed_move())) tro->change_state(pose_Move::instance());
	}
	if (tro->order & ORDER_JUMP) {
		tro->order = ORDER_NONE;
		if (PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) {
			tro->change_state(pose_Jump::instance());
		}
	}
	if (tro->order & ORDER_ATK_X || tro->order & ORDER_ATK_Y) {
		tro->change_state(pose_Atk::instance());
	}
	if (tro->order & ORDER_DMG) {
		tro->order = ORDER_NONE;
		tro->change_state(pose_Damage::instance());
	}
	// 
	if (tro->current_state != pose_Idle::instance()) return;
	if ((tro->count_down > 0.0f)) tro->count_down -= PTR->m_Timer.delta_time();
	if (tro->count_down < 0.0f && tro->count_down > -0.9f) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Idle);
		tro->count_down = -1.0f;
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
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) return;
		tro->order = ORDER_NONE;
		if (tro->index != PTR->m_Control.player1) math::ai_move_pos(tro->index, tro->speed_move());
		else math::mouse_inst_move(tro->index, tro->speed_move());
		tro->change_state(pose_Move::instance());
	}
	if (tro->order & ORDER_MOVE_TOWARD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) return;
		tro->order ^= ORDER_MOVE_TOWARD;
		if (PTR->m_Control.pad.is_L_active()) {
			math::pad_move_toward(tro->index, tro->speed_move());
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act_move());
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tro->change_state(pose_Idle::instance());
		}
	}
	if (tro->order & ORDER_MOVE_WASD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) return;
		tro->order ^= ORDER_MOVE_WASD;
		if (!math::key_move_wasd(tro->index, tro->speed_move())) tro->change_state(pose_Idle::instance());
		else PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act_move());
	}
	if (tro->order & ORDER_JUMP) {
		tro->order = ORDER_NONE;
		if (PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground) {
			tro->change_state(pose_Jump::instance());
		}
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
	PTR->m_Inst.m_Stat[tro->index].phy.velocity.y = tro->velocity_jump;
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Jump);
	tro->count_down = 60.0f;
}
//
void pose_Jump::execute(troll *tro)
{
	bool is_on_ground = PTR->m_Inst.m_Stat[tro->index].phy.is_on_ground;
	if (!tro->is_on_air && !is_on_ground && tro->count_down > 59.0f) {
		tro->is_on_air = true;
	}
	if (tro->is_on_air && is_on_ground) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::JumpGround, true);
		tro->is_on_air = false;
		// Idle do full JumpGround
		if (tro->previous_state == pose_Idle::instance()) tro->count_down = 0.3f;
		else tro->count_down = 0.1f;
	}
	if (!tro->is_on_air && is_on_ground && tro->count_down < 59.0f) {
		if (tro->count_down > 0.0f) {
			tro->count_down -= PTR->m_Timer.delta_time();
		}
		else {
			tro->revert_previous_state();
		}
	}
	if (tro->order & ORDER_JUMP && tro->count_down > 0.3f) {
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
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::BattleReady);
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
		tro->order = ORDER_NONE;
		tro->change_state(pose_Idle::instance());
	}
	if (tro->order & ORDER_DMG) {
		tro->order = ORDER_NONE;
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
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Damage, true);
	tro->count_down = 7.0f/FRAME_RATE;
}
//
void pose_Damage::execute(troll *tro)
{
	tro->count_down -= PTR->m_Timer.delta_time();
	if (tro->count_down < 0.0f) {
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