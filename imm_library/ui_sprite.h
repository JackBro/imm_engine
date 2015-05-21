////////////////
// ui_sprite.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_SPRITE_H
#define UI_SPRITE_H
#include "SpriteBatch.h"
#include "mesh_texture_mgr.h"
#include "CommonStates.h"
namespace imm
{
////////////////
// sprite_simple
////////////////
////////////////
struct sprite_simple
{
	sprite_simple();
	~sprite_simple();
	void init(ID3D11Device *device, ID3D11DeviceContext *device_context);
	void draw_d3d();
	std::unique_ptr<SpriteBatch> sprite;
	std::unique_ptr<CommonStates> states;
	texture_mgr tex_mgr;
	std::map<std::string, ID3D11ShaderResourceView*> map_tex;
	std::map<std::string, XMFLOAT2> map_pos;
};
//
sprite_simple::sprite_simple():
	sprite(nullptr),
	states(nullptr),
	tex_mgr()
{
	;
}
//
sprite_simple::~sprite_simple()
{
	//tex_mgr will kill ID3D11ShaderResourceView*
	;
}
//
void sprite_simple::init(ID3D11Device *device, ID3D11DeviceContext *device_context)
{
	sprite = std::unique_ptr<SpriteBatch>(new SpriteBatch(device_context));
	states = std::unique_ptr<CommonStates>(new CommonStates(device));
	tex_mgr.init(device);
	std::wstring tex_path = str_to_wstr(GLOBAL["path_tex"]);
	tex_path += L"hello_world_BC7.dds";
	//map_tex["test"] = tex_mgr.create_texture(tex_path);
}
//
void sprite_simple::draw_d3d()
{
	sprite->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	for (auto it = map_tex.begin(); it != map_tex.end(); ++it) {
		sprite->Draw(it->second, XMFLOAT2(10, 10));
	}
	sprite->End();
}
//
}
#endif