////////////////
// ui_text_help.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_TEXT_HELP
#define UI_TEXT_HELP
#include "stru_lua_help.h"
namespace imm
{
////////////////
// http://stackoverflow.com/questions/236129/split-a-string-in-c
////////////////
////////////////
void text_split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}
////////////////
// ui_text_lines
////////////////
////////////////
struct ui_text_lines
{
	std::map<std::string, std::string> speaker;
	std::map<std::string, std::vector<std::wstring>> lines;
};
////////////////
// 
////////////////
////////////////
struct ui_text_chunk
{
	ui_text_chunk();
	void remove_all();
	void insert_chunk(
		const std::string &chunk_name,
		const std::map<std::string, std::string> &chunk_raw);
	std::wstring &get_lines(
		const std::string &chunk_name,
		const std::string &dialogue_id);
	std::map<std::string, ui_text_lines> map;
};
//
ui_text_chunk::ui_text_chunk()
{
	;
}
//
void ui_text_chunk::remove_all()
{
	map.clear();
}
//
void ui_text_chunk::insert_chunk(
	const std::string &chunk_name,
	const std::map<std::string, std::string> &chunk_raw)
{
	map.emplace(chunk_name, ui_text_lines());
	for (auto &raw: chunk_raw) {
		std::vector<std::string> elems;
		text_split(raw.second, '\n', elems);
		size_t found = raw.first.find_first_of("_");
		map[chunk_name].speaker[raw.first] = raw.first.substr(found+1);
		for (size_t ix = 0; ix < elems.size(); ix += 3) {
			if (ix > elems.size()-1) break;
			std::string lines;
			size_t row = 0;
			while (row < 3) {
				lines += elems[ix+row];
				row += 1;
				if (ix+row > elems.size()-1) break;
				if (row < 3) lines += '\n';
			}
			map[chunk_name].lines[raw.first].push_back(str_to_wstr(lines));
		}
	}
}
//
std::wstring &ui_text_chunk::get_lines(
	const std::string &chunk_name,
	const std::string &dialogue_id)
{
	return map[chunk_name].lines[dialogue_id][0];	
}
//
}
#endif