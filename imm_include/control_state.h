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
};
//
enum ORDER_STAT_TYPE
{
	ORDER_IS_CLEAR = 0x0,
	ORDER_IS_WALK  = 0x1, // NOT: ORDER_IS_RUN
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
// act
////////////////
////////////////
struct act
{
	static std::string Idle;
	static std::string Walk;
	static std::string Run;
	static std::string Jump;
	static std::string JumpGround;
	static std::string BattleReady;
	static std::string Damage;
};
//
std::string act::Idle        = "Idle";
std::string act::Walk        = "Walk";
std::string act::Run         = "Run";
std::string act::Jump        = "Jump";
std::string act::JumpGround  = "JumpGround";
std::string act::BattleReady = "BattleReady";
std::string act::Damage      = "Damage";
////////////////
// troll
////////////////
////////////////
struct troll
{
	troll();
	void update();
	void change_state(state<troll> *new_state);
	void revert_previous_state();
	float speed_move();
	std::string &act_move();
	state<troll> *current_state;
	state<troll> *previous_state;
	size_t index;
	int order;
	int order_stat;
	int focus;
	bool is_on_air;
	float velocity_jump;
	float count_down;
};
//
troll::troll():
	current_state(pose_Idle::instance()),
	previous_state(pose_Idle::instance()),
	index(0),
	order(ORDER_NONE),
	order_stat(ORDER_IS_CLEAR),
	focus(-1),
	is_on_air(false),
	velocity_jump(35.0f),
	count_down(-1.0f)
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
void troll::revert_previous_state()
{
	change_state(previous_state);
}
//
float troll::speed_move()
{
	if (order_stat & ORDER_IS_WALK)	return 4.5f;
	return 13.5f;
}
//
std::string &troll::act_move()
{
	if (order_stat & ORDER_IS_WALK) return act::Walk;
	return act::Run;
}
//
}
#endif