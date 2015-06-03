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
	particle fragment;
	int instance_ix;
	bool is_active;
	ID3D11ShaderResourceView *state_plasma_tex_srv;
	ID3D11ShaderResourceView *random_tex_srv;
};
//
state_plasma::state_plasma():
	instance_ix(-1),
	is_active(true),
	state_plasma_tex_srv(nullptr),
	random_tex_srv(nullptr)
{
	;
}
//
state_plasma::~state_plasma()
{
	ReleaseCOM(state_plasma_tex_srv);
	ReleaseCOM(random_tex_srv);
}
//
void state_plasma::init_load(ID3D11Device *device, ID3D11DeviceContext *context)
{
	std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
	random_tex_srv = create_RandomTexture1DSRV(device);
	std::vector<std::wstring> flares;
	flares.push_back(path_tex+L"flare1_BC7.dds");
	state_plasma_tex_srv = create_Texture2DArraySRV(device, context, flares);
	fragment.init(device, effects::m_FireFX, state_plasma_tex_srv, random_tex_srv, 500);
	fragment.set_EmitPos(XMFLOAT3(2.0f, 1.0f, -7.0f));
}
//
void state_plasma::update(float dt, float total_time)
{
	if (!is_active) return;
	fragment.update(dt, total_time);
}
//
void state_plasma::draw(ID3D11DeviceContext *context, const camera &cam)
{
	if (!is_active) return;
	fragment.set_EyePos(cam.get_Position());
	fragment.draw(context, cam);
}
//
void state_plasma::reset()
{
	fragment.reset();
}
//
}
#endif