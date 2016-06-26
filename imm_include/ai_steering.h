////////////////
// ai_steering.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_STEERING_H
#define AI_STEERING_H
#include "control_state.h"
namespace imm
{
////////////////
// AI_TACTICS
////////////////
////////////////
enum AI_TACTICS
{
	AI_TAC_NONE   = 0x0,
	AI_TAC_STANDY = 0x1,
	AI_TAC_PATROL = 0x2,
	AI_TAC_SEEK   = 0x4,
	AI_TAC_ATK    = 0x8,
};
////////////////
// AI_REPORT
////////////////
////////////////
enum AI_REPORT
{
	AI_REP_NONE      = 0x0,
	AI_REP_TAR_CLOSE = 0x1,
	AI_REP_TAR_AWAY  = 0X2,
};
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
// ai_Patrol
////////////////
////////////////
struct ai_Patrol: public state<steering>
{
	static ai_Patrol *instance();
	void enter(steering*);
	void execute(steering*);
	void exit(steering*);
private:
	ai_Patrol() {;}
	ai_Patrol(const ai_Patrol&);
	ai_Patrol &operator=(const ai_Patrol&);
};
////////////////
// ai_Seek
////////////////
////////////////
struct ai_Seek: public state<steering>
{
	static ai_Seek *instance();
	void enter(steering*);
	void execute(steering*);
	void exit(steering*);
private:
	ai_Seek() {;}
	ai_Seek(const ai_Seek&);
	ai_Seek &operator=(const ai_Seek&);
};
////////////////
// ai_Atk
////////////////
////////////////
struct ai_Atk: public state<steering>
{
	static ai_Atk *instance();
	void enter(steering*);
	void execute(steering*);
	void exit(steering*);
private:
	ai_Atk() {;}
	ai_Atk(const ai_Atk&);
	ai_Atk &operator=(const ai_Atk&);
};
////////////////
// steering
////////////////
////////////////
struct steering
{
	steering();
	void init(const size_t &index_in);
	void update(const float &dt);
	void change_state(state<steering> *new_state);
	void revert_previous_state();
	float speed_move();
	std::string &act_move();
	state<steering> *current_state;
	state<steering> *previous_state;
	size_t index;
	size_t target;
	bool is_active;
	XMFLOAT3 desired_pos;
	float count_down;
	float update_dt;
	float action_dt;
	int tactics;
	int report;
	std::map<size_t, bool> close;
	std::map<size_t, bool> alert;
	std::vector<size_t> attack;
	std::vector<size_t> damage;
};
//
steering::steering():
	current_state(ai_Standby::instance()),
	previous_state(ai_Standby::instance()),
	index(0),
	target(0),
	is_active(false),
	desired_pos(0.0f, 0.0f, 0.0f),
	count_down(-1.0f),
	update_dt(math::calc_randf(-AI_DELTA_TIME_PHY_SLOW, AI_DELTA_TIME_PHY_SLOW)),
	action_dt(0.0f),
	tactics(AI_TAC_NONE),
	report(AI_REP_NONE),
	close(),
	alert(),
	attack(),
	damage()
{
	;
}
//
void steering::init(const size_t &index_in)
{
	index = index_in;
	is_active = false;
	update_dt = math::calc_randf(-1.0f, 1.0f);
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