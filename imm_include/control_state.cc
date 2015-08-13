////////////////
// control_state.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_math.h"
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
void pose_Idle::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Idle);
}
//
void pose_Idle::execute(troll *tro)
{
	if (tro->order & ORDER_MOVE) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_touch_ground) return;	
	}
	
	
	
	
	
}
//
void pose_Idle::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Walk
////////////////
////////////////
struct troll;
struct pose_Walk: public state<troll>
{
	static pose_Walk *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
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
void pose_Walk::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(act::Walk);
}
//
void pose_Walk::execute(troll *tro)
{
	tro;
}
//
void pose_Walk::exit(troll *tro)
{
	tro;
}

























//
}