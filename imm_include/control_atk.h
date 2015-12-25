////////////////
// control_atk.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_ATK_H
#define CONTROL_ATK_H
#include "control_state.h"
#include "phy_magic.h"
#include <vector>
#include <set>
namespace imm
{
////////////////
// SKILL_TYPE
////////////////
////////////////
enum SKILL_TYPE
{
	SKILL_TYPE_MELEE,
	SKILL_TYPE_MAGIC,
};
////////////////
// skill_para
////////////////
////////////////
struct skill_para
{
	skill_para();
	std::string model_name;
	int skill_ix;
	int current_ix;
	size_t inst_ix;
	bool is_busy;
	bool is_turn_next;
	float count_down;
	char symbol;
};
//
skill_para::skill_para():
	skill_ix(-1),
	current_ix(-1),
	inst_ix(0),
	is_busy(false),
	is_turn_next(false),
	count_down(-1.0f),
	symbol('A')
{
	;
}
////////////////
// skill_data
////////////////
////////////////
struct skill_data
{
	skill_data();
	void build(const std::string &name);
private:
	void current_apply(skill_para &pa);
	void current_over(skill_para &pa);
public:
	void strike(skill_para &pa);
	void update(const float &dt, skill_para &pa);
	std::vector<std::string> atk;
	std::vector<float> frame_end;
	std::vector<float> frame_turn;
	std::vector<float> frame_speed;
	std::vector<int> next_ix;
	std::vector<SKILL_TYPE> type;
	std::vector<SKILL_SPECIFY> specify;
	std::vector<std::vector<std::string>> atk_box;
	std::map<char, int> chunk;
};
////////////////
// damage_data
////////////////
////////////////
struct damage_data
{
	damage_data();
	void update(const float &dt);
	void stamp();
	size_t ix_atk;
	size_t ix_dmg;
	int skill_ix;
	float count_down;
	float delay;
	bool is_calculated;
	bool is_delay;
	const XMFLOAT3 *box_center;
};
////////////////
// control_atk
////////////////
////////////////
template <typename T_app>
struct control_atk
{
	control_atk();
	void init(T_app *app_in);
	void reset();
	void init_skill_para(const size_t &index_in);
	void cause_damage(const size_t &inst_ix_atk, const size_t &inst_ix_dmg, const XMFLOAT3 &box_center);
	void execute(const size_t &index_in, const char &symbol);
	void update(const float &dt);
	T_app *app;
	std::map<std::string, skill_data> data_ski;
	std::map<size_t, skill_para> para_ski;
	std::map<int, damage_data> damage;
	std::map<size_t, std::set<size_t>> hits;
};
//
}
#endif