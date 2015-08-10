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
	virtual void enter(T_troll*, size_t) = 0;
	virtual void execute(T_troll*, size_t) = 0;
	virtual void exit(T_troll*, size_t) = 0;
};
////////////////
// pose_Idle
////////////////
////////////////
struct troll;
struct pose_Idle: public state<troll>
{
	static pose_Idle *instance();
	void enter(troll*, size_t);
	void execute(troll*, size_t);
	void exit(troll*, size_t);
private:
	pose_Idle() {;}
	pose_Idle(const pose_Idle&);
	pose_Idle &operator=(const pose_Idle&);
};
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
};
//
troll::troll():
	current_state(pose_Idle::instance())
{
	;
}
//
void troll::update()
{
	current_state->execute(this, index);
}
//
void troll::change_state(state<troll> *new_state)
{
	assert(current_state && new_state);
	current_state->exit(this, index);
	current_state = new_state;
	current_state->enter(this, index);
}
//
}
#endif