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
	void init(T_app *appin);
	void build_buffer();
	void draw();
	ID3D11Buffer *m_BoxVB;
	ID3D11Buffer *m_BoxIB;
	T_app *app;
};
//
template <typename T_app>
phy_wireframe<T_app>::phy_wireframe():
	m_BoxVB(nullptr),
	m_BoxIB(nullptr)
{
	;	
}
//
template <typename T_app>
phy_wireframe<T_app>::~phy_wireframe()
{
	ReleaseCOM(m_BoxVB);
	ReleaseCOM(m_BoxIB);
}
//
template <typename T_app>
void phy_wireframe<T_app>::init(T_app *appin)
{
	app = appin;
	build_buffer();
}
//
template <typename T_app>
void phy_wireframe<T_app>::build_buffer()
{
	vertex_color vertices[] = {
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Yellow)}
	};
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertex_color)*8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = vertices;
	HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &m_BoxVB));
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
	HR(app->m_D3DDevice->CreateBuffer(&ibd, &iinit_data, &m_BoxIB));
}
//
template <typename T_app>
void phy_wireframe<T_app>::draw()
{
	color_effect *color_fx = effects::m_ColorFX;
	ID3DX11EffectTechnique *tech = color_fx->m_ColorTech;
	UINT stride = sizeof(vertex_color);
	UINT offset = 0;
	app->m_D3DDC->IASetVertexBuffers(0, 1, &m_BoxVB, &stride, &offset);
	app->m_D3DDC->IASetIndexBuffer(m_BoxIB, DXGI_FORMAT_R32_UINT, 0);
	app->m_D3DDC->IASetInputLayout(input_layouts::m_Color);
	app->m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX world_view_proj = XMMatrixMultiply(world, app->m_Cam.get_ViewProj());
	color_fx->set_WorldViewProj(world_view_proj);
	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		tech->GetPassByIndex(p)->Apply(0, app->m_D3DDC);
		// 36 indices for the box.
		app->m_D3DDC->DrawIndexed(36, 0, 0);
	}
}
//
}
#endif