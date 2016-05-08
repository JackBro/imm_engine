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
// ai_mental
////////////////
////////////////
struct ai_mental
{
	size_t ix;
	std::string name;
	int ai_type;
};
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
	void update();
	T_app *app;
	std::vector<ai_mental> mental_data;
	std::vector<ai_mental> mental_scene;
};
//
template <typename T_app>
ai_npc<T_app>::ai_npc():
	app(nullptr),
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
void ai_npc<T_app>::update()
{
	app->m_Inst.m_NameMap;
	app->m_Inst.m_Steering;
	for (auto &steer: app->m_Inst.m_Steering) {
		steer.first;
		steer.second;
	}
}
//
}
#endif