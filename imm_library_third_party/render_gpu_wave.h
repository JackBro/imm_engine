////////////////
// render_gpu_wave.h
// gpu_wave.h by Frank Luna (C) 2011 All Rights Reserved.
// Performs the calculations for the wave simulation using the ComputeShader on the GPU.
// The solution is saved to a floating-point texture.  The client must then set this
// texture as a SRV and do the displacement mapping in the vertex shader over a grid.
////////////////
////////////////
#ifndef RENDER_GPU_WAVE_H
#define RENDER_GPU_WAVE_H
//#include <algorithm>
#include "ia_effect.h"
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
	UINT get_RowCount() const;
	UINT get_ColumnCount() const;
	UINT get_VertexCount() const;
	UINT get_TriangleCount() const;
	float get_Width() const;
	float get_Depth() const;
	ID3D11ShaderResourceView* get_DisplacementMap();
	// How many groups do we need to dispatch to cover the wave grid.
	// Note that m_NumRows and m_NumCols should be divisible by 16
	// so there is no remainder when we divide into thread groups.
	void init(ID3D11Device* device, UINT m, UINT n, float dx, float dt, float speed, float damping);
	void update(ID3D11DeviceContext* dc, float dt);
	void disturb(ID3D11DeviceContext* dc, UINT i, UINT j, float magnitude);
private:
	void build_wave_simulation_views(ID3D11Device* device);
private:
	UINT m_NumRows;
	UINT m_NumCols;
	UINT m_VertexCount;
	UINT m_TriangleCount;
	// Simulation constants we can precompute.
	float m_K[3];
	float m_TimeStep;
	float m_SpatialStep;
	ID3D11ShaderResourceView *m_WavesPrevSolSRV;
	ID3D11ShaderResourceView *m_WavesCurrSolSRV;
	ID3D11ShaderResourceView *m_WavesNextSolSRV;
	ID3D11UnorderedAccessView *m_WavesPrevSolUAV;
	ID3D11UnorderedAccessView *m_WavesCurrSolUAV;
	ID3D11UnorderedAccessView *m_WavesNextSolUAV;
};
//
gpu_wave::gpu_wave():
	m_NumRows(0),
	m_NumCols(0),
	m_VertexCount(0),
	m_TriangleCount(0),
	m_TimeStep(0.0f),
	m_SpatialStep(0.0f),
	m_WavesPrevSolSRV(nullptr),
	m_WavesCurrSolSRV(nullptr),
	m_WavesNextSolSRV(nullptr),
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
	ReleaseCOM(m_WavesPrevSolSRV);
	ReleaseCOM(m_WavesCurrSolSRV);
	ReleaseCOM(m_WavesNextSolSRV);
	ReleaseCOM(m_WavesPrevSolUAV);
	ReleaseCOM(m_WavesCurrSolUAV);
	ReleaseCOM(m_WavesNextSolUAV);
}
UINT gpu_wave::get_RowCount() const {return m_NumRows;}
UINT gpu_wave::get_ColumnCount() const {return m_NumCols;}
UINT gpu_wave::get_VertexCount() const {return m_VertexCount;}
UINT gpu_wave::get_TriangleCount() const {return m_TriangleCount;}
float gpu_wave::get_Width() const {return m_NumCols*m_SpatialStep;}
float gpu_wave::get_Depth() const {return m_NumRows*m_SpatialStep;}
ID3D11ShaderResourceView* gpu_wave::get_DisplacementMap()
{
	// After an Update, the current solution stores the solution we want to render.
	return m_WavesCurrSolSRV;
}
void gpu_wave::init(ID3D11Device* device, UINT m, UINT n, float dx, float dt, float speed, float damping)
{
	m_NumRows  = m;
	m_NumCols  = n;
	m_VertexCount   = m*n;
	m_TriangleCount = (m-1)*(n-1)*2;
	m_TimeStep    = dt;
	m_SpatialStep = dx;
	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	m_K[0]   = (damping*dt-2.0f)/ d;
	m_K[1]   = (4.0f-8.0f*e) / d;
	m_K[2]   = (2.0f*e) / d;
	build_wave_simulation_views(device);
}
void gpu_wave::update(ID3D11DeviceContext* dc, float dt)
{
	static float t = 0;
	// Accumulate time.
	t += dt;
	// Only update the simulation at the specified time step.
	if (t >= m_TimeStep) {
		D3DX11_TECHNIQUE_DESC techDesc;
		effects::m_WaveSimFX->SetWaveConstants(m_K);
		effects::m_WaveSimFX->SetPrevSolInput(m_WavesPrevSolSRV);
		effects::m_WaveSimFX->SetCurrSolInput(m_WavesCurrSolSRV);
		effects::m_WaveSimFX->SetNextSolOutput(m_WavesNextSolUAV);
		effects::m_WaveSimFX->m_UpdateWavesTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p) {
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
		ID3D11ShaderResourceView* nullSRV[1] = { 0 };
		dc->CSSetShaderResources(0, 1, nullSRV);
		// Unbind output from compute shader (we are going to use this output as an input in the next pass,
		// and a resource cannot be both an output and input at the same time.
		ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
		dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
		// Disable compute shader.
		dc->CSSetShader(0, 0, 0);
		//
		// Ping-pong buffers in preparation for the next update.
		// The previous solution is no longer needed and becomes the target of the next solution in the next update.
		// The current solution becomes the previous solution.
		// The next solution becomes the current solution.
		//
		ID3D11ShaderResourceView* srvTemp = m_WavesPrevSolSRV;
		m_WavesPrevSolSRV = m_WavesCurrSolSRV;
		m_WavesCurrSolSRV = m_WavesNextSolSRV;
		m_WavesNextSolSRV = srvTemp;
		ID3D11UnorderedAccessView* uavTemp = m_WavesPrevSolUAV;
		m_WavesPrevSolUAV = m_WavesCurrSolUAV;
		m_WavesCurrSolUAV = m_WavesNextSolUAV;
		m_WavesNextSolUAV = uavTemp;
		t = 0.0f; // reset time
	}
}
void gpu_wave::disturb(ID3D11DeviceContext* dc, UINT i, UINT j, float magnitude)
{
	D3DX11_TECHNIQUE_DESC techDesc;
	// The grid element to displace.
	effects::m_WaveSimFX->SetDisturbIndex(i, j);
	// The magnitude of the displacement.
	effects::m_WaveSimFX->SetDisturbMag(magnitude);
	// Displace the current solution heights to generate a wave.
	effects::m_WaveSimFX->SetCurrSolOutput(m_WavesCurrSolUAV);
	effects::m_WaveSimFX->m_DisturbWavesTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		ID3DX11EffectPass* pass = effects::m_WaveSimFX->m_DisturbWavesTech->GetPassByIndex(p);
		pass->Apply(0, dc);
		// One thread group kicks off one thread, which displaces the height of one
		// vertex and its neighbors.
		dc->Dispatch(1, 1, 1);
	}
	// Unbind output from compute shader so we can use it as a shader input (a resource cannot be bound
	// as an output and input).
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
}
void gpu_wave::build_wave_simulation_views(ID3D11Device* device)
{
	ReleaseCOM(m_WavesPrevSolSRV);
	ReleaseCOM(m_WavesCurrSolSRV);
	ReleaseCOM(m_WavesNextSolSRV);
	ReleaseCOM(m_WavesPrevSolUAV);
	ReleaseCOM(m_WavesCurrSolUAV);
	ReleaseCOM(m_WavesNextSolUAV);
	// All the textures for the wave simulation will be bound as a shader resource and
	// unordered access view at some point since we ping-pong the buffers.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width     = m_NumCols;
	texDesc.Height    = m_NumRows;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R32_FLOAT;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage     = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags      = 0;
	// Zero out the buffers initially.
	std::vector<float> zero(m_NumRows*m_NumCols, 0.0f);
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &zero[0];
	initData.SysMemPitch = m_NumCols*sizeof(float);
	ID3D11Texture2D* prevWaveSolutionTex = 0;
	ID3D11Texture2D* currWaveSolutionTex = 0;
	ID3D11Texture2D* nextWaveSolutionTex = 0;
	HR(device->CreateTexture2D(&texDesc, &initData, &prevWaveSolutionTex));
	HR(device->CreateTexture2D(&texDesc, &initData, &currWaveSolutionTex));
	HR(device->CreateTexture2D(&texDesc, &initData, &nextWaveSolutionTex));
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	HR(device->CreateShaderResourceView(prevWaveSolutionTex, &srvDesc, &m_WavesPrevSolSRV));
	HR(device->CreateShaderResourceView(currWaveSolutionTex, &srvDesc, &m_WavesCurrSolSRV));
	HR(device->CreateShaderResourceView(nextWaveSolutionTex, &srvDesc, &m_WavesNextSolSRV));
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	HR(device->CreateUnorderedAccessView(prevWaveSolutionTex, &uavDesc, &m_WavesPrevSolUAV));
	HR(device->CreateUnorderedAccessView(currWaveSolutionTex, &uavDesc, &m_WavesCurrSolUAV));
	HR(device->CreateUnorderedAccessView(nextWaveSolutionTex, &uavDesc, &m_WavesNextSolUAV));
	// Views save a reference to the texture so we can release our reference.
	ReleaseCOM(prevWaveSolutionTex);
	ReleaseCOM(currWaveSolutionTex);
	ReleaseCOM(nextWaveSolutionTex);
}
}
//
#endif