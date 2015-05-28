////////////////
// phy_auxiliary.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_AUXILIARY_H
#define PHY_AUXILIARY_H
#include "phy_prepare.h"
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
	void remove();
	void reset();
	void init(T_app *appin);
	void build_buffer_update();
	void draw();
	bool is_initialized;
	bool is_drawing;
	// Notice Bounding only use BoundingBox for lazy develop!!
	std::vector<ID3D11Buffer*> box_vb;
	std::vector<ID3D11Buffer*> box_ib;
	T_app *app;
};
//
template <typename T_app>
phy_wireframe<T_app>::phy_wireframe():
	is_initialized(false),
	is_drawing(false),
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_wireframe<T_app>::~phy_wireframe()
{
	remove();
}
//
template <typename T_app>
void phy_wireframe<T_app>::remove()
{
	for (auto &vb: box_vb) ReleaseCOM(vb);
	for (auto &ib: box_ib) ReleaseCOM(ib);
}
//
template <typename T_app>
void phy_wireframe<T_app>::reset()
{
	remove();
	is_initialized = false;
	box_vb.clear();
	box_ib.clear();
	box_vb.shrink_to_fit();
	box_ib.shrink_to_fit();
}
//
template <typename T_app>
void phy_wireframe<T_app>::init(T_app *appin)
{
	app = appin;
}
//
template <typename T_app>
void phy_wireframe<T_app>::build_buffer_update()
{
	if (is_initialized || app->m_Inst.m_IsLoading) return;
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
		box_vb.push_back(nullptr);
		box_ib.push_back(nullptr);
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(vertex_color)*8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinit_data;
		vinit_data.pSysMem = vertices;
		HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &box_vb.back()));
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
		HR(app->m_D3DDevice->CreateBuffer(&ibd, &iinit_data, &box_ib.back()));
	}
	is_initialized = true;
}
//
template <typename T_app>
void phy_wireframe<T_app>::draw()
{
	if (!is_initialized || !is_drawing) return;
	color_effect *color_fx = effects::m_ColorFX;
	ID3DX11EffectTechnique *tech = color_fx->m_ColorTech;
	UINT stride = sizeof(vertex_color);
	UINT offset = 0;
	app->m_D3DDC->IASetInputLayout(input_layouts::m_Color);
	app->m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	app->m_D3DDC->RSSetState(render::m_WireframeRS);
	XMMATRIX view_proj = app->m_Cam.get_ViewProj();
	for (size_t ix = 0; ix != app->m_Inst.m_BoundL.map.size(); ++ix) {
		app->m_D3DDC->IASetVertexBuffers(0, 1, &box_vb[ix], &stride, &offset);
		app->m_D3DDC->IASetIndexBuffer(box_ib[ix], DXGI_FORMAT_R32_UINT, 0);
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
	app->m_D3DDC->RSSetState(0);
}
//
}
#endif