////////////////
// control_math.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef CONTROL_MATH_H
#define CONTROL_MATH_H
#include <DirectXMath.h>
using namespace DirectX;
#include "control_state.h"
#include <string>
namespace imm {namespace math
{
////////////////
// static const
////////////////
////////////////
static const XMVECTORF32 VECTOR_AXIS_Y = {0.0f, 1.0f, 0.0f, 0.0f};
static const XMVECTORF32 VECTOR_FRONT_FACING = {0.0f, 0.0f, -1.0f, 0.0f};
static const XMFLOAT3 VECTOR3_NULL = {0.0f, 10.0f, 0.0f};
////////////////
// functions
////////////////
////////////////
void mouse_inst_move(const size_t &index, const float &speed);
void mouse_move_toward_hit(
	CXMVECTOR &hit_pos,
	const size_t &index,
	const float &speed);
void mouse_move_toward_hit_ai_avoid(
	const bool &is_avoid_force,
	const size_t &index,
	const float &speed);
void mouse_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	CXMVECTOR &direction);
void pad_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	XMVECTOR &direction,
	const float &rot_cam,
	const bool &is_keyboard);
void mouse_hit_plane_y(XMVECTOR &hit_pos_out);
void mouse_hit_terrain(XMVECTOR &hit_pos_out);
void pad_move_toward(const size_t &index, const float &speed);
bool key_move_wasd(const size_t &index, const float &speed);
void set_inst_speed(const size_t &index, const float &speed);
void set_inst_face_to_inst2(const size_t &index, const size_t &index2);
void set_face_to_face(const size_t &index, const size_t &index2);
void ai_move_pos(const size_t &index, const float &speed);
void ai_move_pos_stop(const size_t &index);
//
}}
#endif