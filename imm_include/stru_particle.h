////////////////
// stru_particle.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_PARTICLE_H
#define STRU_PARTICLE_H
#include "render_particle.h"
#include "mesh_d3d_util.h"
#include "stru_lua_help.h"
#include <list>
namespace imm
{
////////////////
// plasma_type
////////////////
////////////////
enum plasma_type
{
	fire,
	strike,
	lightning,
};
////////////////
// state_plasma
////////////////
////////////////
struct state_plasma
{
	state_plasma();
	~state_plasma();
	void init_load(ID3D11Device *device, ID3D11DeviceContext *context);
	void update(const float &dt, const float &total_time);
	void draw(ID3D11DeviceContext *context, const camera &cam);
	void remove_all();
	void push_back(plasma_type type, const float &count_down, const XMFLOAT3 &pos);
	particle pt_fire;
	particle pt_strike;
	particle pt_lightning;
	std::list<inst_plasam> list_fire;
	std::list<inst_plasam> list_strike;
	std::list<inst_plasam> list_lightning;
	bool is_active;
	size_t index_fire;
	size_t index_strike;
	size_t index_lightning;
	ID3D11ShaderResourceView *tex_fire_srv;
	ID3D11ShaderResourceView *tex_strike_srv;
	ID3D11ShaderResourceView *tex_lightning_srv;
	ID3D11ShaderResourceView *tex_random_srv;
private:
	state_plasma(const state_plasma &rhs);
	state_plasma &operator=(const state_plasma &rhs);
};
//
state_plasma::state_plasma():
	pt_fire(),
	pt_strike(),
	pt_lightning(),
	list_fire(),
	list_strike(),
	list_lightning(),
	is_active(false),
	index_fire(0),
	index_strike(0),
	index_lightning(0),
	tex_fire_srv(nullptr),
	tex_strike_srv(nullptr),
	tex_lightning_srv(nullptr),
	tex_random_srv(nullptr)
{
	;
}
//
state_plasma::~state_plasma()
{
	RELEASE_COM(tex_fire_srv);
	RELEASE_COM(tex_strike_srv);
	RELEASE_COM(tex_lightning_srv);
	RELEASE_COM(tex_random_srv);
}
//
void state_plasma::init_load(ID3D11Device *device, ID3D11DeviceContext *context)
{
	std::string concrete = IMM_PATH["script"]+"scene_common\\state_plasma.lua";
	std::map<std::string, std::string> get_dds;
	get_dds["plasma_fire_dds"] = "";
	get_dds["plasma_strike_dds"] = "";
	get_dds["plasma_lightning_dds"] = "";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	l_reader.map_from_string(get_dds);
	if (csv_value_is_empty(get_dds["plasma_fire_dds"]) ||
		csv_value_is_empty(get_dds["plasma_strike_dds"]) ||
		csv_value_is_empty(get_dds["plasma_lightning_dds"])) {
	//
		is_active = false;
		return;
	}
	else {
		is_active = true;
	}
	std::wstring path_tex = str_to_wstr(IMM_PATH["texture"]+"particle\\");
	tex_random_srv = create_RandomTexture1DSRV(device);
	std::vector<std::wstring> file_names;
	// Fire
	file_names.push_back(path_tex+str_to_wstr(get_dds["plasma_fire_dds"]));
	tex_fire_srv = create_Texture2DArraySRV(device, context, file_names);
	pt_fire.init(device, effects::m_PtFireFX, tex_fire_srv, tex_random_srv, 500, 10);
	// Strike
	file_names.clear();
	file_names.push_back(path_tex+str_to_wstr(get_dds["plasma_strike_dds"]));
	tex_strike_srv = create_Texture2DArraySRV(device, context, file_names);
	pt_strike.init(device, effects::m_PtStrikeFX, tex_strike_srv, tex_random_srv, 500, 10);
	// Lightning
	file_names.clear();
	file_names.push_back(path_tex+str_to_wstr(get_dds["plasma_strike_dds"]));
	tex_lightning_srv = create_Texture2DArraySRV(device, context, file_names);
	pt_lightning.init(device, effects::m_PtLightningFX, tex_lightning_srv, tex_random_srv, 500, 10);
	// Test
	list_fire.emplace_back();
	list_fire.back().pos = XMFLOAT3(10.0f, 20.0f, 0.0f);
	list_fire.back().count_down = 50.0f;
}
//
void state_plasma::update(const float &dt, const float &total_time)
{
	if (!is_active) return;
	pt_fire.update(dt, total_time);
	pt_strike.update(dt, total_time);
	pt_lightning.update(dt, total_time);
	for (auto &inst: list_fire) inst.update(dt);
	for (auto &inst: list_strike) inst.update(dt);
	for (auto &inst: list_lightning) inst.update(dt);
	auto is_should_del =
		[](const inst_plasam &pla) {return (pla.count_down > -99.0f && pla.count_down < 0.0f);};
	list_fire.remove_if(is_should_del);
	list_strike.remove_if(is_should_del);
	list_lightning.remove_if(is_should_del);
	//
}
//
void state_plasma::draw(ID3D11DeviceContext *context, const camera &cam)
{
	if (!is_active) return;
	pt_fire.set_EyePos(cam.get_Position());
	pt_strike.set_EyePos(cam.get_Position());
	pt_lightning.set_EyePos(cam.get_Position());
	pt_fire.draw_list(context, cam, list_fire);
	pt_strike.draw_list(context, cam, list_strike);
	pt_lightning.draw_list(context, cam, list_lightning);
}
//
void state_plasma::remove_all()
{
	list_fire.clear();
	list_strike.clear();
	list_lightning.clear();
}
//
void state_plasma::push_back(plasma_type type, const float &count_down, const XMFLOAT3 &pos)
{
	auto do_push_back =
		[&](size_t &myindex, std::list<inst_plasam> &mylist, const particle &mypt) {
		++myindex;
		if (myindex > mypt.get_ListSize()-1) myindex = 0;
		mylist.emplace_back();
		mylist.back().pos = pos;
		mylist.back().count_down = count_down;
		mylist.back().slot = myindex;
		};
	//
	switch(type) {
	case fire:
		do_push_back(index_fire, list_fire, pt_fire);
		break;
	case strike:
		do_push_back(index_strike, list_strike, pt_strike);
		break;
	case lightning:
		do_push_back(index_lightning, list_lightning, pt_lightning);
		break;
	}
}
//
}
#endif