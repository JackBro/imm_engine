////////////////
// condition_trigger.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONDITION_TRIGGER_H
#define CONDITION_TRIGGER_H
#include <string>
#include <map>
namespace imm
{
////////////////
// condition_task
////////////////
////////////////
struct condition_task
{
	enum type {
		once_time,
	};
	condition_task();
	bool is_ran;
	float certain_time;
};
//
condition_task::condition_task():
	is_ran(false),
	certain_time(0.0f)
{
	;
}
////////////////
// condition_trigger
////////////////
////////////////
template <typename T_app>
struct condition_trigger
{
	condition_trigger();
	void init(T_app *app_in);
	void reset();
	bool trigger(const std::string &task_name);
	void update(const float &dt);
	void update_scene01();
	float scene_pass_time;
	float delta_time;
	std::map<std::string, condition_task> task;
	T_app *app;
};
//
template <typename T_app>
condition_trigger<T_app>::condition_trigger():
	scene_pass_time(0.0f),
	delta_time(0.0f),
	app(nullptr)
{
	;
}
//
template <typename T_app>
void condition_trigger<T_app>::reset()
{
	for (auto &tas: task) {
		tas.second.is_ran = false;
	}
}
//
template <typename T_app>
bool condition_trigger<T_app>::trigger(const std::string &task_name)
{
	auto tas = &task[task_name];
	if (tas->is_ran) return false;
	if (scene_pass_time > tas->certain_time) {
		tas->is_ran = true;
		return true;
	}
	else return false;
}
//
template <typename T_app>
void condition_trigger<T_app>::init(T_app *app_in)
{
	app = app_in;
	task.emplace("clear_cmd", condition_task());
	task["clear_cmd"].certain_time = 3.0f;
	task.emplace("01_test", condition_task());
	task["01_test"].certain_time = 5.0f;
}
//
template <typename T_app>
void condition_trigger<T_app>::update(const float &dt)
{
	delta_time += dt+(math::calc_randf(-AI_DELTA_TIME_LOGIC, AI_DELTA_TIME_LOGIC))*0.1f;
	if (delta_time < AI_DELTA_TIME_LOGIC) return;
	else delta_time -= AI_DELTA_TIME_LOGIC;
	scene_pass_time = app->m_Timer.total_time() - app->m_Scene.begin_time;
	if (trigger("clear_cmd")) {
		if (!app->m_Cmd.is_active) app->m_Cmd.input.clear();
	}
	//
	update_scene01();
}
//
template <typename T_app>
void condition_trigger<T_app>::update_scene01()
{
	if (app->m_Scene.scene_ix != "01") return;
	if (trigger("01_test")) {
		app->m_UiMgr.group_active("dialogue", "test", true);
	}
}
}
#endif