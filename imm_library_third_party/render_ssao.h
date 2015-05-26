////////////////
// render_ssao.h
// Ssao.h by Frank Luna (C) 2011 All Rights Reserved.
// Class encapsulates data and methods to perform screen space ambient occlusion.
////////////////
////////////////
#ifndef RENDER_SSAO_H
#define RENDER_SSAO_H
#include "ia_vertex.h"
#include "imm_camera.h"
#include "imm_core.h"
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
using namespace DirectX::PackedVector;
namespace imm
{
////////////////
// ssao
// Should we render AO at half resolution?
////////////////
////////////////
class ssao
{
public:
	ssao(ID3D11Device *device, ID3D11DeviceContext *dc, int width, int height, float fovy, float far_z);
	~ssao();
	ID3D11ShaderResourceView *get_NormalDepthSRV();
	ID3D11ShaderResourceView *get_AmbientSRV();
	// Call when the backbuffer is resized.
	void on_size(int width, int height, float fovy, float far_z);
	// Changes the render target to the NormalDepth render target.	Pass the
	// main depth buffer as the depth buffer to use when we render to the
	// NormalDepth map.  This pass lays down the scene depth so that there in
	// no overdraw in the subsequent rendering pass.
	void set_NormalDepthRenderTarget(ID3D11DepthStencilView *dsv);
	// Changes the render target to the Ambient render target and draws a fullscreen
	// quad to kick off the pixel shader to compute the AmbientMap.  We still keep the
	// main depth buffer binded to the pipeline, but depth buffer read/writes
	// are disabled, as we do not need the depth buffer computing the Ambient map.
	void compute_ssao(const camera &cam1);
	// Blurs the ambient map to smooth out the noise caused by only taking a
	// few random samples per pixel.  We use an edge preserving blur so that
	// we do not blur across discontinuities--we want edges to remain edges.
	void blur_ambient_map(int blur_count);
private:
	ssao(const ssao &rhs);
	ssao &operator=(const ssao &rhs);
	void blur_ambient_map(ID3D11ShaderResourceView *input_srv, ID3D11RenderTargetView *output_rtv, bool horz_blur);
	void build_FrustumFarCorners(float fovy, float far_z);
	void build_FullScreenQuad();
	void build_TextureViews();
	void release_TextureViews();
	void build_RandomVectorTexture();
	void build_OffsetVectors();
	void draw_FullScreenQuad();
	//
	ID3D11Device *m_D3DDevice;
	ID3D11DeviceContext *m_DC;
	ID3D11Buffer *m_ScreenQuadVB;
	ID3D11Buffer *m_ScreenQuadIB;
	ID3D11ShaderResourceView *m_RandomVectorSRV;
	ID3D11RenderTargetView *m_NormalDepthRTV;
	ID3D11ShaderResourceView *m_NormalDepthSRV;
	// Need two for ping-ponging during blur.
	ID3D11RenderTargetView *m_AmbientRTV0;
	ID3D11ShaderResourceView *m_AmbientSRV0;
	ID3D11RenderTargetView *m_AmbientRTV1;
	ID3D11ShaderResourceView *m_AmbientSRV1;
	UINT m_RenderTargetWidth;
	UINT m_RenderTargetHeight;
	XMFLOAT4 m_FrustumFarCorner[4];
	XMFLOAT4 m_Offsets[14];
	D3D11_VIEWPORT m_AmbientMapViewport;
};
//
ssao::ssao(ID3D11Device *device, ID3D11DeviceContext *dc, int width, int height, float fovy, float far_z):
	m_D3DDevice(device),
	m_DC(dc),
	m_ScreenQuadVB(0),
	m_ScreenQuadIB(0),
	m_RandomVectorSRV(0),
	m_NormalDepthRTV(0),
	m_NormalDepthSRV(0),
	m_AmbientRTV0(0),
	m_AmbientSRV0(0),
	m_AmbientRTV1(0),
	m_AmbientSRV1(0)
{
	on_size(width, height, fovy, far_z);
	build_FullScreenQuad();
	build_OffsetVectors();
	build_RandomVectorTexture();
}
//
ssao::~ssao()
{
	ReleaseCOM(m_ScreenQuadVB);
	ReleaseCOM(m_ScreenQuadIB);
	ReleaseCOM(m_RandomVectorSRV);
	release_TextureViews();
}
//
ID3D11ShaderResourceView *ssao::get_NormalDepthSRV() {return m_NormalDepthSRV;}
ID3D11ShaderResourceView *ssao::get_AmbientSRV() {return m_AmbientSRV0;}
//
void ssao::on_size(int width, int height, float fovy, float far_z)
{
	m_RenderTargetWidth = width;
	m_RenderTargetHeight = height;
	// We render to ambient map at half the resolution.
	m_AmbientMapViewport.TopLeftX	= 0.0f;
	m_AmbientMapViewport.TopLeftY	= 0.0f;
	m_AmbientMapViewport.Width		= width / 2.0f;
	m_AmbientMapViewport.Height		= height / 2.0f;
	m_AmbientMapViewport.MinDepth	= 0.0f;
	m_AmbientMapViewport.MaxDepth	= 1.0f;
	build_FrustumFarCorners(fovy, far_z);
	build_TextureViews();
}
//
void ssao::set_NormalDepthRenderTarget(ID3D11DepthStencilView *dsv)
{
	ID3D11RenderTargetView *render_targets[1] = {m_NormalDepthRTV};
	m_DC->OMSetRenderTargets(1, render_targets, dsv);
	// Clear view space normal to (0,0,-1) and clear depth to be very far away.
	float clear_color[] = {0.0f, 0.0f, -1.0f, 1e5f};
	m_DC->ClearRenderTargetView(m_NormalDepthRTV, clear_color);
}
//
void ssao::compute_ssao(const camera &cam1)
{
	// Bind the ambient map as the render target.  Observe that this pass does not bind
	// a depth/stencil buffer--it does not need it, and without one, no depth test is
	// performed, which is what we want.
	ID3D11RenderTargetView *render_targets[1] = {m_AmbientRTV0};
	m_DC->OMSetRenderTargets(1, render_targets, 0);
	m_DC->ClearRenderTargetView(m_AmbientRTV0, reinterpret_cast<const float*>(&Colors::Black));
	m_DC->RSSetViewports(1, &m_AmbientMapViewport);
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	static const XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX P	= cam1.get_Proj();
	XMMATRIX PT = XMMatrixMultiply(P, T);
	effects::m_SsaoFX->set_ViewToTexSpace(PT);
	effects::m_SsaoFX->set_OffsetVectors(m_Offsets);
	effects::m_SsaoFX->set_FrustumCorners(m_FrustumFarCorner);
	effects::m_SsaoFX->set_NormalDepthMap(m_NormalDepthSRV);
	effects::m_SsaoFX->set_RandomVecMap(m_RandomVectorSRV);
	UINT stride = sizeof(basic32);
	UINT offset = 0;
	m_DC->IASetInputLayout(input_layouts::m_Basic32);
	m_DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DC->IASetVertexBuffers(0, 1, &m_ScreenQuadVB, &stride, &offset);
	m_DC->IASetIndexBuffer(m_ScreenQuadIB, DXGI_FORMAT_R16_UINT, 0);
	ID3DX11EffectTechnique *tech = effects::m_SsaoFX->m_SsaoTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p) {
		tech->GetPassByIndex(p)->Apply(0, m_DC);
		m_DC->DrawIndexed(6, 0, 0);
	}
}
//
void ssao::blur_ambient_map(int blur_count)
{
	for (int i = 0; i < blur_count; ++i) {
		// Ping-pong the two ambient map textures as we apply
		// horizontal and vertical blur passes.
		blur_ambient_map(m_AmbientSRV0, m_AmbientRTV1, true);
		blur_ambient_map(m_AmbientSRV1, m_AmbientRTV0, false);
	}
}
//
void ssao::blur_ambient_map(ID3D11ShaderResourceView *input_srv, ID3D11RenderTargetView *output_rtv, bool horz_blur)
{
	ID3D11RenderTargetView *render_targets[1] = {output_rtv};
	m_DC->OMSetRenderTargets(1, render_targets, 0);
	m_DC->ClearRenderTargetView(output_rtv, reinterpret_cast<const float*>(&Colors::Black));
	m_DC->RSSetViewports(1, &m_AmbientMapViewport);
	effects::m_SsaoBlurFX->set_TexelWidth(1.0f / m_AmbientMapViewport.Width);
	effects::m_SsaoBlurFX->set_TexelHeight(1.0f / m_AmbientMapViewport.Height);
	effects::m_SsaoBlurFX->set_NormalDepthMap(m_NormalDepthSRV);
	effects::m_SsaoBlurFX->set_InputImage(input_srv);
	ID3DX11EffectTechnique *tech;
	if (horz_blur) tech = effects::m_SsaoBlurFX->m_HorzBlurTech;
	else tech = effects::m_SsaoBlurFX->m_VertBlurTech;
	UINT stride = sizeof(basic32);
	UINT offset = 0;
	m_DC->IASetInputLayout(input_layouts::m_Basic32);
	m_DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DC->IASetVertexBuffers(0, 1, &m_ScreenQuadVB, &stride, &offset);
	m_DC->IASetIndexBuffer(m_ScreenQuadIB, DXGI_FORMAT_R16_UINT, 0);
	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p) {
		tech->GetPassByIndex(p)->Apply(0, m_DC);
		m_DC->DrawIndexed(6, 0, 0);
		// Unbind the input SRV as it is going to be an output in the next blur.
		effects::m_SsaoBlurFX->set_InputImage(0);
		tech->GetPassByIndex(p)->Apply(0, m_DC);
	}
}
//
void ssao::build_FrustumFarCorners(float fovy, float far_z)
{
	float aspect = (float)m_RenderTargetWidth / (float)m_RenderTargetHeight;
	float half_height = far_z * tanf(0.5f*fovy);
	float half_width  = aspect * half_height;
	m_FrustumFarCorner[0] = XMFLOAT4(-half_width, -half_height, far_z, 0.0f);
	m_FrustumFarCorner[1] = XMFLOAT4(-half_width, +half_height, far_z, 0.0f);
	m_FrustumFarCorner[2] = XMFLOAT4(+half_width, +half_height, far_z, 0.0f);
	m_FrustumFarCorner[3] = XMFLOAT4(+half_width, -half_height, far_z, 0.0f);
}
//
void ssao::build_FullScreenQuad()
{
	basic32 v[4];
	v[0].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	v[1].pos = XMFLOAT3(-1.0f, +1.0f, 0.0f);
	v[2].pos = XMFLOAT3(+1.0f, +1.0f, 0.0f);
	v[3].pos = XMFLOAT3(+1.0f, -1.0f, 0.0f);
	// Store far plane frustum corner indices in Normal.x slot.
	v[0].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	v[1].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	v[2].normal = XMFLOAT3(2.0f, 0.0f, 0.0f);
	v[3].normal = XMFLOAT3(3.0f, 0.0f, 0.0f);
	v[0].tex = XMFLOAT2(0.0f, 1.0f);
	v[1].tex = XMFLOAT2(0.0f, 0.0f);
	v[2].tex = XMFLOAT2(1.0f, 0.0f);
	v[3].tex = XMFLOAT2(1.0f, 1.0f);
	D3D11_BUFFER_DESC vbd;
	vbd.Usage				= D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth			= sizeof(basic32) * 4;
	vbd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags		= 0;
	vbd.MiscFlags			= 0;
	vbd.StructureByteStride	= 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = v;
	HR(m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &m_ScreenQuadVB));
	USHORT indices[6] = {
		0, 1, 2,
		0, 2, 3
	};
	D3D11_BUFFER_DESC ibd;
	ibd.Usage				= D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth			= sizeof(USHORT) * 6;
	ibd.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags		= 0;
	ibd.StructureByteStride	= 0;
	ibd.MiscFlags			= 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = indices;
	HR(m_D3DDevice->CreateBuffer(&ibd, &iinit_data, &m_ScreenQuadIB));
}
//
void ssao::build_TextureViews()
{
	release_TextureViews();
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width				= m_RenderTargetWidth;
	tex_desc.Height				= m_RenderTargetHeight;
	tex_desc.MipLevels			= 1;
	tex_desc.ArraySize			= 1;
	tex_desc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
	tex_desc.SampleDesc.Count	= 1;
	tex_desc.SampleDesc.Quality	= 0;
	tex_desc.Usage				= D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tex_desc.CPUAccessFlags		= 0;
	tex_desc.MiscFlags			= 0;
	ID3D11Texture2D *normal_depth_tex = 0;
	HR(m_D3DDevice->CreateTexture2D(&tex_desc, 0, &normal_depth_tex));
	HR(m_D3DDevice->CreateShaderResourceView(normal_depth_tex, 0, &m_NormalDepthSRV));
	HR(m_D3DDevice->CreateRenderTargetView(normal_depth_tex, 0, &m_NormalDepthRTV));
	// view saves a reference.
	ReleaseCOM(normal_depth_tex);
	// Render ambient map at half resolution.
	tex_desc.Width = m_RenderTargetWidth / 2;
	tex_desc.Height = m_RenderTargetHeight / 2;
	tex_desc.Format = DXGI_FORMAT_R16_FLOAT;
	ID3D11Texture2D *ambient_tex0 = 0;
	HR(m_D3DDevice->CreateTexture2D(&tex_desc, 0, &ambient_tex0));
	HR(m_D3DDevice->CreateShaderResourceView(ambient_tex0, 0, &m_AmbientSRV0));
	HR(m_D3DDevice->CreateRenderTargetView(ambient_tex0, 0, &m_AmbientRTV0));
	ID3D11Texture2D *ambientTex1 = 0;
	HR(m_D3DDevice->CreateTexture2D(&tex_desc, 0, &ambientTex1));
	HR(m_D3DDevice->CreateShaderResourceView(ambientTex1, 0, &m_AmbientSRV1));
	HR(m_D3DDevice->CreateRenderTargetView(ambientTex1, 0, &m_AmbientRTV1));
	// view saves a reference.
	ReleaseCOM(ambient_tex0);
	ReleaseCOM(ambientTex1);
}
//
void ssao::release_TextureViews()
{
	ReleaseCOM(m_NormalDepthRTV);
	ReleaseCOM(m_NormalDepthSRV);
	ReleaseCOM(m_AmbientRTV0);
	ReleaseCOM(m_AmbientSRV0);
	ReleaseCOM(m_AmbientRTV1);
	ReleaseCOM(m_AmbientSRV1);
}
//
void ssao::build_RandomVectorTexture()
{
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width				= 256;
	tex_desc.Height				= 256;
	tex_desc.MipLevels			= 1;
	tex_desc.ArraySize			= 1;
	tex_desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count	= 1;
	tex_desc.SampleDesc.Quality	= 0;
	tex_desc.Usage				= D3D11_USAGE_IMMUTABLE;
	tex_desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags		= 0;
	tex_desc.MiscFlags			= 0;
	D3D11_SUBRESOURCE_DATA init_data = {0};
	init_data.SysMemPitch = 256*sizeof(XMCOLOR);
	XMCOLOR color[256*256];
	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 256; ++j) {
			XMFLOAT3 v(calc_randf(), calc_randf(), calc_randf());
			color[i*256+j] = XMCOLOR(v.x, v.y, v.z, 0.0f);
		}
	}
	init_data.pSysMem = color;
	ID3D11Texture2D *tex = 0;
	HR(m_D3DDevice->CreateTexture2D(&tex_desc, &init_data, &tex));
	HR(m_D3DDevice->CreateShaderResourceView(tex, 0, &m_RandomVectorSRV));
	// view saves a reference.
	ReleaseCOM(tex);
}
//
void ssao::build_OffsetVectors()
{
	// Start with 14 uniformly distributed vectors.  We choose the 8 corners of the cube
	// and the 6 center points along each cube face.  We always alternate the points on
	// opposites sides of the cubes.  This way we still get the vectors spread out even
	// if we choose to use less than 14 samples.
	// 8 cube corners
	m_Offsets[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	m_Offsets[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);
	m_Offsets[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	m_Offsets[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);
	m_Offsets[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	m_Offsets[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);
	m_Offsets[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	m_Offsets[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);
	// 6 centers of cube faces
	m_Offsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_Offsets[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);
	m_Offsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_Offsets[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);
	m_Offsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	m_Offsets[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);
	for (int i = 0; i < 14; ++i) {
		// Create random lengths in [0.25, 1.0].
		float s = calc_randf(0.25f, 1.0f);
		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&m_Offsets[i]));
		XMStoreFloat4(&m_Offsets[i], v);
	}
}
}
#endif