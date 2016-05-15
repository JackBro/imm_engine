////////////////
// phy_ai_probe.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_AI_PROBE_H
#define PHY_AI_PROBE_H
#include "phy_prepare.h"
namespace imm
{
////////////////
// ai_probe
////////////////
////////////////
template <typename T_app>
struct ai_probe
{
	ai_probe();
	void init(T_app *app_in);
	void rebuild();
	void update();
	std::map<size_t, BoundingSphere> sphere_l;
	std::map<size_t, BoundingSphere> sphere_w;
	T_app *app;
	float radius_add;
};
//
template <typename T_app>
ai_probe<T_app>::ai_probe():
	app(nullptr),
	radius_add(1.0f)
{
	;
}
//
template <typename T_app>
void ai_probe<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ai_probe<T_app>::rebuild()
{
	for (auto &stat: app->m_Inst.m_Stat) {
		if (stat.is_controllable) {
			size_t ix = &stat - &app->m_Inst.m_Stat[0];
			float radius = (app->m_Inst.m_BoundL.extents_x(ix)+app->m_Inst.m_BoundL.extents_z(ix))/2.0f;
			radius += radius_add;
			XMFLOAT3 center = XMFLOAT3(0.0f, 0.0f, 0.0f);
			sphere_l[ix].Radius = radius;
			sphere_l[ix].Center = center;
			sphere_w[ix] = sphere_l[ix];
		}
	}
}
//
//
template <typename T_app>
void ai_probe<T_app>::update()
{
	for (auto &sph: sphere_w) {
		sph.second.Transform(sphere_l[sph.first], XMLoadFloat4x4(app->m_Inst.m_Stat[sph.first].get_World()));
	}
}
//
}
#endif