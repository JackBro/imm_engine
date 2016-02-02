////////////////
// ai_steering.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_STEERING_H
#define AI_STEERING_H
#include "ai_decision.h"
#include "control_state.h"
namespace imm
{
////////////////
// ai_Standby
////////////////
////////////////
struct steering;
struct ai_Standby: public state<steering>
{
	static ai_Standby *instance();
	void enter(steering*);
	void execute(steering*);
	void exit(steering*);
private:
	ai_Standby() {;}
	ai_Standby(const ai_Standby&);
	ai_Standby &operator=(const ai_Standby&);
};
////////////////
// steering
////////////////
////////////////
struct steering
{
	steering();
	void init(const size_t &index_in);
	void update();
	void change_state(state<steering> *new_state);
	void revert_previous_state();
	float speed_move();
	std::string &act_move();
	state<steering> *current_state;
	state<steering> *previous_state;
	size_t index;
	bool is_active;
	XMFLOAT3 desired_pos;
	std::map<size_t, bool> touch;
	std::vector<size_t> attack;
	std::vector<size_t> damage;
};
//
steering::steering():
	current_state(ai_Standby::instance()),
	previous_state(ai_Standby::instance()),
	index(0),
	is_active(false),
	desired_pos(0.0f, 0.0f, 0.0f),
	touch(),
	attack(),
	damage()
{
	;
}
//
void steering::init(const size_t &index_in)
{
	index = index_in;
	is_active = true;
}
//
void steering::change_state(state<steering> *new_state)
{
	assert(current_state && new_state);
	previous_state = current_state;
	current_state->exit(this);
	current_state = new_state;
	current_state->enter(this);
}
//
void steering::revert_previous_state()
{
	change_state(previous_state);
}
//
}
#endif