////////////////
// phy_magic.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "phy_magic.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// magic_inst
////////////////
////////////////
magic_inst::magic_inst():
	specify(SKILL_MAGIC_LIGHTNING),
	count_down(1.0f),
	duration(1.0f),
	caster(0),
	target(0)
{
	;
}
//
void magic_inst::update(const float &dt)
{
	count_down -= dt;
	duration -=dt;
	if (count_down < 0.0f) {
		switch (specify) {
		case SKILL_MAGIC_LIGHTNING:
			apply_MAGIC_LIGHTNING();
			break;
		}
	}
}
//
void magic_inst::apply_MAGIC_LIGHTNING()
{
	size_t ix_dmg = PTR->m_AiInfo.random_an_enemy(caster);
	XMFLOAT3 center = PTR->m_Inst.m_BoundW.center(ix_dmg);
	PTR->m_Scene.plasma.push_back(PLASMA_LIGHTNING, 3.0f, center);
	PTR->m_Scene.audio.play_effect("electricity_voltage");	
}
//
}