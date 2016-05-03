////////////////
// ai_status_data.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AI_STATUS_DATA_H
#define AI_STATUS_DATA_H
#include "ai_attribute.h"
namespace imm
{
////////////////
// status_data
////////////////
////////////////
struct status_data
{
	status_data();
	~status_data() {;}
	void build_name();
	std::wstring *get_name(const std::string &name_in);
	void build_attr();
	void assign_attr(ai_points &points, const std::string &inst_name);
	std::map<std::string, std::wstring> name;
	std::map<std::string, ai_points> attr;
};
//
status_data::status_data():
	name(),
	attr()
{
	build_name();
	build_attr();
}
//
void status_data::build_name()
{
	name["pepper"] = L"Pepper";
	name["sinon"] = L"Sinon";
	name["unknow"] = L"Unknow";
}
//
std::wstring *status_data::get_name(const std::string &name_in)
{
	if (name.count(name_in)) return &name[name_in];
	return &name["unknow"];
}
//
void status_data::build_attr()
{
	attr["pepper"].hp_max = 60.0f;
	attr["pepper"].hp = 60.0f;
}
void status_data::assign_attr(ai_points &points, const std::string &inst_name)
{
	if (attr.count(inst_name)) points = attr[inst_name];
}
//
}
#endif