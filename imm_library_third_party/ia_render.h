////////////////
// ia_render.h
// RenderStates.h by Frank Luna (C) 2011 All Rights Reserved.
// Defines render state objects.
////////////////
////////////////
#ifndef IA_RENDER_H
#define IA_RENDER_H
#include "imm_core.h"
namespace imm
{
////////////////
// render
////////////////
////////////////
class render
{
public:
	static void init_all(ID3D11Device *device);
	static void destroy_all();
	// Rasterizer states
	static ID3D11RasterizerState *m_WireframeRS;
	static ID3D11RasterizerState *m_NoCullRS;
	static ID3D11RasterizerState *m_CullClockwiseRS;
	// Blend states
	static ID3D11BlendState *m_AlphaToCoverageBS;
	static ID3D11BlendState *m_TransparentBS;
	static ID3D11BlendState *m_NoRenderTargetWritesBS;
	// Depth/stencil states
	static ID3D11DepthStencilState *m_MarkMirrorDSS;
	static ID3D11DepthStencilState *m_DrawReflectionDSS;
	static ID3D11DepthStencilState *m_NoDoubleBlendDSS;
	static ID3D11DepthStencilState *m_LessEqualDSS;
	static ID3D11DepthStencilState *m_EqualsDSS;
};
//
ID3D11RasterizerState *render::m_WireframeRS = 0;
ID3D11RasterizerState *render::m_NoCullRS = 0;
ID3D11RasterizerState *render::m_CullClockwiseRS = 0;
//
ID3D11BlendState *render::m_AlphaToCoverageBS = 0;
ID3D11BlendState *render::m_TransparentBS = 0;
ID3D11BlendState *render::m_NoRenderTargetWritesBS = 0;
//
ID3D11DepthStencilState *render::m_MarkMirrorDSS = 0;
ID3D11DepthStencilState *render::m_DrawReflectionDSS = 0;
ID3D11DepthStencilState *render::m_NoDoubleBlendDSS = 0;
ID3D11DepthStencilState *render::m_LessEqualDSS = 0;
ID3D11DepthStencilState *render::m_EqualsDSS = 0;
//
void render::init_all(ID3D11Device *device)
{
////////////////
// Rasterizer states
////////////////
////////////////
	// m_WireframeRS
	D3D11_RASTERIZER_DESC wireframe_desc;
	ZeroMemory(&wireframe_desc, sizeof(D3D11_RASTERIZER_DESC));
	wireframe_desc.FillMode              = D3D11_FILL_WIREFRAME;
	wireframe_desc.CullMode              = D3D11_CULL_BACK;
	wireframe_desc.FrontCounterClockwise = false;
	wireframe_desc.DepthClipEnable       = true;
	HR(device->CreateRasterizerState(&wireframe_desc, &m_WireframeRS));
	// m_NoCullRS
	D3D11_RASTERIZER_DESC no_cull_desc;
	ZeroMemory(&no_cull_desc, sizeof(D3D11_RASTERIZER_DESC));
	no_cull_desc.FillMode              = D3D11_FILL_SOLID;
	no_cull_desc.CullMode              = D3D11_CULL_NONE;
	no_cull_desc.FrontCounterClockwise = false;
	no_cull_desc.DepthClipEnable       = true;
	HR(device->CreateRasterizerState(&no_cull_desc, &m_NoCullRS));
	// m_CullClockwiseRS
	// Note: Define such that we still cull backfaces by making front faces CCW.
	// If we did not cull backfaces, then we have to worry about the BackFace
	// property in the D3D11_DEPTH_STENCIL_DESC.
	D3D11_RASTERIZER_DESC cull_clockwise_desc;
	ZeroMemory(&cull_clockwise_desc, sizeof(D3D11_RASTERIZER_DESC));
	cull_clockwise_desc.FillMode              = D3D11_FILL_SOLID;
	cull_clockwise_desc.CullMode              = D3D11_CULL_BACK;
	cull_clockwise_desc.FrontCounterClockwise = true;
	cull_clockwise_desc.DepthClipEnable       = true;
	HR(device->CreateRasterizerState(&cull_clockwise_desc, &m_CullClockwiseRS));
////////////////
// Blend states
////////////////
////////////////
	// m_AlphaToCoverageBS
	D3D11_BLEND_DESC alpha_to_coverage_desc = {0};
	alpha_to_coverage_desc.AlphaToCoverageEnable                 = true;
	alpha_to_coverage_desc.IndependentBlendEnable                = false;
	alpha_to_coverage_desc.RenderTarget[0].BlendEnable           = false;
	alpha_to_coverage_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&alpha_to_coverage_desc, &m_AlphaToCoverageBS));
	// m_TransparentBS
	D3D11_BLEND_DESC transparent_desc = {0};
	transparent_desc.AlphaToCoverageEnable                 = false;
	transparent_desc.IndependentBlendEnable                = false;
	transparent_desc.RenderTarget[0].BlendEnable           = true;
	transparent_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
	transparent_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
	transparent_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	transparent_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	transparent_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	transparent_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	transparent_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&transparent_desc, &m_TransparentBS));
	// m_NoRenderTargetWritesBS
	D3D11_BLEND_DESC no_render_target_writes_desc = {0};
	no_render_target_writes_desc.AlphaToCoverageEnable                 = false;
	no_render_target_writes_desc.IndependentBlendEnable                = false;
	no_render_target_writes_desc.RenderTarget[0].BlendEnable           = false;
	no_render_target_writes_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
	no_render_target_writes_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
	no_render_target_writes_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	no_render_target_writes_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	no_render_target_writes_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	no_render_target_writes_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	no_render_target_writes_desc.RenderTarget[0].RenderTargetWriteMask = 0;
	HR(device->CreateBlendState(&no_render_target_writes_desc, &m_NoRenderTargetWritesBS));
////////////////
// Depth/stencil states
////////////////
////////////////
	// m_MarkMirrorDSS
	D3D11_DEPTH_STENCIL_DESC mirror_desc;
	mirror_desc.DepthEnable                  = true;
	mirror_desc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ZERO;
	mirror_desc.DepthFunc                    = D3D11_COMPARISON_LESS;
	mirror_desc.StencilEnable                = true;
	mirror_desc.StencilReadMask              = 0xff;
	mirror_desc.StencilWriteMask             = 0xff;
	mirror_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	mirror_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirror_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_REPLACE;
	mirror_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
	// We are not rendering backfacing polygons, so these settings do not matter.
	mirror_desc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
	mirror_desc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;
	mirror_desc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_REPLACE;
	mirror_desc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;
	HR(device->CreateDepthStencilState(&mirror_desc, &m_MarkMirrorDSS));
	// m_DrawReflectionDSS
	D3D11_DEPTH_STENCIL_DESC draw_reflection_desc;
	draw_reflection_desc.DepthEnable                  = true;
	draw_reflection_desc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
	draw_reflection_desc.DepthFunc                    = D3D11_COMPARISON_LESS;
	draw_reflection_desc.StencilEnable                = true;
	draw_reflection_desc.StencilReadMask              = 0xff;
	draw_reflection_desc.StencilWriteMask             = 0xff;
	draw_reflection_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_EQUAL;
	// We are not rendering backfacing polygons, so these settings do not matter.
	draw_reflection_desc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.BackFace.StencilFunc         = D3D11_COMPARISON_EQUAL;
	HR(device->CreateDepthStencilState(&draw_reflection_desc, &m_DrawReflectionDSS));
	// m_NoDoubleBlendDSS
	D3D11_DEPTH_STENCIL_DESC no_double_blend_desc;
	no_double_blend_desc.DepthEnable                  = true;
	no_double_blend_desc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
	no_double_blend_desc.DepthFunc                    = D3D11_COMPARISON_LESS;
	no_double_blend_desc.StencilEnable                = true;
	no_double_blend_desc.StencilReadMask              = 0xff;
	no_double_blend_desc.StencilWriteMask             = 0xff;
	no_double_blend_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_INCR;
	no_double_blend_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_EQUAL;
	// We are not rendering backfacing polygons, so these settings do not matter.
	no_double_blend_desc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_INCR;
	no_double_blend_desc.BackFace.StencilFunc         = D3D11_COMPARISON_EQUAL;
	HR(device->CreateDepthStencilState(&no_double_blend_desc, &m_NoDoubleBlendDSS));
	// m_LessEqualDSS
	D3D11_DEPTH_STENCIL_DESC less_equal_desc;
	less_equal_desc.DepthEnable    = true;
	less_equal_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	less_equal_desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
	less_equal_desc.StencilEnable  = false;
	HR(device->CreateDepthStencilState(&less_equal_desc, &m_LessEqualDSS));
	// m_EqualsDSS
	D3D11_DEPTH_STENCIL_DESC equals_esc;
	ZeroMemory(&equals_esc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equals_esc.DepthEnable    = true;
	equals_esc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	equals_esc.DepthFunc      = D3D11_COMPARISON_EQUAL;
	HR(device->CreateDepthStencilState(&equals_esc, &m_EqualsDSS));
}
void render::destroy_all()
{
	ReleaseCOM(m_WireframeRS);
	ReleaseCOM(m_NoCullRS);
	ReleaseCOM(m_CullClockwiseRS);
	//
	ReleaseCOM(m_AlphaToCoverageBS);
	ReleaseCOM(m_TransparentBS);
	ReleaseCOM(m_NoRenderTargetWritesBS);
	//
	ReleaseCOM(m_MarkMirrorDSS);
	ReleaseCOM(m_DrawReflectionDSS);
	ReleaseCOM(m_NoDoubleBlendDSS);
	ReleaseCOM(m_LessEqualDSS);
	ReleaseCOM(m_EqualsDSS);
}
}
#endif