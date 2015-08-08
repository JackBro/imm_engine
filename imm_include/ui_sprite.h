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
#include "SpriteFont.h"
#include "CommonStates.h"
#include "mesh_texture_mgr.h"
#include "D2d1.h"
namespace imm
{
////////////////
// ui_rect
////////////////
////////////////
struct ui_rect
{
	enum type {
		background  = 0,
		button      = 1,
		text_pure   = 2,
		text_layout = 3,
		sprite      = 4,
	};
	ui_rect();
	type tp;
	std::string brush_ix;
	std::vector<std::string> brush_sel; // button has two brush, for represents selected or not
	D2D1_RECT_F rc;
	std::wstring text;
	std::string id_str;
	std::string parent_str;
	std::string group;
	std::string dwrite_ix;
	XMFLOAT4 margin;
	int parent;
	bool active;
};
//
ui_rect::ui_rect():
	tp(type::background),
	parent(-1),
	active(false)
{
	;
}
////////////////
// sprite_simple
////////////////
////////////////
struct sprite_simple
{
	sprite_simple();
	~sprite_simple();
	void init(ID3D11Device *device, ID3D11DeviceContext *device_context);
	void build_buffer(const std::map<std::string, std::string> &get_dds);
	void draw_d3d(
		const std::map<std::string, size_t> &map_sprite_rect,
		const std::vector<ui_rect> &rect,
		const std::map<std::string, std::string> &map_sprite_name);
	void on_resize(float scale_in, const std::map<std::string, XMFLOAT2> &get_resize);
	std::unique_ptr<SpriteBatch> sprite_batch;
	std::unique_ptr<SpriteFont> sprite_font;
	std::unique_ptr<CommonStates> states;
	texture_mgr tex_mgr;
	std::map<std::string, ID3D11ShaderResourceView*> map_tex;
	std::map<std::string, XMFLOAT2> map_pos;
	float scale;
	bool is_built;
private:
	sprite_simple(const sprite_simple &rhs);
	sprite_simple &operator=(const sprite_simple &rhs);
};
//
sprite_simple::sprite_simple():
	sprite_batch(nullptr),
	sprite_font(nullptr),
	states(nullptr),
	tex_mgr(),
	scale(1.0f),
	is_built(false)
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
	sprite_batch = std::unique_ptr<SpriteBatch>(new SpriteBatch(device_context));
	states = std::unique_ptr<CommonStates>(new CommonStates(device));
	tex_mgr.init(device);
}
void sprite_simple::build_buffer(const std::map<std::string, std::string> &get_dds)
{
	std::wstring tex_path = str_to_wstr(IMM_PATH["texture"]);
	for (auto it = get_dds.begin(); it != get_dds.end(); ++it) {
		if (it->second == "") return;
		std::wstring tex_path_dds = tex_path + str_to_wstr(it->second);
		map_tex[it->first] = tex_mgr.create_texture(tex_path_dds);
	}
	if (map_tex.size() > 0) is_built = true;
}
//
void sprite_simple::draw_d3d(
	const std::map<std::string, size_t> &map_sprite_rect,
	const std::vector<ui_rect> &rect,
	const std::map<std::string, std::string> &map_sprite_name)
{
	if (!is_built) return;
	sprite_batch->Begin(
		SpriteSortMode_Deferred,
		states->NonPremultiplied());
	// Draw sprite according UI rect active
	for (auto &map: map_sprite_rect) {
		if (rect[map.second].active) {
			const std::string *s_name = &map_sprite_name.at(map.first);
			if (!map_tex.count(*s_name)) continue;
			sprite_batch->Draw(
				map_tex[*s_name],
				map_pos[*s_name],
				nullptr,
				Colors::White,
				0,
				XMFLOAT2(0.0f, 0.0f),
				scale);
		}
	}
	sprite_batch->End();
}
//
void sprite_simple::on_resize(float scale_in, const std::map<std::string, XMFLOAT2> &get_resize)
{
	scale = scale_in;
	for (auto it = get_resize.begin(); it != get_resize.end(); ++it) {
		map_pos[it->first] = it->second;
	}
}
//
}
#endif