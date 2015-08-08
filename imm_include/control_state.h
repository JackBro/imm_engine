////////////////
// control_state.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H
namespace imm
{
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
// troll
////////////////
////////////////
struct troll
{
	troll();
	void update();
	void change_state(state<troll> *new_state);
	state<troll> *current_state;
};
//
troll::troll():
	current_state(nullptr)
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
// pose_Idle
////////////////
////////////////
struct pose_Idle: public state<troll>
{
	static pose_Idle *instance();
	void enter(troll *tro);
	void execute(troll *tro);
	void exit(troll *tro);
private:
	pose_Idle() {;}
	pose_Idle(const pose_Idle&);
	pose_Idle &operator=(const pose_Idle&);
};
//
pose_Idle *pose_Idle::instance()
{
	static pose_Idle instance;
	return &instance;
}
//
void pose_Idle::enter(troll *tro)
{
	tro;
}
//
void pose_Idle::execute(troll *tro)
{
	tro;
	cout << "idle" << endl;
}
//















//
}
#endif