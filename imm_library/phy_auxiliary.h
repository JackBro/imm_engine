////////////////
// phy_auxiliary.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_AUXILIARY_H
#define PHY_AUXILIARY_H
#include "ia_vertex.h"
#include "imm_camera.h"
#include <DirectXColors.h>
namespace imm
{
////////////////
// phy_wireframe
////////////////
////////////////
template <typename T_app>
struct phy_wireframe
{
	phy_wireframe();
	~phy_wireframe();
	void remove_buffer();
	void remove_all();
	void init(T_app *appin);
	void build_buffer();
	void draw();
	bool is_drawing;
	// Notice Bounding only use BoundingBox for lazy develop!!
	std::vector<ID3D11Buffer*> box_collision;
	std::vector<ID3D11Buffer*> box_attack;
	ID3D11Buffer *box_ib;
	T_app *app;
};
//
template <typename T_app>
phy_wireframe<T_app>::phy_wireframe():
	is_drawing(false),
	box_ib(nullptr),
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_wireframe<T_app>::~phy_wireframe()
{
	remove_buffer();
	ReleaseCOM(box_ib);
}
//
template <typename T_app>
void phy_wireframe<T_app>::remove_buffer()
{
	for (auto &vb: box_collision) ReleaseCOM(vb);
}
//
template <typename T_app>
void phy_wireframe<T_app>::remove_all()
{
	remove_buffer();
	box_collision.clear();
	box_collision.shrink_to_fit();
	box_attack.clear();
	box_attack.shrink_to_fit();
}
//
template <typename T_app>
void phy_wireframe<T_app>::init(T_app *appin)
{
	app = appin;
	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		4, 5, 1,
		4, 1, 0,
		// right face
		3, 2, 6,
		3, 6, 7,
		// top face
		1, 5, 6,
		1, 6, 2,
		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = indices;
	HR(app->m_D3DDevice->CreateBuffer(&ibd, &iinit_data, &box_ib));
}
//
template <typename T_app>
void phy_wireframe<T_app>::build_buffer()
{
	remove_all();	
	// box_collision
	for (size_t ix = 0; ix != app->m_Inst.m_BoundL.map.size(); ++ix) {
		XMFLOAT3 corners[8];
		assert(app->m_Inst.m_BoundL.map[ix].first == phy_bound_type::box);
		app->m_Inst.m_BoundL.b1[app->m_Inst.m_BoundL.map[ix].second].GetCorners(corners);
		vertex_color vertices[] = {
			{corners[0], XMFLOAT4(Colors::Yellow)},
			{corners[1], XMFLOAT4(Colors::Yellow)},
			{corners[2], XMFLOAT4(Colors::Yellow)},
			{corners[3], XMFLOAT4(Colors::Yellow)},
			{corners[4], XMFLOAT4(Colors::Yellow)},
			{corners[5], XMFLOAT4(Colors::Yellow)},
			{corners[6], XMFLOAT4(Colors::Yellow)},
			{corners[7], XMFLOAT4(Colors::Yellow)}
		};
		box_collision.push_back(nullptr);
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(vertex_color)*8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinit_data;
		vinit_data.pSysMem = vertices;
		HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &box_collision.back()));
	}
	// box_attack
	for (size_t ix = 0; ix != app->m_Attack.bbox_l.size(); ++ix) {
		XMFLOAT3 corners[8];
		app->m_Attack.bbox_l[ix].GetCorners(corners);
		vertex_color vertices[] = {
			{corners[0], XMFLOAT4(Colors::Red)},
			{corners[1], XMFLOAT4(Colors::Red)},
			{corners[2], XMFLOAT4(Colors::Red)},
			{corners[3], XMFLOAT4(Colors::Red)},
			{corners[4], XMFLOAT4(Colors::Red)},
			{corners[5], XMFLOAT4(Colors::Red)},
			{corners[6], XMFLOAT4(Colors::Red)},
			{corners[7], XMFLOAT4(Colors::Red)}
		};
		box_attack.push_back(nullptr);
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(vertex_color)*8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinit_data;
		vinit_data.pSysMem = vertices;
		HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &box_attack.back()));
	}
}
//
template <typename T_app>
void phy_wireframe<T_app>::draw()
{
	if (!is_drawing) return;
	color_effect *color_fx = effects::m_ColorFX;
	ID3DX11EffectTechnique *tech = color_fx->m_ColorTech;
	UINT stride = sizeof(vertex_color);
	UINT offset = 0;
	app->m_D3DDC->IASetInputLayout(input_layouts::m_Color);
	app->m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	app->m_D3DDC->RSSetState(render::m_WireframeRS);
	app->m_D3DDC->IASetIndexBuffer(box_ib, DXGI_FORMAT_R32_UINT, 0);	
	XMMATRIX view_proj = app->m_Cam.get_ViewProj();
	// Draw collision box
	for (size_t ix = 0; ix != app->m_Inst.m_BoundL.map.size(); ++ix) {
		app->m_D3DDC->IASetVertexBuffers(0, 1, &box_collision[ix], &stride, &offset);
		XMFLOAT4X4 *inst_world = app->m_Inst.m_Stat[ix].get_World();
		XMMATRIX world = XMLoadFloat4x4(inst_world);
		XMMATRIX world_view_proj = XMMatrixMultiply(world, view_proj);
		color_fx->set_WorldViewProj(world_view_proj);
		D3DX11_TECHNIQUE_DESC tech_desc;
		tech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			tech->GetPassByIndex(p)->Apply(0, app->m_D3DDC);
			// 36 indices for the box.
			app->m_D3DDC->DrawIndexed(36, 0, 0);
		}
	}
	// Draw attack box
	for (auto it_map = app->m_Attack.map.begin(); it_map != app->m_Attack.map.end(); ++it_map) {
		XMFLOAT4X4 *inst_world = app->m_Inst.m_Stat[it_map->first].get_World();
		XMMATRIX world = XMLoadFloat4x4(inst_world);
		for (auto it_box = it_map->second.begin(); it_box != it_map->second.end(); ++it_box) {
			app->m_D3DDC->IASetVertexBuffers(0, 1, &box_attack[it_box->second], &stride, &offset);
			size_t bone_ix =
				app->m_Attack.model[*app->m_Inst.m_Stat[it_map->first].get_ModelName()].box[it_box->first].bone_ix;
			XMMATRIX bone_trans = XMLoadFloat4x4(
				app->m_Inst.m_Stat[it_map->first].get_FinalTransform(bone_ix)
			);
			world = bone_trans*world;
			XMMATRIX world_view_proj = XMMatrixMultiply(world, view_proj);
			color_fx->set_WorldViewProj(world_view_proj);
			D3DX11_TECHNIQUE_DESC tech_desc;
			tech->GetDesc(&tech_desc);
			for(UINT p = 0; p < tech_desc.Passes; ++p) {
				tech->GetPassByIndex(p)->Apply(0, app->m_D3DDC);
				// 36 indices for the box.
				app->m_D3DDC->DrawIndexed(36, 0, 0);
			}
		}
	}
	app->m_D3DDC->RSSetState(0);
}
//
}
#endif