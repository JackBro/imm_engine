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
#include <vector>
#include <set>
namespace imm
{
////////////////
// combo_para
////////////////
////////////////
struct combo_para
{
	combo_para();
	std::string model_name;
	int combo_ix;
	int current_ix;
	size_t inst_ix;
	bool is_busy;
	bool is_turn_next;
	float count_down;
};
//
combo_para::combo_para():
	combo_ix(-1),
	current_ix(-1),
	inst_ix(0),
	is_busy(false),
	is_turn_next(false),
	count_down(-1.0f)
{
	;
}
////////////////
// combo_data
////////////////
////////////////
struct combo_data
{
	combo_data();
	void build(const std::string &name);
private:
	void current_apply(combo_para &pa);
	void current_over(combo_para &pa);
public:
	void strike(combo_para &pa);
	void update(const float &dt, combo_para &pa);
	std::vector<std::string> atk;
	std::vector<float> frame_end;
	std::vector<float> frame_turn;
	std::vector<float> frame_speed;
	std::vector<std::vector<std::string>> atk_box;
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
	int combo_ix;
	float count_down;
	bool is_calculated;
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
	void init_combo_para(const size_t &index_in);
	void cause_damage(const size_t &inst_ix_atk, const size_t &inst_ix_dmg, const XMFLOAT3 &box_center);
	void execute(const size_t &index_in);
	void update(const float &dt);
	T_app *app;
	std::map<std::string, combo_data> combo_d;
	std::map<size_t, combo_para> c_para;
	std::map<int, damage_data> damage;
	std::map<size_t, std::set<size_t>> hits;
};
//
}
#endif