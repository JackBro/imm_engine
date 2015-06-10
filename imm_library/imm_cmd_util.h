////////////////
// imm_cmd_util.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef IMM_CMD_UTIL_H
#define IMM_CMD_UTIL_H
#include "stru_scene_help.h"
#include <atomic>
namespace imm
{
////////////////
// atomic_wstring
////////////////
////////////////
struct atomic_wstring
{
	std::recursive_mutex mutex1;
	std::wstring str;
	template <typename T_wadde>
	atomic_wstring &operator+=(const T_wadde &str2);
	void pop_back();
	template <typename T_itor>
	atomic_wstring &assign(const T_itor &first, const T_itor &last);
	template <typename T_wstr>
	atomic_wstring &assign(const T_wstr &str2);
	size_t size();
	std::wstring::iterator begin();
	std::wstring::iterator end();
	const wchar_t* c_str();
	void clear();
};
//
template <typename T_wadde>
atomic_wstring &atomic_wstring::operator+=(const T_wadde &str2)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str += str2;
	return *this;
}
//
void atomic_wstring::pop_back()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.pop_back();
}
//
template <typename T_itor>
atomic_wstring &atomic_wstring::assign(const T_itor &first, const T_itor &last)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.assign(first, last);
	return *this;
}
//
template <typename T_wstr>
atomic_wstring &atomic_wstring::assign(const T_wstr &str2)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.assign(str2);
	return *this;
}
//
size_t atomic_wstring::size()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.size();
}
//
std::wstring::iterator atomic_wstring::begin()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.begin();
}
//
std::wstring::iterator atomic_wstring::end()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.end();
}
//
const wchar_t* atomic_wstring::c_str()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.c_str();
}
//
void atomic_wstring::clear()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.clear();
}
////////////////
// misc_util_b3m
////////////////
////////////////
template <typename T_wstring>
void misc_util_b3m(ID3D11Device *d3d_device, T_wstring &input_str, std::atomic<bool> &is_busy)
{
	is_busy = true;
	texture_mgr tex_mgr;
	tex_mgr.init(d3d_device);
	lua_reader l_reader;
	l_reader.loadfile(GLOBAL["path_lua"]+"cmd_util_b3m.lua");
	bin_m3d model_bin;
	std::vector<std::vector<std::string>> model_m3d;
	l_reader.vec2d_str_from_table("csv_model_input", model_m3d);
	std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
	auto it = model_m3d.begin()+1;
	while (it != model_m3d.end()) {
		basic_model model_bas;
		skinned_model model_ski;
		bool is_skinned = false;
		l_reader.assign_bool(is_skinned, (*it)[2]);
		if (is_skinned) {
			model_ski.set(d3d_device, tex_mgr, GLOBAL["path_inp"]+(*it)[1], path_tex);
			model_bin.write_to_bin(model_ski, GLOBAL["path_out"]+(*it)[0]+".b3m");
		}
		else {
			model_bas.set(d3d_device, tex_mgr, GLOBAL["path_inp"]+(*it)[1], path_tex);
			model_bin.write_to_bin(model_bas, GLOBAL["path_out"]+(*it)[0]+".b3m");
		}
		std::wstring temp((*it)[1].begin(), (*it)[1].end());
		input_str += L"\n> ";
		input_str += temp+L" exported OK";
		++it;
	}
	input_str += L"\n> util b3m has finished all conversion.\n";
	is_busy = false;
}
}
#endif