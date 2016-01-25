////////////////
// ai_attribute.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_ATTRIBUTE_H
#define AI_ATTRIBUTE_H
namespace imm
{
////////////////
// ai_points
////////////////
////////////////
struct ai_points
{
	ai_points();
	int hp;
	int mp;
	int str;
	int mgc;
	int def;
	int res;
};
//
ai_points::ai_points():
	hp(20),
	mp(20),
	str(5),
	mgc(5),
	def(5),
	res(5)
{
	;
}
////////////////
// ai_attr
////////////////
////////////////
template <typename T_app>
struct ai_attr
{
	ai_attr();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void rebuild_troll();
	T_app *app;
};
//
template <typename T_app>
ai_attr<T_app>::ai_attr():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void ai_attr<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ai_attr<T_app>::reset()
{
	;
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild()
{
	reset();
	rebuild_troll();
}
template <typename T_app>
void ai_attr<T_app>::rebuild_troll()
{
	;
}
//
}
#endif