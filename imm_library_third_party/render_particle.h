////////////////
// render_particle.h
// ParticleSystem.h by Frank Luna (C) 2011 All Rights Reserved.
////////////////
////////////////
#ifndef RENDER_PARTICLE_H
#define RENDER_PARTICLE_H
#include "imm_camera.h"
#include "ia_vertex.h"
namespace imm
{
////////////////
// particle
////////////////
////////////////
class particle
{
public:
	particle();
	~particle();
	// Time elapsed since the system was reset.
	float get_Age()const;
	void set_EyePos(const XMFLOAT3 &eye_pos_w);
	void set_EmitPos(const XMFLOAT3 &emit_pos_w);
	void set_EmitDir(const XMFLOAT3 &emit_dir_w);
	void init(ID3D11Device *device, particle_effect *fx, ID3D11ShaderResourceView *tex_array_srv,
		ID3D11ShaderResourceView *random_tex_srv, UINT max_particles);
	void reset();
	void update(float dt, float game_time);
	void draw(ID3D11DeviceContext *dc, const camera &cam1);
private:
	void build_VB(ID3D11Device *device);
	particle (const particle &rhs);
	particle &operator=(const particle &rhs);
	UINT m_MaxParticles;
	bool m_FirstRun;
	float m_GameTime;
	float m_TimeStep;
	float m_Age;
	XMFLOAT3 m_EyePosW;
	XMFLOAT3 m_EmitPosW;
	XMFLOAT3 m_EmitDirW;
	particle_effect *m_FX;
	ID3D11Buffer *m_InitVB;
	ID3D11Buffer *m_DrawVB;
	ID3D11Buffer *m_StreamOutVB;
	ID3D11ShaderResourceView *m_TexArraySRV;
	ID3D11ShaderResourceView *m_RandomTexSRV;
};
//
particle::~particle()
{
	ReleaseCOM(m_InitVB);
	ReleaseCOM(m_DrawVB);
	ReleaseCOM(m_StreamOutVB);
}
//
particle::particle():
	m_InitVB(0),
	m_DrawVB(0),
	m_StreamOutVB(0),
	m_TexArraySRV(0),
	m_RandomTexSRV(0)
{
	m_FirstRun = true;
	m_GameTime = 0.0f;
	m_TimeStep = 0.0f;
	m_Age	  = 0.0f;
	m_EyePosW  = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_EmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_EmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}
//
float particle::get_Age() const {return m_Age;}
void particle::set_EyePos(const XMFLOAT3 &eye_pos_w) {m_EyePosW = eye_pos_w;}
void particle::set_EmitPos(const XMFLOAT3 &emit_pos_w) {m_EmitPosW = emit_pos_w;}
void particle::set_EmitDir(const XMFLOAT3 &emit_dir_w) {m_EmitDirW = emit_dir_w;}
//
void particle::init(ID3D11Device *device, particle_effect *fx, ID3D11ShaderResourceView *tex_array_srv,
	ID3D11ShaderResourceView *random_tex_srv, UINT max_particles)
{
	m_MaxParticles = max_particles;
	m_FX = fx;
	m_TexArraySRV  = tex_array_srv;
	m_RandomTexSRV = random_tex_srv;
	build_VB(device);
}
//
void particle::reset()
{
	m_FirstRun = true;
	m_Age = 0.0f;
}
//
void particle::update(float dt, float game_time)
{
	m_GameTime = game_time;
	m_TimeStep = dt;
	m_Age += dt;
}
//
void particle::draw(ID3D11DeviceContext *dc, const camera &cam1)
{
	XMMATRIX view_proj = cam1.get_ViewProj();
	// Set constants.
	m_FX->set_ViewProj(view_proj);
	m_FX->set_GameTime(m_GameTime);
	m_FX->set_TimeStep(m_TimeStep);
	m_FX->set_EyePosW(m_EyePosW);
	m_FX->set_EmitPosW(m_EmitPosW);
	m_FX->set_EmitDirW(m_EmitDirW);
	m_FX->set_TexArray(m_TexArraySRV);
	m_FX->set_RandomTex(m_RandomTexSRV);
	// Set IA stage.
	dc->IASetInputLayout(input_layouts::m_Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT stride = sizeof(stru_particle);
	UINT offset = 0;
	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if (m_FirstRun) dc->IASetVertexBuffers(0, 1, &m_InitVB, &stride, &offset);
	else dc->IASetVertexBuffers(0, 1, &m_DrawVB, &stride, &offset);
	// Draw the current particle list using stream-out only to update them.
	// The updated vertices are streamed-out to the target VB.
	dc->SOSetTargets(1, &m_StreamOutVB, &offset);
	D3DX11_TECHNIQUE_DESC tech_desc;
	m_FX->m_StreamOutTech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		m_FX->m_StreamOutTech->GetPassByIndex(p)->Apply(0, dc);
		if (m_FirstRun) {
			dc->Draw(1, 0);
			m_FirstRun = false;
		}
		else dc->DrawAuto();
	}
	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer *bufferArray[1] = {0};
	dc->SOSetTargets(1, bufferArray, &offset);
	// ping-pong the vertex buffers
	std::swap(m_DrawVB, m_StreamOutVB);
	// Draw the updated particle system we just streamed-out.
	dc->IASetVertexBuffers(0, 1, &m_DrawVB, &stride, &offset);
	m_FX->m_DrawTech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		m_FX->m_DrawTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawAuto();
	}
}
//
void particle::build_VB(ID3D11Device *device)
{
	// Create the buffer to kick-off the particle system.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage				= D3D11_USAGE_DEFAULT;
	vbd.ByteWidth			= sizeof(stru_particle) * 1;
	vbd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags		= 0;
	vbd.MiscFlags			= 0;
	vbd.StructureByteStride	= 0;
	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	stru_particle p;
	ZeroMemory(&p, sizeof(stru_particle));
	p.age = 0.0f;
	p.type = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = &p;
	HR(device->CreateBuffer(&vbd, &vinit_data, &m_InitVB));
	// Create the ping-pong buffers for stream-out and drawing.
	vbd.ByteWidth = sizeof(stru_particle) * m_MaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	HR(device->CreateBuffer(&vbd, 0, &m_DrawVB));
	HR(device->CreateBuffer(&vbd, 0, &m_StreamOutVB));
}
}
#endif