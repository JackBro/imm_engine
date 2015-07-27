////////////////
// control_auxiliary.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_AUXILIARY_H
#define CONTROL_AUXILIARY_H
#include "control_xinput.h"
namespace imm
{
////////////////
// control_stop
////////////////
////////////////
struct control_stop
{
	control_stop();
	void set_aabb(CXMVECTOR &pos, const float &half_y);
	bool contains(const XMFLOAT3 &center);
	bool is_stop;
	BoundingBox bbox;
	XMFLOAT3 hit_pos;
	float dt_cd;
	float speed;
};
//
control_stop::control_stop():
	is_stop(true),
	hit_pos(0.0f, 0.0f, 0.0f),
	dt_cd(0.0f),
	speed(0.0f)
{
	;
}
//
void control_stop::set_aabb(CXMVECTOR &pos, const float &half_y)
{
	XMStoreFloat3(&bbox.Center, pos);
	bbox.Extents = XMFLOAT3(0.2f, half_y*2.0f, 0.2f);
	hit_pos = bbox.Center;
}
//
bool control_stop::contains(const XMFLOAT3 &center)
{
	XMVECTOR C = XMLoadFloat3(&center);
	if (bbox.Contains(C) == 2) return true;
	else return false;
}
////////////////
// control_motion
////////////////
////////////////
struct control_motion
{
	control_motion();
	std::string walk_run;
	std::string idle;
	std::string jump;
	float speed;
	float speed_walk;
	float speed_run;
	float jump_velocity;
	void switch_walk_run();
	void make_run();
	void make_walk();
};
//
control_motion::control_motion():
	walk_run("Run"),
	idle("Idle"),
	jump("Jump"),
	speed(13.5f),
	speed_walk(4.5f),
	speed_run(13.5),
	jump_velocity(20.0f)
{
	;
}
//
void control_motion::switch_walk_run()
{
	if (walk_run == "Walk") {
		walk_run = "Run";
		speed = speed_run;
	}
	else {
		walk_run = "Walk";
		speed = speed_walk;
	}
}
//
void control_motion::make_run()
{
	if (walk_run != "Run") {
		walk_run = "Run";
		speed = speed_run;
	}
}
//
void control_motion::make_walk()
{
	if (walk_run != "Walk") {
		walk_run = "Walk";
		speed = speed_walk;
	}
}
//
}
#endif