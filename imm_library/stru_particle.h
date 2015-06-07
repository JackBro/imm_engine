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
namespace imm
{
////////////////
// state_plasma
////////////////
////////////////
struct state_plasma
{
	state_plasma();
	~state_plasma();
	void init_load(ID3D11Device *device, ID3D11DeviceContext *context);
	void update(float dt, float total_time);
	void draw(ID3D11DeviceContext *context, const camera &cam);
	void reset();
	particle flame;
	std::vector<XMFLOAT3> flame_emit_pos;
	bool is_active;
	ID3D11ShaderResourceView *flame_tex_srv;
	ID3D11ShaderResourceView *random_tex_srv;
};
//
state_plasma::state_plasma():
	is_active(true),
	flame_tex_srv(nullptr),
	random_tex_srv(nullptr)
{
	;
}
//
state_plasma::~state_plasma()
{
	ReleaseCOM(flame_tex_srv);
	ReleaseCOM(random_tex_srv);
}
//
void state_plasma::init_load(ID3D11Device *device, ID3D11DeviceContext *context)
{
	// Flame
	std::string describe = GLOBAL["path_lua"]+"scene_common\\state_plasma.lua";
	std::map<std::string, std::string> get_dds;
	get_dds["plasma_flame_dds"] = "";
	lua_reader l_reader;
	l_reader.loadfile(describe);
	l_reader.map_from_global(get_dds);
	if (csv_value_is_empty(get_dds["plasma_flame_dds"])) {
		is_active = false;
		return;
	}
	std::wstring path_tex = str_to_wstr(GLOBAL["path_tex"]);
	random_tex_srv = create_RandomTexture1DSRV(device);
	std::vector<std::wstring> flares;
	flares.push_back(path_tex+str_to_wstr(get_dds["plasma_flame_dds"]));
	flame_tex_srv = create_Texture2DArraySRV(device, context, flares);
	flame.init(device, effects::m_FireFX, flame_tex_srv, random_tex_srv, 500);
	// Dummy set
	flame.set_EmitPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
}
//
void state_plasma::update(float dt, float total_time)
{
	if (!is_active) return;
	flame.update(dt, total_time);
}
//
void state_plasma::draw(ID3D11DeviceContext *context, const camera &cam)
{
	if (!is_active) return;
	flame.set_EyePos(cam.get_Position());
	flame.draw_list(context, cam, flame_emit_pos);
	
}
//
void state_plasma::reset()
{
	if (!is_active) return;
	flame.reset();
}
//
}
#endif