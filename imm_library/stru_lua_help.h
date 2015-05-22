////////////////
// stru_lua_help.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_LUA_HELP_H
#define STRU_LUA_HELP_H
#include <iostream>
#include "imm_core.h"
#include "lua.hpp"
#include <mutex>
namespace imm
{
////////////////
// lua_reader
////////////////
////////////////	
class lua_reader
{
public:
	lua_reader():
		mutex1(),
		L(nullptr),
		file_name("")
		{;}
	~lua_reader() {if (L) lua_close(L);}
	std::recursive_mutex mutex1;
	lua_State *L;
	std::string file_name;
	void loadfile(const std::string &fname);
	void clear_stack();
	void map_from_global(std::map<std::string, std::string> &map_str);
	void vec2d_str_from_global(
		const std::string &table_name,
		std::vector<std::vector<std::string>> &vec2d_str);
	void vec2d_str_from_global_wstr(
		const std::string &table_name,
		std::vector<std::vector<std::wstring>> &vec2d_wstr);
	template <typename T_bool>
	void assign_bool(T_bool &b_out, const std::string &str_in);
private:
	lua_reader(const lua_reader &rhs);
	lua_reader &operator=(const lua_reader &rhs);
};
//
void lua_reader::loadfile(const std::string &fname)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (L) lua_close(L);
	file_name = fname;
	L = luaL_newstate();
	luaL_openlibs(L);
	if (luaL_loadfile(L, file_name.c_str()) || lua_pcall(L, 0, 0, 0)) {
		std::wostringstream os;
		os << "Lua error: " << lua_tostring(L, -1);
		ERROR_MES(os.str().c_str());
		return;
	}
}
//
void lua_reader::clear_stack()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	int n = lua_gettop(L);
	lua_pop(L, n);
}
//
void lua_reader::map_from_global(std::map<std::string, std::string> &map_str)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (!L) {assert(false); return;}
	for (auto map_it = map_str.begin(); map_it != map_str.end(); ++map_it) {
		lua_getglobal(L, map_it->first.c_str());
		if (lua_isnil(L,-1)) {assert(false); return;}
		if (lua_isstring(L, -1)) map_it->second = (lua_tostring(L, -1));
		else {
			std::wostringstream os;
			os << "Load " << file_name.c_str() << " error: " << map_it->first.c_str();
			ERROR_MES(os.str().c_str());
			return;
		}
	}
}
//
void lua_reader::vec2d_str_from_global(
	const std::string &table_name,
	std::vector<std::vector<std::string>> &vec2d_str)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (!L) {assert(false); return;}
	vec2d_str = std::vector<std::vector<std::string>>();
	int ix_1 = 1;
	lua_getglobal(L, table_name.c_str());
	if (lua_isnil(L,-1)) {
		std::wostringstream os;
		os << "Lua table name error: " << table_name.c_str();
		ERROR_MES(os.str().c_str());
	}
	lua_pushstring(L, "dummy");
	while (!lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_pushinteger(L, ix_1++);
		lua_gettable(L, -2);
		int ix_2 = 1;
		vec2d_str.push_back(std::vector<std::string>());
		if (lua_istable(L, -1)) {
			lua_pushstring(L, "dummy");
			while (!lua_isnil(L, -1)) {
				lua_pop(L, 1);
				lua_pushinteger(L, ix_2++);
				lua_gettable(L, -2);
				if (lua_isstring(L, -1)) {
					vec2d_str[ix_1-2].push_back(lua_tostring(L, -1));
				}
			}
			lua_pop(L, 1);
			// if blank line, delete it
			if (vec2d_str.back().size() == 0) vec2d_str.pop_back();
		}
	}
	vec2d_str.pop_back();
}
//
void lua_reader::vec2d_str_from_global_wstr(
	const std::string &table_name,
	std::vector<std::vector<std::wstring>> &vec2d_wstr)
{
	vec2d_wstr = std::vector<std::vector<std::wstring>>();
	std::vector<std::vector<std::string>> vec2d_str;
	vec2d_str_from_global(table_name, vec2d_str);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	for (size_t ix = 0; ix < vec2d_str.size(); ++ix) {
		vec2d_wstr.push_back(std::vector<std::wstring>());
		for (size_t ix_2 = 0; ix_2 < vec2d_str[ix].size(); ++ix_2)
			vec2d_wstr[ix].push_back(convert.from_bytes(vec2d_str[ix][ix_2]));
	}	
}
//
template <typename T_bool>
void lua_reader::assign_bool(T_bool &b_out, const std::string &str_in)
{
	if (str_in == "true" || str_in == "1")
		b_out = true;
	else b_out = false;
}
////////////////
// rotation_xyz
////////////////
////////////////
struct rotation_xyz
{
	rotation_xyz():
		x(0.0f),
		y(0.0f),
		z(0.0f)
		{;}
	rotation_xyz(const std::string &xyz);
	float x;
	float y;
	float z;
	XMMATRIX get_Matrix();
};
//
rotation_xyz::rotation_xyz(const std::string &xyz)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	std::vector<std::string::size_type> pos;
	pos.push_back(xyz.find_first_of("Xx"));
	pos.push_back(xyz.find_first_of("Yy"));
	pos.push_back(xyz.find_first_of("Zz"));
	//"0123456789" will check converting, stof() first part of string must be a number
	std::string s_number("0123456789");
	for (size_t ix = 0; ix != pos.size(); ++ix) {
		if (pos[ix] != std::string::npos && pos[ix] < xyz.size()-1) {
			std::string::size_type offset = 0;
			if (xyz.substr(pos[ix]+1).find_first_of("+-") == 0) offset = 1;
			if (xyz.substr(pos[ix]+1+offset).find_first_of(s_number) == 0) {
				switch (ix) {
					case 0: x = std::stof(xyz.substr(pos[ix]+1))/180.0f*XM_PI; break;
					case 1: y = std::stof(xyz.substr(pos[ix]+1))/180.0f*XM_PI; break;
					case 2: z = std::stof(xyz.substr(pos[ix]+1))/180.0f*XM_PI; break;
				}
			}
		}
	}
}
//
XMMATRIX rotation_xyz::get_Matrix()
{
	XMMATRIX R = XMMatrixIdentity();
	if (abs(x) > 0.01f) R = XMMatrixMultiply(R, XMMatrixRotationX(x));
	if (abs(y) > 0.01f) R = XMMatrixMultiply(R, XMMatrixRotationY(y));
	if (abs(z) > 0.01f) R = XMMatrixMultiply(R, XMMatrixRotationZ(z));
	return R;
}
////////////////
// csv_string_to_float
////////////////
////////////////
std::vector<float> csv_string_to_float(const std::string &str, const size_t &count)
{
	std::vector<float> rt;
	std::string::size_type pos = 0, pos_get;
	for (size_t ix = 0; ix != count; ++ix) {
		rt.push_back(std::stof(str.substr(pos), &pos_get));
		pos += pos_get;
	}
	return rt;
}
////////////////
// csv_value_is_empty
////////////////
////////////////
bool csv_value_is_empty(const std::string &str)
{
	if (str == "none") return true;
	if (str == "0") return true;
	if (str == "empty") return true;
	if (str == "") return true;
	return false;
}
////////////////
// lua_config
////////////////
////////////////
template <typename T_app>
struct lua_config
{
	lua_config();
	lua_config(T_app *app_in);
	void additional1();
	T_app *m_App;
};
//
template <typename T_app>
lua_config<T_app>::lua_config():
	m_App(nullptr)
{
	;
}
//
template <typename T_app>
lua_config<T_app>::lua_config(T_app *app_in)
{
	m_App = app_in;
	std::map<std::string, std::string> g_map;
	g_map["is_fullscreen"] = "";
	g_map["is_4xmsaa"] = "";
	g_map["is_sync_iterval"] = "";
	lua_reader l_reader;
	l_reader.loadfile(GLOBAL["path_lua"]+"imm_config.lua");
	l_reader.map_from_global(g_map);
	l_reader.assign_bool(m_App->m_FullScreen, g_map["is_fullscreen"]);
	l_reader.assign_bool(m_App->m_Enable4xMsaa, g_map["is_4xmsaa"]);
	l_reader.assign_bool(m_App->m_IsSyncInterval, g_map["is_sync_iterval"]);
}
template <typename T_app>
void lua_config<T_app>::additional1()
{
	std::map<std::string, std::string> g_map;
	g_map["is_slient"] = "";
	lua_reader l_reader;
	l_reader.loadfile(GLOBAL["path_lua"]+"imm_config.lua");
	l_reader.map_from_global(g_map);
	l_reader.assign_bool(m_App->m_Cmd.is_slient, g_map["is_slient"]);
}
}
#endif