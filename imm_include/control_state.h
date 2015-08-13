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
// 
////////////////
////////////////
typedef enum ORDER_ACT_TYPE {
	ORDER_NONE     = 0x0,
	ORDER_JUMP     = 0x1,
	ORDER_WALK_RUN = 0x2,
	
	ORDER_MOVE     = 0x4,
	
	
	
} ORDER_ACT_TYPE;




////////////////
// troll
////////////////
////////////////
struct troll
{
	troll();
	void update();
	void change_state(state<troll> *new_state);
	size_t index;
	state<troll> *current_state;
	int order;
	float speed;
	
	
	
};
//
troll::troll():
	current_state(pose_Idle::instance()),
	order(ORDER_NONE),
	speed(13.5f)
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





//
}
#endif