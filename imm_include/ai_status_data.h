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
#include "control_state.h"
#include "stru_lua_help.h"
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
	void init();
	void build_ability();
	void build_name();
	std::wstring *get_name(const std::string &name_in);
	void build_attr();
	void assign_attr(ai_points &points, const std::string &inst_name);
	std::map<std::string, std::wstring> name;
	std::map<std::string, ai_points> attr;
	std::map<std::string, action_data> ability;
};
//
status_data::status_data():
	name(),
	attr(),
	ability()
{
	;	
}
//
void status_data::init()
{
	build_ability();
	build_name();
	build_attr();
}
//
void status_data::build_ability()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_ability", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		std::string model = vec2d[ix][0];
		ability[model];
		ability[model].velocity_Jump = std::stof(vec2d[ix][1]);
		ability[model].speed_Walk = std::stof(vec2d[ix][2]);
		ability[model].speed_Run = std::stof(vec2d[ix][3]);
		ability[model].speed_Roll = std::stof(vec2d[ix][4]);
		ability[model].frame_RollStep = std::stof(vec2d[ix][5])*FRAME_RATE_1DIV;
		ability[model].frame_RollToIdle = std::stof(vec2d[ix][6])*FRAME_RATE_1DIV;
	}
}
//
void status_data::build_name()
{
	name["pepper"] = L"Pepper";
	name["nino"] = L"Nino";
	name["unknow"] = L"Unknow";
	name["black_warrior"] = L"Soul of Warrior Armor";
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
	//attr["black_warrior"].hp_max = 60.0f;
	//attr["black_warrior"].hp = 60.0f;
}
void status_data::assign_attr(ai_points &points, const std::string &inst_name)
{
	if (attr.count(inst_name)) points = attr[inst_name];
}
//
}
#endif