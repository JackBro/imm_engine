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
	size_t ix_tar = PTR->m_AiInfo.random_an_enemy(caster);
	sphere.Center = PTR->m_Inst.m_BoundW.center(ix_tar);
	PTR->m_Scene.plasma.push_back(PLASMA_LIGHTNING, 3.0f, sphere.Center);
	PTR->m_Scene.audio.play_effect("electricity_voltage");
	for (auto &ix_inst: PTR->m_AiInfo.vec_all) {
		bool is_touch = PTR->m_Inst.m_BoundW.intersects(ix_inst, sphere);
		if (is_touch) {
			PTR->m_Control.atk.cause_damage(
				caster,
				ix_inst,
				math::VECTOR3_NULL,
				SKILL_MAGIC_LIGHTNING);
			//
		}
	}
}
//
}