////////////////
// control_state.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_state.h"
namespace imm
{
////////////////
// pose_Idle
////////////////
////////////////
pose_Idle *pose_Idle::instance()
{
	static pose_Idle instance;
	return &instance;
}
//
void pose_Idle::enter(troll *tro, size_t index)
{
	tro;
	index;
}
//
void pose_Idle::execute(troll *tro, size_t index)
{
	tro;
	index;
}
//
void pose_Idle::exit(troll *tro, size_t index)
{
	tro;
	index;
}
//
}