////////////////
// stru_element.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_ELEMENT_H
#define STRU_ELEMENT_H
#include "cast_particle.h"
#include "mesh_d3d_util.h"
namespace imm
{
////////////////
// flare
////////////////
////////////////
struct flare
{
	flare();
	~flare();
	void init(ID3D11Device *device, ID3D11DeviceContext *context);
	void update(float dt, float total_time);
	void draw(ID3D11DeviceContext *context, const camera &cam);
	particle fragment;
	int instance_ix;
	ID3D11ShaderResourceView *flare_tex_srv;
	ID3D11ShaderResourceView *random_tex_srv;
};
//
flare::flare():
	instance_ix(-1),
	flare_tex_srv(nullptr),
	random_tex_srv(nullptr)
{
	;
}
//
flare::~flare()
{
	ReleaseCOM(flare_tex_srv);
	ReleaseCOM(random_tex_srv);
}
//
void flare::init(ID3D11Device *device, ID3D11DeviceContext *context)
{
	std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
	random_tex_srv = create_RandomTexture1DSRV(device);
	std::vector<std::wstring> flares;
	flares.push_back(path_tex+L"flare0_BC7.dds");
	flare_tex_srv = create_Texture2DArraySRV(device, context, flares);
	fragment.init(device, effects::m_FireFX, flare_tex_srv, random_tex_srv, 500);
	fragment.set_EmitPos(XMFLOAT3(0.0f, 1.0f, 10.0f));
}
//
void flare::update(float dt, float total_time)
{
	fragment.update(dt, total_time);
}
//
void flare::draw(ID3D11DeviceContext *context, const camera &cam)
{
	fragment.set_EyePos(cam.get_Position());
	fragment.draw(context, cam);
}
//
}
#endif