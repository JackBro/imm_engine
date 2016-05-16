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
// ai_sphere
////////////////
////////////////
struct ai_sphere
{
	ai_sphere();
	void init(const float &radius_inst_in, const float &radius_add_in);
	void transform(CXMMATRIX &world);
	BoundingSphere L;
	BoundingSphere W;
	bool is_active;
	float radius_inst;
	float radius_add;
};
//
ai_sphere::ai_sphere():
	is_active(false),
	radius_inst(0.0f),
	radius_add(1.0f)
{
	;
}
//
void ai_sphere::init(const float &radius_inst_in, const float &radius_add_in)
{
	radius_inst = radius_inst_in;
	radius_add = radius_add_in;
	L.Radius = radius_inst+radius_add;
	L.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	W = L;
}
//
void ai_sphere::transform(CXMMATRIX &world)
{
	L.Transform(W, world);
}
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
	std::map<size_t, ai_sphere> sphere;
	T_app *app;
};
//
template <typename T_app>
ai_probe<T_app>::ai_probe():
	app(nullptr)
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
			float radius_inst = (app->m_Inst.m_BoundL.extents_x(ix)+app->m_Inst.m_BoundL.extents_z(ix))/2.0f;
			sphere[ix].init(radius_inst, 1.0f);
		}
	}
}
//
//
template <typename T_app>
void ai_probe<T_app>::update()
{
	for (auto &sph: sphere) {
		if (!sph.second.is_active) continue;
		sph.second.transform(XMLoadFloat4x4(app->m_Inst.m_Stat[sph.first].get_World()));
		for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
			if (!app->m_Inst.m_Stat[ix].is_invoke_physics()) continue;
			bool is_touch = app->m_Inst.m_BoundW.intersects(ix, sph.second.W);
			app->m_Inst.m_Steering[sph.first].touch[ix] = is_touch;
		}
	}
}
//
}
#endif