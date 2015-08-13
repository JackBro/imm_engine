////////////////
// control_math.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_state.h"
#ifndef CONTROL_MATH_H
#define CONTROL_MATH_H
namespace imm {namespace math
{
void mouse_move_toward_hit(
	CXMVECTOR &hit_pos,
	const size_t &index,
	const float &speed);
//
void mouse_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	CXMVECTOR &direction);
//



//
}}
#endif