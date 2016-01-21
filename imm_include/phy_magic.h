////////////////
// phy_magic.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_MAGIC_H
#define PHY_MAGIC_H
#include "phy_prepare.h"
#include <list>
namespace imm
{
////////////////
// SKILL_SPECIFY
////////////////
////////////////
enum SKILL_SPECIFY
{
	SKILL_MELEE_UNARMED   = 0,
	SKILL_MAGIC_PREPARE01 = 1,
	SKILL_MAGIC_LIGHTNING = 2,
};
////////////////
// magic_inst
////////////////
////////////////
struct magic_inst
{
	magic_inst();
	void update(const float &dt);
	SKILL_SPECIFY specify;
	BoundingSphere shpere;
	float count_down;
	float duration;
};
////////////////
// phy_magic
////////////////
////////////////
template <typename T_app>
struct phy_magic
{
	phy_magic();
	~phy_magic();
	void init(T_app *app_in);
	void update();
	void invoke(const SKILL_SPECIFY &specify);
	void push_MAGIC_LIGHTNING();
	std::list<magic_inst> inst;
	T_app *app;
};
//
//
template <typename T_app>
phy_magic<T_app>::phy_magic():
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_magic<T_app>::~phy_magic()
{
	;
}
//
template <typename T_app>
void phy_magic<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void phy_magic<T_app>::update()
{
	for (auto &ins: inst) ins.update(app->m_Timer.delta_time());
	auto is_should_del =
		[](const magic_inst &mag) {return (mag.count_down < 0.0f);};
	inst.remove_if(is_should_del);
}
//
template <typename T_app>
void phy_magic<T_app>::invoke(const SKILL_SPECIFY &specify)
{
	switch (specify) {
	case SKILL_MAGIC_PREPARE01:
		app->m_Scene.audio.play_effect("electricity_spark");
		break;
	case SKILL_MAGIC_LIGHTNING:
		push_MAGIC_LIGHTNING();
		break;
	}
}
//
template <typename T_app>
void phy_magic<T_app>::push_MAGIC_LIGHTNING()
{
	inst.emplace_back();
	inst.back().count_down = 0.5f;
	inst.back().specify = SKILL_MAGIC_LIGHTNING;
}
//
}
#endif