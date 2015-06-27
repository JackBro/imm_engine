////////////////
// stru_liquid.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_LIQUID_H
#define STRU_LIQUID_H
#include "render_gpu_wave.h"
#include "DDSTextureLoader.h"
#include "mesh_geometry_gen.h"
#include "ia_render.h"
namespace imm
{
////////////////
// state_liquid
////////////////
////////////////
struct state_liquid
{
	state_liquid();
	~state_liquid();
	void init(ID3D11Device* device);
	void update(ID3D11DeviceContext *context, float dt, float total_time);
	void draw(ID3D11DeviceContext *context, light_dir lights[3], const camera &cam1);
	gpu_wave wave;
	material wave_mat;
	ID3D11Buffer *wave_vb;
	ID3D11Buffer *wave_ib;
	ID3D11ShaderResourceView *wave_map_srv;
	XMFLOAT4X4 wave_world;
	XMFLOAT4X4 wave_tex_transform;
	XMFLOAT2 wave_tex_offset;
	UINT wave_index_count;
};
//
state_liquid::state_liquid():
	wave(),
	wave_vb(nullptr),
	wave_ib(nullptr),
	wave_map_srv(nullptr),
	wave_tex_offset(0.0f, 0.0f),
	wave_index_count(0)
{
	XMStoreFloat4x4(&wave_world, XMMatrixIdentity());
	XMStoreFloat4x4(&wave_tex_transform, XMMatrixIdentity());
	wave_world._42 = 20.0f;
	wave_mat.ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	wave_mat.diffuse  = XMFLOAT4(0.27f, 0.55f, 0.79f, 0.5f);
	wave_mat.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}
//
state_liquid::~state_liquid()
{
	ReleaseCOM(wave_vb);
	ReleaseCOM(wave_ib);
	ReleaseCOM(wave_map_srv);
}
//
void state_liquid::init(ID3D11Device* device)
{
	//std::wstring path = str_to_wstr(IMM_PATH["texture"]);
	//path += L"water01_BC7.dds";
	//HR(CreateDDSTextureFromFile(device, path.c_str(), nullptr, &wave_map_srv));
	wave.init(device, 512, 512, 0.5f, 0.03f, 4.0f, 0.2f);
	//
	geometry::mesh_data grid;
	geometry geo_gen;
	geo_gen.create_grid(wave.width(), wave.depth(), wave.row_count(), wave.column_count(), grid);
	wave_index_count = static_cast<UINT>(grid.indices.size());
	//
	// Extract the vertex elements we are interested.
	//
	std::vector<vertex::basic32> vertices(grid.vertices.size());
	for(UINT i = 0; i < grid.vertices.size(); ++i) {
		vertices[i].pos    = grid.vertices[i].position;
		vertices[i].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[i].tex    = grid.vertices[i].tex_c;
	}
	D3D11_BUFFER_DESC vbd;
	vbd.Usage          = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth      = static_cast<UINT>(sizeof(vertex::basic32) * grid.vertices.size());
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags      = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinit_data, &wave_vb));
	D3D11_BUFFER_DESC ibd;
	ibd.Usage          = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth      = sizeof(UINT) * wave_index_count;
	ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags      = 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = &grid.indices[0];
	HR(device->CreateBuffer(&ibd, &iinit_data, &wave_ib));
}
//
void state_liquid::update(ID3D11DeviceContext *context, float dt, float total_time)
{
	//
	// Generate a random wave.
	//
	static float t_base = 0.0f;
	if ((total_time - t_base) >= 0.1f ) {
		t_base += 0.1f;
		DWORD i = 5 + rand() % (wave.row_count()-10);
		DWORD j = 5 + rand() % (wave.column_count()-10);
		float r = math::calc_randf(1.0f, 2.0f);
		wave.disturb(context, i, j, r);
	}
	wave.update(context, dt);
	//
	// Animate water texture coordinates.
	//
	// Tile water texture.
	XMMATRIX wave_scale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	// Translate texture over time.
	wave_tex_offset.y += 0.05f*dt;
	wave_tex_offset.x += 0.1f*dt;
	XMMATRIX wave_offset = XMMatrixTranslation(wave_tex_offset.x, wave_tex_offset.y, 0.0f);
	// Combine scale and translation.
	XMStoreFloat4x4(&wave_tex_transform, wave_scale*wave_offset);
}
//
void state_liquid::draw(ID3D11DeviceContext *context, light_dir lights[3], const camera &cam1)
{
	context->IASetInputLayout(input_layouts::m_Basic32);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(vertex::basic32);
	UINT offset = 0;
	effects::m_WaveRenderFX->set_DirLights(lights);
	effects::m_WaveRenderFX->set_EyePosW(cam1.get_Position());
	ID3DX11EffectTechnique* wave_tech = effects::m_WaveRenderFX->m_Light3Tech;
	//ID3DX11EffectTechnique* wave_tech = effects::m_WaveRenderFX->m_Light3TexTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	XMMATRIX view_proj = cam1.get_ViewProj();
	XMMATRIX world = XMLoadFloat4x4(&wave_world);
	XMMATRIX world_inv_trans = math::inverse_transpose(world);
	XMMATRIX world_view_proj = world*view_proj;
	//
	// Draw the waves.
	//
	context->IASetVertexBuffers(0, 1, &wave_vb, &stride, &offset);
	context->IASetIndexBuffer(wave_ib, DXGI_FORMAT_R32_UINT, 0);
	wave_tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		// Set per object constants.
		effects::m_WaveRenderFX->set_World(world);
		effects::m_WaveRenderFX->set_WorldInvTranspose(world_inv_trans);
		effects::m_WaveRenderFX->set_WorldViewProj(world_view_proj);
		effects::m_WaveRenderFX->set_TexTransform(XMLoadFloat4x4(&wave_tex_transform));
		effects::m_WaveRenderFX->set_Material(wave_mat);
		effects::m_WaveRenderFX->set_DiffuseMap(wave_map_srv);
		effects::m_WaveRenderFX->set_DisplacementMap(wave.get_displacement_map());
		effects::m_WaveRenderFX->set_GridSpatialStep(0.5f);
		effects::m_WaveRenderFX->set_DisplacementTexelSize(XMFLOAT2(1.0f/wave.column_count(), 1.0f/wave.row_count()));
		context->OMSetBlendState(render::m_TransparentBS, BLEND_FACTOR_ZERO, 0xffffffff);
		wave_tech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(wave_index_count, 0, 0);
		// Restore default blend state
		context->OMSetBlendState(0, BLEND_FACTOR_ZERO, 0xffffffff);
	}
	// Unbind displacement map from vertex shader, as we will be binding it as a compute
	// shader resource in the next update.
	ID3D11ShaderResourceView* null_srv[1] = {0};
	context->VSSetShaderResources(0, 1, null_srv);
}
//
}
#endif