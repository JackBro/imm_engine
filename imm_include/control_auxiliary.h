////////////////
// control_auxiliary.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_AUXILIARY_H
#define CONTROL_AUXILIARY_H
#include "control_xinput.h"
#include <DirectXCollision.h>
using namespace DirectX;
namespace imm
{
////////////////
// control_stop
////////////////
////////////////
template <typename T_app>
struct control_stop
{
	control_stop();
	void set_destination(const float &speed_in, CXMVECTOR &pos, const float &half_y);
	void set_aabb(CXMVECTOR &pos, const float &half_y);
	bool contains(const XMFLOAT3 &center);
	void update(T_app *app, const size_t &index, const float &dt);
	void interrupt(T_app *app, const size_t &index);
	bool is_stop;
	BoundingBox bbox;
	XMFLOAT3 hit_pos;
	float cooldown;
	float speed;
};
//
template <typename T_app>
control_stop<T_app>::control_stop():
	is_stop(true),
	hit_pos(0.0f, 0.0f, 0.0f),
	cooldown(0.0f),
	speed(0.0f)
{
	;
}
//
template <typename T_app>
void control_stop<T_app>::set_destination(const float &speed_in, CXMVECTOR &pos, const float &half_y)
{
	is_stop = false;
	speed = speed_in;
	set_aabb(pos, half_y);
}
//
template <typename T_app>
void control_stop<T_app>::set_aabb(CXMVECTOR &pos, const float &half_y)
{
	XMStoreFloat3(&bbox.Center, pos);
	bbox.Extents = XMFLOAT3(0.2f, half_y*2.0f, 0.2f);
	hit_pos = bbox.Center;
}
//
template <typename T_app>
bool control_stop<T_app>::contains(const XMFLOAT3 &center)
{
	XMVECTOR C = XMLoadFloat3(&center);
	if (bbox.Contains(C) == 2) return true;
	else return false;
}
//
template <typename T_app>
void control_stop<T_app>::update(T_app *app, const size_t &index, const float &dt)
{
	if (is_stop) return;
	// assume bound's center approximate instance's world
	bool is_inter = contains(app->m_Inst.m_BoundW.center(index));
	if (is_inter) {
		is_stop = true;
		app->m_Inst.m_Stat[index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
		app->m_Inst.m_Troll[index].order |= ORDER_IDLE;
	}
	// adjust the direction
	else {
		cooldown += dt;
		if (cooldown > 0.5f) cooldown = 0.0f;
		else return;
		if (!app->m_Inst.m_Stat[index].phy.is_on_land) return;
		XMVECTOR hit = XMLoadFloat3(&(hit_pos));
		math::mouse_move_toward_hit(hit, index, speed);
	}
}
//
template <typename T_app>
void control_stop<T_app>::interrupt(T_app *app, const size_t &index)
{
	if (is_stop) return;
	is_stop = true;
	app->m_Inst.m_Stat[index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
//
}
#endif