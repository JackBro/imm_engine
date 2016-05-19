////////////////
// ai_information.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_INFORMATION_H
#define AI_INFORMATION_H
#include <set>
#include <vector>
namespace imm
{
////////////////
// AGENT_TYPE
////////////////
////////////////
enum AGENT_TYPE
{
	AGENT_TYPE_STATIC,
	AGENT_TYPE_AI,
};
////////////////
// ai_info
////////////////
////////////////
template <typename T_app>
struct ai_info
{
	ai_info();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	size_t random_an_enemy(const size_t &caller);
	T_app *app;
	std::set<size_t> set_friend;
	std::set<size_t> set_enemy;
	std::set<size_t> set_all;
	std::vector<size_t> vec_friend;
	std::vector<size_t> vec_enemy;
	std::vector<size_t> vec_all;
};
//
template <typename T_app>
ai_info<T_app>::ai_info():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void ai_info<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ai_info<T_app>::reset()
{
	set_friend.clear();
	set_enemy.clear();
	set_all.clear();
	vec_friend.clear();
	vec_enemy.clear();
	vec_all.clear();
	vec_friend.shrink_to_fit();
	vec_enemy.shrink_to_fit();
	vec_all.shrink_to_fit();
}
//
template <typename T_app>
void ai_info<T_app>::rebuild()
{
	reset();
	size_t ix = 0;
	for (auto &stat: app->m_Inst.m_Stat) {
		if (stat.property & MODEL_IS_CONTROLLABLE) {
			set_enemy.insert(ix);
			vec_enemy.push_back(ix);
			set_all.insert(ix);
			vec_all.push_back(ix);
		}
		++ix;
	}
}
//
template <typename T_app>
size_t ai_info<T_app>::random_an_enemy(const size_t &caller)
{
	caller;
	size_t range = vec_enemy.size();
	int offset = rand() % range;
	return vec_enemy[offset];
}
//
}
#endif