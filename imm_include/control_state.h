////////////////
// control_state.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "imm_app.h"
#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H
namespace imm
{
////////////////
// PTR
////////////////
////////////////
class imm_app;
static imm_app *PTR;
void ptr_app_init(imm_app *app_in)
{
	PTR = app_in;
}
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
	pose_Move(const pose_Idle&);
	pose_Move &operator=(const pose_Idle&);
};
////////////////
// ORDER_ACT_TYPE
////////////////
////////////////
enum ORDER_ACT_TYPE {
	ORDER_NONE        = 0x0,
	ORDER_JUMP        = 0x1,
	ORDER_MOVE_HIT    = 0x2,
	ORDER_MOVE_TOWARD = 0x4,
	ORDER_IDLE        = 0x8,
};
//
enum ORDER_STAT_TYPE {
	ORDER_IS_CLEAR = 0x0,
	ORDER_IS_WALK  = 0x1,
	
	
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
};
//
std::string act::Idle = "Idle";
std::string act::Walk = "Walk";
std::string act::Run  = "Run";
std::string act::Jump = "Jump";
////////////////
// troll
////////////////
////////////////
struct troll
{
	troll();
	void update();
	void change_state(state<troll> *new_state);
	float act_speed();
	std::string &act_move();
	state<troll> *current_state;
	size_t index;
	int order;
	int order_stat;
	
	
};
//
troll::troll():
	current_state(pose_Idle::instance()),
	index(0),
	order(ORDER_NONE),
	order_stat(0x0)
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
	current_state->exit(this);
	current_state = new_state;
	current_state->enter(this);
}
//
float troll::act_speed()
{
	if (order_stat & ORDER_IS_WALK)	return 4.5f;
	return 13.5f;
}
std::string &troll::act_move()
{
	if (order_stat & ORDER_IS_WALK) return act::Walk;
	return act::Run;
}






//
}
#endif