////////////////
// render_wave.h
// gpu_wave.h by Frank Luna (C) 2011 All Rights Reserved.
// Performs the calculations for the wave simulation using the ComputeShader on the GPU.
// The solution is saved to a floating-point texture.  The client must then set this
// texture as a SRV and do the displacement mapping in the vertex shader over a grid.
////////////////
////////////////
#ifndef RENDER_WAVE_H
#define RENDER_WAVE_H
#include "ia_effect.h"
#include "mesh_d3d_util.h"
namespace imm
{
////////////////
// gpu_wave
////////////////
////////////////
class gpu_wave
{
public:
	gpu_wave();
	~gpu_wave();
	UINT row_count() const;
	UINT column_count() const;
	UINT vertex_count() const;
	UINT triangle_count() const;
	float width() const;
	float depth() const;
	ID3D11ShaderResourceView* get_displacement_map();
	// How many groups do we need to dispatch to cover the wave grid.
	// Note that m_NumRows and m_NumCols should be divisible by 16
	// so there is no remainder when we divide into thread groups.
	void init(ID3D11Device* device, UINT m, UINT n, float dx, float dt, float speed, float damping);
	void update(ID3D11DeviceContext* dc, float dt, float total_time);
	void disturb(ID3D11DeviceContext* dc, UINT i, UINT j, float magnitude);
private:
	void build_wave_simulation_views(ID3D11Device* device);
	UINT m_NumRows;
	UINT m_NumCols;
	UINT m_VertexCount;
	UINT m_TriangleCount;
	// Simulation constants we can precompute.
	float m_K[3];
	float m_TimeStep;
	float m_SpatialStep;
	float m_TimeBase;
	ID3D11ShaderResourceView *m_WavesPrevSolSRV;
	ID3D11ShaderResourceView *m_WavesCurrSolSRV;
	ID3D11ShaderResourceView *m_WavesNextSolSRV;
	ID3D11ShaderResourceView *m_RandomTexSRV;
	ID3D11UnorderedAccessView *m_WavesPrevSolUAV;
	ID3D11UnorderedAccessView *m_WavesCurrSolUAV;
	ID3D11UnorderedAccessView *m_WavesNextSolUAV;
	gpu_wave(const gpu_wave &rhs);
	gpu_wave &operator=(const gpu_wave &rhs);
};
//
gpu_wave::gpu_wave():
	m_NumRows(0),
	m_NumCols(0),
	m_VertexCount(0),
	m_TriangleCount(0),
	m_TimeStep(0.0f),
	m_SpatialStep(0.0f),
	m_TimeBase(0),
	m_WavesPrevSolSRV(nullptr),
	m_WavesCurrSolSRV(nullptr),
	m_WavesNextSolSRV(nullptr),
	m_RandomTexSRV(nullptr),
	m_WavesPrevSolUAV(nullptr),
	m_WavesCurrSolUAV(nullptr),
	m_WavesNextSolUAV(nullptr)
{
	m_K[0] = 0.0f;
	m_K[1] = 0.0f;
	m_K[2] = 0.0f;
}
gpu_wave::~gpu_wave()
{
	RELEASE_COM(m_WavesPrevSolSRV);
	RELEASE_COM(m_WavesCurrSolSRV);
	RELEASE_COM(m_WavesNextSolSRV);
	RELEASE_COM(m_RandomTexSRV);
	RELEASE_COM(m_WavesPrevSolUAV);
	RELEASE_COM(m_WavesCurrSolUAV);
	RELEASE_COM(m_WavesNextSolUAV);
}
//
UINT gpu_wave::row_count() const {return m_NumRows;}
UINT gpu_wave::column_count() const {return m_NumCols;}
UINT gpu_wave::vertex_count() const {return m_VertexCount;}
UINT gpu_wave::triangle_count() const {return m_TriangleCount;}
float gpu_wave::width() const {return m_NumCols*m_SpatialStep;}
float gpu_wave::depth() const {return m_NumRows*m_SpatialStep;}
ID3D11ShaderResourceView* gpu_wave::get_displacement_map()
{
	// After an Update, the current solution stores the solution we want to render.
	return m_WavesCurrSolSRV;
}
void gpu_wave::init(ID3D11Device* device, UINT m, UINT n, float dx, float dt, float speed, float damping)
{
	m_NumRows = m;
	m_NumCols = n;
	m_VertexCount   = m*n;
	m_TriangleCount = (m-1)*(n-1)*2;
	m_TimeStep    = dt;
	m_SpatialStep = dx;
	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	m_K[0]  = (damping*dt-2.0f)/ d;
	m_K[1]  = (4.0f-8.0f*e) / d;
	m_K[2]  = (2.0f*e) / d;
	build_wave_simulation_views(device);
}
void gpu_wave::update(ID3D11DeviceContext* dc, float dt, float total_time)
{
	// Accumulate time.
	m_TimeBase += dt;
	// Only update the simulation at the specified time step.
	if (m_TimeBase < m_TimeStep) return;
	D3DX11_TECHNIQUE_DESC tech_desc;
	effects::m_WaveSimFX->set_WaveConstants(m_K);
	effects::m_WaveSimFX->set_GameTime(total_time);
	effects::m_WaveSimFX->set_PrevSolInput(m_WavesPrevSolSRV);
	effects::m_WaveSimFX->set_CurrSolInput(m_WavesCurrSolSRV);
	effects::m_WaveSimFX->set_RandomTex(m_RandomTexSRV);
	effects::m_WaveSimFX->set_NextSolOutput(m_WavesNextSolUAV);
	effects::m_WaveSimFX->m_UpdateWavesTech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p) {
		ID3DX11EffectPass* pass = effects::m_WaveSimFX->m_UpdateWavesTech->GetPassByIndex(p);
		pass->Apply(0, dc);
		// How many groups do we need to dispatch to cover the wave grid.
		// Note that m_NumRows and m_NumCols should be divisible by 16
		// so there is no remainder.
		UINT numGroupsX = m_NumCols / 16;
		UINT numGroupsY = m_NumRows / 16;
		dc->Dispatch(numGroupsX, numGroupsY, 1);
	}
	// Unbind the input textures from the CS for good housekeeping.
	ID3D11ShaderResourceView* null_srv[1] = {0};
	dc->CSSetShaderResources(0, 1, null_srv);
	// Unbind output from compute shader (we are going to use this output as an input in the next pass,
	// and a resource cannot be both an output and input at the same time.
	ID3D11UnorderedAccessView* null_uav[1] = {0};
	dc->CSSetUnorderedAccessViews(0, 1, null_uav, 0);
	// Disable compute shader.
	dc->CSSetShader(0, 0, 0);
	//
	// Ping-pong buffers in preparation for the next update.
	// The previous solution is no longer needed and becomes the target of the next solution in the next update.
	// The current solution becomes the previous solution.
	// The next solution becomes the current solution.
	//
	ID3D11ShaderResourceView* srv_temp = m_WavesPrevSolSRV;
	m_WavesPrevSolSRV = m_WavesCurrSolSRV;
	m_WavesCurrSolSRV = m_WavesNextSolSRV;
	m_WavesNextSolSRV = srv_temp;
	ID3D11UnorderedAccessView* uav_temp = m_WavesPrevSolUAV;
	m_WavesPrevSolUAV = m_WavesCurrSolUAV;
	m_WavesCurrSolUAV = m_WavesNextSolUAV;
	m_WavesNextSolUAV = uav_temp;
	m_TimeBase = 0.0f; // reset time
}
void gpu_wave::disturb(ID3D11DeviceContext* dc, UINT i, UINT j, float magnitude)
{
	D3DX11_TECHNIQUE_DESC tech_desc;
	// The grid element to displace.
	effects::m_WaveSimFX->set_DisturbIndex(i, j);
	// The magnitude of the displacement.
	effects::m_WaveSimFX->set_DisturbMag(magnitude);
	// Displace the current solution heights to generate a wave.
	effects::m_WaveSimFX->set_CurrSolOutput(m_WavesCurrSolUAV);
	effects::m_WaveSimFX->m_DisturbWavesTech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p) {
		ID3DX11EffectPass* pass = effects::m_WaveSimFX->m_DisturbWavesTech->GetPassByIndex(p);
		pass->Apply(0, dc);
		// One thread group kicks off one thread, which displaces the height of one
		// vertex and its neighbors.
		dc->Dispatch(1, 1, 1);
	}
	// Unbind output from compute shader so we can use it as a shader input (a resource cannot be bound
	// as an output and input).
	ID3D11UnorderedAccessView* null_uav[1] = {0};
	dc->CSSetUnorderedAccessViews(0, 1, null_uav, 0);
}
void gpu_wave::build_wave_simulation_views(ID3D11Device* device)
{
	RELEASE_COM(m_WavesPrevSolSRV);
	RELEASE_COM(m_WavesCurrSolSRV);
	RELEASE_COM(m_WavesNextSolSRV);
	RELEASE_COM(m_RandomTexSRV);
	RELEASE_COM(m_WavesPrevSolUAV);
	RELEASE_COM(m_WavesCurrSolUAV);
	RELEASE_COM(m_WavesNextSolUAV);
	// All the textures for the wave simulation will be bound as a shader resource and
	// unordered access view at some point since we ping-pong the buffers.
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width              = m_NumCols;
	tex_desc.Height             = m_NumRows;
	tex_desc.MipLevels          = 1;
	tex_desc.ArraySize          = 1;
	tex_desc.Format             = DXGI_FORMAT_R32_FLOAT;
	tex_desc.SampleDesc.Count   = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage              = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tex_desc.CPUAccessFlags     = 0;
	tex_desc.MiscFlags          = 0;
	// Zero out the buffers initially.
	std::vector<float> zero(m_NumRows*m_NumCols, 0.0f);
	D3D11_SUBRESOURCE_DATA init_data;
	init_data.pSysMem = &zero[0];
	init_data.SysMemPitch = m_NumCols*sizeof(float);
	ID3D11Texture2D* prev_wave_solution_tex = 0;
	ID3D11Texture2D* curr_wave_solution_tex = 0;
	ID3D11Texture2D* next_wave_solution_tex = 0;
	HR(device->CreateTexture2D(&tex_desc, &init_data, &prev_wave_solution_tex));
	HR(device->CreateTexture2D(&tex_desc, &init_data, &curr_wave_solution_tex));
	HR(device->CreateTexture2D(&tex_desc, &init_data, &next_wave_solution_tex));
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
	srv_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels       = 1;
	HR(device->CreateShaderResourceView(prev_wave_solution_tex, &srv_desc, &m_WavesPrevSolSRV));
	HR(device->CreateShaderResourceView(curr_wave_solution_tex, &srv_desc, &m_WavesCurrSolSRV));
	HR(device->CreateShaderResourceView(next_wave_solution_tex, &srv_desc, &m_WavesNextSolSRV));
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	uav_desc.Format             = DXGI_FORMAT_R32_FLOAT;
	uav_desc.ViewDimension      = D3D11_UAV_DIMENSION_TEXTURE2D;
	uav_desc.Texture2D.MipSlice = 0;
	HR(device->CreateUnorderedAccessView(prev_wave_solution_tex, &uav_desc, &m_WavesPrevSolUAV));
	HR(device->CreateUnorderedAccessView(curr_wave_solution_tex, &uav_desc, &m_WavesCurrSolUAV));
	HR(device->CreateUnorderedAccessView(next_wave_solution_tex, &uav_desc, &m_WavesNextSolUAV));
	// Views save a reference to the texture so we can release our reference.
	RELEASE_COM(prev_wave_solution_tex);
	RELEASE_COM(curr_wave_solution_tex);
	RELEASE_COM(next_wave_solution_tex);
	m_RandomTexSRV = create_RandomTexture1DWaveSRV(device);
}
}
//
#endif