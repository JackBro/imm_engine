////////////////
// ingot_data.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef INGOT_DATA_H
#define INGOT_DATA_H
#include "ai_attribute.h"
namespace imm
{
////////////////
// ingot_data
////////////////
////////////////
struct ingot_data
{
	ingot_data();
	~ingot_data() {;}
	void build_name();
	std::wstring *get_name(const std::string &name_in);
	void build_attr();
	void assign_attr(ai_points &points, const std::string &inst_name);
	std::map<std::string, std::wstring> name;
	std::map<std::string, ai_points> attr;
};
//
ingot_data::ingot_data():
	name(),
	attr()
{
	build_name();
	build_attr();
}
//
void ingot_data::build_name()
{
	name["pepper"] = L"Pepper";
	name["sinon"] = L"Sinon";
	name["unknow"] = L"Unknow";
}
//
std::wstring *ingot_data::get_name(const std::string &name_in)
{
	if (name.count(name_in)) return &name[name_in];
	return &name["unknow"];
}
//
void ingot_data::build_attr()
{
	attr["pepper"].hp_max = 60;
	attr["pepper"].hp = 60;
}
void ingot_data::assign_attr(ai_points &points, const std::string &inst_name)
{
	if (attr.count(inst_name)) points = attr[inst_name];
}
//
}
#endif