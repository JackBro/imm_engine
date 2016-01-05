////////////////
// render_particle.h
// ParticleSystem.h by Frank Luna (C) 2011 All Rights Reserved.
////////////////
////////////////
#ifndef RENDER_PARTICLE_H
#define RENDER_PARTICLE_H
#include "imm_camera.h"
#include "pipe_vertex.h"
namespace imm
{
////////////////
// inst_plasam
////////////////
////////////////
struct inst_plasam
{
	inst_plasam();
	void update(const float &dt);
	XMFLOAT3 pos;
	float count_down;
	bool is_first_run;
	size_t slot;
};
//
inst_plasam::inst_plasam():
	pos(),
	count_down(-100.0f),
	is_first_run(true),
	slot(0)
{
	;
}
//
void inst_plasam::update(const float &dt)
{
	count_down -= dt;
}
////////////////
// particle
////////////////
////////////////
class particle
{
public:
	particle();
	~particle();
	size_t get_ListSize() const;
	void set_EyePos(const XMFLOAT3 &eye_pos_w);
	void set_EmitPos(const XMFLOAT3 &emit_pos_w);
	void set_EmitDir(const XMFLOAT3 &emit_dir_w);
	void init(
		ID3D11Device *device,
		particle_effect *fx,
		ID3D11ShaderResourceView *tex_array_srv,
		ID3D11ShaderResourceView *random_tex_srv,
		UINT max_particles,
		const size_t &list_size);
	void update(float dt, float game_time);
	template <typename list_plasam>
	void draw_list(ID3D11DeviceContext *dc, const camera &cam1, list_plasam &list);
private:
	void build_VB(ID3D11Device *device);
	particle (const particle &rhs);
	particle &operator=(const particle &rhs);
	UINT m_MaxParticles;
	float m_GameTime;
	float m_TimeStep;
	size_t m_ListSize;
	XMFLOAT3 m_EyePosW;
	XMFLOAT3 m_EmitPosW;
	XMFLOAT3 m_EmitDirW;
	particle_effect *m_FX;
	ID3D11Buffer *m_InitVB;
	std::vector<ID3D11Buffer*> m_DrawVBList;
	std::vector<ID3D11Buffer*> m_StreamOutVBList;	
	ID3D11ShaderResourceView *m_TexArraySRV;
	ID3D11ShaderResourceView *m_RandomTexSRV;
};
//
particle::~particle()
{
	RELEASE_COM(m_InitVB);
	for (auto &vb: m_DrawVBList) RELEASE_COM(vb);
	for (auto &vb: m_StreamOutVBList) RELEASE_COM(vb);
}
//
particle::particle():
	m_MaxParticles(0),
	m_GameTime(0.0f),
	m_TimeStep(0.0f),
	m_ListSize(0),
	m_FX(nullptr),
	m_InitVB(nullptr),
	m_DrawVBList(),
	m_StreamOutVBList(),
	m_TexArraySRV(nullptr),
	m_RandomTexSRV(nullptr)
{
	m_EyePosW  = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_EmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_EmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}
//
size_t particle::get_ListSize() const {return m_ListSize;}
void particle::set_EyePos(const XMFLOAT3 &eye_pos_w) {m_EyePosW = eye_pos_w;}
void particle::set_EmitPos(const XMFLOAT3 &emit_pos_w) {m_EmitPosW = emit_pos_w;}
void particle::set_EmitDir(const XMFLOAT3 &emit_dir_w) {m_EmitDirW = emit_dir_w;}
//
void particle::init(
	ID3D11Device *device,
	particle_effect *fx,
	ID3D11ShaderResourceView *tex_array_srv,
	ID3D11ShaderResourceView *random_tex_srv,
	UINT max_particles,
	const size_t &list_size)
{
	m_MaxParticles = max_particles;
	m_FX = fx;
	m_TexArraySRV  = tex_array_srv;
	m_RandomTexSRV = random_tex_srv;
	m_ListSize     = list_size;
	build_VB(device);
}
//
void particle::update(float dt, float game_time)
{
	m_GameTime = game_time;
	m_TimeStep = dt;
}
//
template <typename list_plasam>
void particle::draw_list(ID3D11DeviceContext *dc, const camera &cam1, list_plasam &list)
{
	XMMATRIX view_proj = cam1.get_ViewProj();
	// Set constants.
	m_FX->set_ViewProj(view_proj);
	m_FX->set_GameTime(m_GameTime);
	m_FX->set_TimeStep(m_TimeStep);
	m_FX->set_EyePosW(m_EyePosW);
	m_FX->set_EmitDirW(m_EmitDirW);
	m_FX->set_TexArray(m_TexArraySRV);
	m_FX->set_RandomTex(m_RandomTexSRV);
	// Set IA stage.
	dc->IASetInputLayout(input_layouts::m_Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT stride = sizeof(vertex::particle);
	UINT offset = 0;
	for (auto &inst: list) {
		m_FX->set_EmitPosW(inst.pos);
		// On the first pass, use the initialization VB.  Otherwise, use
		// the VB that contains the current particle list.
		if (inst.is_first_run) dc->IASetVertexBuffers(0, 1, &m_InitVB, &stride, &offset);
		else dc->IASetVertexBuffers(0, 1, &m_DrawVBList[inst.slot], &stride, &offset);
		// Draw the current particle list using stream-out only to update them.
		// The updated vertices are streamed-out to the target VB.
		dc->SOSetTargets(1, &m_StreamOutVBList[inst.slot], &offset);
		D3DX11_TECHNIQUE_DESC tech_desc;
		m_FX->m_StreamOutTech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			m_FX->m_StreamOutTech->GetPassByIndex(p)->Apply(0, dc);
			if (inst.is_first_run) {
				dc->Draw(1, 0);
				inst.is_first_run = false;
			}
			else dc->DrawAuto();
		}
		// done streaming-out--unbind the vertex buffer
		ID3D11Buffer *bufferArray[1] = {0};
		dc->SOSetTargets(1, bufferArray, &offset);
		// ping-pong the vertex buffers
		std::swap(m_DrawVBList[inst.slot], m_StreamOutVBList[inst.slot]);
		// Draw the updated particle system we just streamed-out.
		dc->IASetVertexBuffers(0, 1, &m_DrawVBList[inst.slot], &stride, &offset);
		m_FX->m_DrawTech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			m_FX->m_DrawTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawAuto();
		}
	}
}
//
void particle::build_VB(ID3D11Device *device)
{
	// Create the buffer to kick-off the particle system.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage               = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth           = sizeof(vertex::particle) * 1;
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;
	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	vertex::particle p;
	ZeroMemory(&p, sizeof(vertex::particle));
	p.age = 0.0f;
	p.type = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = &p;
	HR(device->CreateBuffer(&vbd, &vinit_data, &m_InitVB));
	// Create the ping-pong buffers for stream-out and drawing.
	vbd.ByteWidth = sizeof(vertex::particle) * m_MaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	m_DrawVBList.resize(m_ListSize, nullptr);
	m_StreamOutVBList.resize(m_ListSize, nullptr);
	for (size_t ix = 0; ix != m_ListSize; ++ix) {
		HR(device->CreateBuffer(&vbd, 0, &m_DrawVBList[ix]));
		HR(device->CreateBuffer(&vbd, 0, &m_StreamOutVBList[ix]));
	}
}
}
#endif