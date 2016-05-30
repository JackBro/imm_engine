////////////////
// ai_npc_interface.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_NPC_INTERFACE_H
#define AI_NPC_INTERFACE_H
#include "ai_steering.h"
namespace imm
{
////////////////
////////////////
// AI_TYPE
////////////////
////////////////
enum AI_TYPE
{
	AI_NONE        = 0x0,
	AI_BEAT_PLAYER = 0x1,
};
////////////////
// ai_mental
////////////////
////////////////
struct ai_mental
{
	ai_mental();
	size_t ix;
	std::string name;
	int type;
};
ai_mental::ai_mental():
	ix(0),
	name(),
	type(AI_BEAT_PLAYER)
{
	;
}
////////////////
// ai_npc
////////////////
////////////////
template <typename T_app>
struct ai_npc
{
	ai_npc();
	void init_load(T_app *app_in);
	void reset();
	void rebuild();
	void update(const float &dt);
	void update_beat_player(ai_mental &mind);
	void set_Seek();
	T_app *app;
	float delta_time;
	std::vector<ai_mental> mental_data;
	std::vector<ai_mental> mental_scene;
};
//
template <typename T_app>
ai_npc<T_app>::ai_npc():
	app(nullptr),
	delta_time(0.0f),
	mental_data(),
	mental_scene()
{
	;
}
//
template <typename T_app>
void ai_npc<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	mental_data.emplace_back();
	mental_data.back().name = "black_warrior";
	mental_data.back().type = AI_BEAT_PLAYER;
}
//
template <typename T_app>
void ai_npc<T_app>::reset()
{
	mental_scene.clear();
	mental_scene.shrink_to_fit();
}
//
template <typename T_app>
void ai_npc<T_app>::rebuild()
{
	reset();
	for (auto &data: mental_data) {
		if (!app->m_Inst.m_NameMap.count(data.name)) continue;
		mental_scene.push_back(data);
		mental_scene.back().ix = app->m_Inst.m_NameMap[data.name];
	}
}
//
template <typename T_app>
void ai_npc<T_app>::update(const float &dt)
{
	delta_time += dt+(math::calc_randf(-AI_DELTA_TIME_LOGIC, AI_DELTA_TIME_LOGIC))*0.1f;
	if (delta_time < AI_DELTA_TIME_LOGIC) return;
	else delta_time -= AI_DELTA_TIME_LOGIC;
	//
	for (auto &mind: mental_scene) {
		if (mind.type & AI_BEAT_PLAYER) update_beat_player(mind);
	}
}
//
template <typename T_app>
void ai_npc<T_app>::update_beat_player(ai_mental &mind)
{
	auto ste = &app->m_Inst.m_Steering[mind.ix];
	if (ste->current_state != ai_Seek::instance()) {
		if (PTR->m_Inst.m_Steering[ste->index].touch[ste->target]) return;
		ste->target = PTR->m_Control.player1;
		ste->count_down = -1.0f;
		ste->tactics |= AI_TAC_CLOSETO;
		app->m_Inst.m_Probe.set_active(mind.ix);
	}
	if (ste->report & AI_REP_CLOSETO) {
		ste->tactics |= AI_TAC_ATK;
	}
}
//
}
#endif