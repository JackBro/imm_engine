////////////////
// render_shadow.h
// ShadowMap.h by Frank Luna (C) 2011 All Rights Reserved.
// Helper class for implementing shadows via shadow mapping algorithm.
////////////////
////////////////
#ifndef RENDER_SHADOW_H
#define RENDER_SHADOW_H
#include "imm_core.h"
namespace imm
{
////////////////
// shadow_map
////////////////
////////////////
class shadow_map
{
public:
	shadow_map(ID3D11Device *device, UINT width, UINT height);
	~shadow_map();
	ID3D11ShaderResourceView *get_DepthMapSRV();
	void bind_dsv_and_set_null_render_target(ID3D11DeviceContext *dc);
private:
	shadow_map(const shadow_map &rhs);
	shadow_map &operator=(const shadow_map &rhs);
	UINT m_Width;
	UINT m_Height;
	ID3D11ShaderResourceView *m_DepthMapSRV;
	ID3D11DepthStencilView *m_DepthMapDSV;
	D3D11_VIEWPORT m_Viewport;
};
//
shadow_map::~shadow_map()
{
	ReleaseCOM(m_DepthMapSRV);
	ReleaseCOM(m_DepthMapDSV);
}
//
shadow_map::shadow_map(ID3D11Device *device, UINT width, UINT height):
	m_Width(width),
	m_Height(height),
	m_DepthMapSRV(0),
	m_DepthMapDSV(0)
{
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width    = static_cast<float>(width);
	m_Viewport.Height   = static_cast<float>(height);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width              = m_Width;
	tex_desc.Height             = m_Height;
	tex_desc.MipLevels          = 1;
	tex_desc.ArraySize          = 1;
	tex_desc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
	tex_desc.SampleDesc.Count   = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage              = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags     = 0;
	tex_desc.MiscFlags          = 0;
	ID3D11Texture2D *depth_map = 0;
	HR(device->CreateTexture2D(&tex_desc, 0, &depth_map));
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags              = 0;
	dsv_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(depth_map, &dsv_desc, &m_DepthMapDSV));
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels       = tex_desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(depth_map, &srvDesc, &m_DepthMapSRV));
	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(depth_map);
}
//
ID3D11ShaderResourceView *shadow_map::get_DepthMapSRV() {return m_DepthMapSRV;}
//
void shadow_map::bind_dsv_and_set_null_render_target(ID3D11DeviceContext *dc)
{
	dc->RSSetViewports(1, &m_Viewport);
	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView *render_targets[1] = {0};
	dc->OMSetRenderTargets(1, render_targets, m_DepthMapDSV);
	dc->ClearDepthStencilView(m_DepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
}
#endif