////////////////
// audio_sfx_select.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AUDIO_SFX_SELECT_H
#define AUDIO_SFX_SELECT_H
#include "phy_magic.h"
namespace imm
{
////////////////
// sfx_select
////////////////
////////////////
template <typename T_app>
struct sfx_select
{
	sfx_select();
	void init(T_app *app_in);
	void play_effect(const SKILL_SPECIFY &skill, const size_t &ix1, const size_t &ix2, const XMFLOAT3 &center);
	T_app *app;
};
//
template <typename T_app>
sfx_select<T_app>::sfx_select():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void sfx_select<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void sfx_select<T_app>::play_effect(const SKILL_SPECIFY &skill, const size_t &ix1, const size_t &ix2, const XMFLOAT3 &center)
{
	skill;
	ix1;
	if (app->m_Inst.m_Troll[ix2].order_stat & ORDER_IS_GUARD) {
		app->m_Scene.plasma.push_back(PLASMA_STRIKE2, 0.5f, center);
		app->m_Scene.audio.play_effect(sfx::PunchLight);
	}
	else {
		app->m_Scene.plasma.push_back(PLASMA_STRIKE, 0.5f, center);
		app->m_Scene.audio.play_effect(sfx::Punch);
	}
}
//
}
#endif