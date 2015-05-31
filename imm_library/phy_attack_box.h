////////////////
// phy_attack_box.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_ATTACK_BOX_H
#define PHY_ATTACK_BOX_H
#include "phy_prepare.h"
#include <map>
namespace imm
{
////////////////
// 
////////////////
////////////////
struct phy_attack
{
	phy_attack();
	std::string name;
	std::map<std::string, int> map_bone;
	XMFLOAT4X4 to_bone;
	XMFLOAT3 extents;
};
//
phy_attack::phy_attack()
{
	;
}
////////////////
// 
////////////////
////////////////
template <typename T_app>
struct phy_attack_box
{
	phy_attack_box();
	~phy_attack_box();
	phy_bound_mgr bound_l;
	phy_bound_mgr bound_w;
	std::map<size_t, int> map;
	T_app *app;
};
//
template <typename T_app>
phy_attack_box<T_app>::phy_attack_box():
	bound_l(),
	bound_w(),
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_attack_box<T_app>::~phy_attack_box()
{
	;
}
//
}
#endif