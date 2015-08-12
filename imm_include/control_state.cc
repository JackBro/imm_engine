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
	PTR->m_Inst.m_Stat[index].check_set_ClipName(act::Idle);
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
////////////////
// pose_Walk
////////////////
////////////////
struct troll;
struct pose_Walk: public state<troll>
{
	static pose_Walk *instance();
	void enter(troll*, size_t);
	void execute(troll*, size_t);
	void exit(troll*, size_t);
private:
	pose_Walk() {;}
	pose_Walk(const pose_Idle&);
	pose_Walk &operator=(const pose_Idle&);
};
//
pose_Walk *pose_Walk::instance()
{
	static pose_Walk instance;
	return &instance;
}
//
void pose_Walk::enter(troll *tro, size_t index)
{
	tro;
	PTR->m_Inst.m_Stat[index].check_set_ClipName(act::Walk);
}
//
void pose_Walk::execute(troll *tro, size_t index)
{
	tro;
	index;
}
//
void pose_Walk::exit(troll *tro, size_t index)
{
	tro;
	index;
}

























//
}