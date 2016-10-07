////////////////
// control_state.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H
#include "control_math.h"
#include "imm_core.h"
#include <string>
namespace imm
{
////////////////
// ORDER_ACT_TYPE
////////////////
////////////////
enum ORDER_ACT_TYPE
{
	ORDER_NONE        = 0x0,
	ORDER_JUMP        = 0x1,
	ORDER_MOVE_HIT    = 0x2,
	ORDER_MOVE_TOWARD = 0x4,
	ORDER_MOVE_WASD   = 0x8,
	ORDER_IDLE        = 0x10,
	ORDER_ATK_X       = 0x20,
	ORDER_ATK_Y       = 0x40,
	ORDER_DMG         = 0x80,
	ORDER_ROLL        = 0x100,
	ORDER_GUARD       = 0x200,
	ORDER_GUARD_NO    = 0x400,
	ORDER_ENGAGE      = 0x800,
	ORDER_HITFLY      = 0x1000,
};
//
enum ORDER_STAT_TYPE
{
	ORDER_IS_CLEAR  = 0x0,
	ORDER_IS_WALK   = 0x1, // NOT: ORDER_IS_RUN
	ORDER_IS_ROLL   = 0x2,
	ORDER_IS_ENGAGE = 0x4,
	ORDER_IS_GUARD  = 0x8,
};
////////////////
// BATTLE_STAT
////////////////
////////////////
enum BATTLE_STAT
{
	BATTLE_STAT_NONE  = 0x0,
	BATTLE_STAT_DODGE = 0x1,
};
////////////////
// state
////////////////
////////////////
template <class T_troll>
struct state
{
	virtual ~state() {;}
	virtual void enter(T_troll*) = 0;
	virtual void execute(T_troll*) = 0;
	virtual void exit(T_troll*) = 0;
};
////////////////
// pose_Idle
////////////////
////////////////
struct troll;
struct pose_Idle: public state<troll>
{
	static pose_Idle *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Idle() {;}
	pose_Idle(const pose_Idle&);
	pose_Idle &operator=(const pose_Idle&);
};
////////////////
// pose_Move
////////////////
////////////////
struct troll;
struct pose_Move: public state<troll>
{
	static pose_Move *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Move() {;}
	pose_Move(const pose_Move&);
	pose_Move &operator=(const pose_Move&);
};
////////////////
// pose_Roll
////////////////
////////////////
struct troll;
struct pose_Roll: public state<troll>
{
	static pose_Roll *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Roll() {;}
	pose_Roll(const pose_Roll&);
	pose_Roll &operator=(const pose_Roll&);
};
////////////////
// pose_Jump
////////////////
////////////////
struct troll;
struct pose_Jump: public state<troll>
{
	static pose_Jump *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Jump() {;}
	pose_Jump(const pose_Jump&);
	pose_Jump &operator=(const pose_Jump&);
};
////////////////
// pose_Atk
////////////////
////////////////
struct troll;
struct pose_Atk: public state<troll>
{
	static pose_Atk *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Atk() {;}
	pose_Atk(const pose_Atk&);
	pose_Atk &operator=(const pose_Atk&);
};
////////////////
// pose_Damage
////////////////
////////////////
struct troll;
struct pose_Damage: public state<troll>
{
	static pose_Damage *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Damage() {;}
	pose_Damage(const pose_Damage&);
	pose_Damage &operator=(const pose_Damage&);
};
////////////////
// act_str
////////////////
////////////////
struct act_str
{
	act_str();
	act_str(int &order_stat);
	std::string Idle();
	std::string Run();
	std::string Jump();
	std::string JumpLand();
	std::string Engage();
	std::string Damage();
	std::string Roll();
	std::string WalkRev();
	std::string Guard();
	int *p_order_s;
};
////////////////
// action_data
////////////////
////////////////
struct action_data
{
	action_data();
	float velocity_Jump;
	float speed_Walk;
	float speed_Run;
	float speed_Roll;
	float frame_Damage;
	float frame_RollStep1;
	float frame_RollStep2;
	float frame_JumpLand;
	float cd_Idle;
	float cd_Jump;
	float cd_Damage;
	float cd_RollStep1;
	float cd_RollStep2;
	float cd_GuardMin;
};
//
action_data::action_data():
	velocity_Jump(35.0f),
	speed_Walk(4.5f),
	speed_Run(13.5f),
	speed_Roll(30.0f),
	frame_Damage(10.0f*FRAME_RATE_1DIV),
	frame_RollStep1(10.0f*FRAME_RATE_1DIV),
	frame_RollStep2(10.0f*FRAME_RATE_1DIV),
	frame_JumpLand(7.0f*FRAME_RATE_1DIV),
	cd_Idle(-1.0f),
	cd_Jump(-1.0f),
	cd_Damage(-1.0f),
	cd_RollStep1(-1.0f),
	cd_RollStep2(-1.0f),
	cd_GuardMin(-1.0f)
{
	;
}
////////////////
// troll
////////////////
////////////////
struct troll
{
	troll();
	void update();
	void change_state(state<troll> *new_state);
	void change_state_execute(state<troll> *new_state);
	void revert_previous_state();
	bool is_ai();
	float speed_move();
	state<troll> *current_state;
	state<troll> *previous_state;
	size_t index;
	int order;
	int order_stat;
	int battle_stat;
	int focus;
	bool is_on_air;
	action_data A;
	act_str act;
};
//
troll::troll():
	current_state(pose_Idle::instance()),
	previous_state(pose_Idle::instance()),
	index(0),
	order(ORDER_NONE),
	order_stat(ORDER_IS_CLEAR),
	battle_stat(BATTLE_STAT_NONE),
	focus(-1),
	is_on_air(false),
	A(),
	act(order_stat)
{
	;
}
//
void troll::update()
{
	current_state->execute(this);
}
//
void troll::change_state(state<troll> *new_state)
{
	assert(current_state && new_state);
	previous_state = current_state;
	current_state->exit(this);
	current_state = new_state;
	current_state->enter(this);
}
//
void troll::change_state_execute(state<troll> *new_state)
{
	assert(current_state && new_state);
	previous_state = current_state;
	current_state->exit(this);
	current_state = new_state;
	current_state->enter(this);
	current_state->execute(this);
}
//
void troll::revert_previous_state()
{
	change_state(previous_state);
}
//
float troll::speed_move()
{
	if (order_stat & ORDER_IS_WALK)	return A.speed_Walk;
	return A.speed_Run;
}
////////////////
// inl
////////////////
////////////////
#include "control_state_act.cc"
}
#endif