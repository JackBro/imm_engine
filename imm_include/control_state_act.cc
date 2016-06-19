////////////////
// control_state_act.cc
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
act_str::act_str()
{
	assert(false);
}
//
act_str::act_str(int &order_stat)
{
	p_order_s = &order_stat;
}
//
std::string act_str::Idle()
{
	if (*p_order_s & ORDER_IS_GUARD) return "IdleGuard";
	if (*p_order_s & ORDER_IS_ENGAGE) return "Engage";
	return "Idle";
}
//
std::string act_str::Run()
{
	if (*p_order_s & ORDER_IS_WALK) {
		return "Walk";
	}
	return "Run";
}
//
std::string act_str::Jump()
{
	return "Jump";
}
//
std::string act_str::JumpLand()
{
	return "JumpLand";
}
//
std::string act_str::Engage()
{
	return "Engage";
}
//
std::string act_str::Damage()
{
	return "Damage";
}
//
std::string act_str::Roll()
{
	return "Roll";
}
//
std::string act_str::WalkRev()
{
	return "WalkRev";
}
//
std::string act_str::Guard()
{
	return "Guard";
}
//
