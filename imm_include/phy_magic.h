////////////////
// phy_magic.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
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
	SKILL_MELEE_UNARMED,
	SKILL_MAGIC_PREPARE1,
	SKILL_MAGIC_LIGHTNING,
};
////////////////
// magic_inst
////////////////
////////////////
struct magic_inst
{
	magic_inst();
	void update(const float &dt);
	BoundingSphere shpere;
	float count_down;
	float duration;
};
//
magic_inst::magic_inst():
	count_down(0.0f),
	duration(1.0f)
{
	;
}
//
void magic_inst::update(const float &dt)
{
	count_down -= dt;
	duration -=dt;
}
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
}
//
template <typename T_app>
void phy_magic<T_app>::invoke(const SKILL_SPECIFY &specify)
{
	switch (specify) {
	case SKILL_MAGIC_PREPARE1:
		break;
	case SKILL_MAGIC_LIGHTNING:
		break;
	}
}
//
//
}
#endif