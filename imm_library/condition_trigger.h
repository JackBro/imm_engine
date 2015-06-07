////////////////
// condition_trigger.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONDITION_TRIGGER_H
#define CONDITION_TRIGGER_H
namespace imm
{
////////////////
// condition_trigger
////////////////
////////////////
template <typename T_app>
struct condition_trigger
{
	condition_trigger();
	void init(T_app *app_in);
	void update();
	T_app *app;
};
//
template <typename T_app>
condition_trigger<T_app>::condition_trigger():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void condition_trigger<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void condition_trigger<T_app>::update()
{
	;
}

//
}
#endif