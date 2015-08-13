////////////////
// ia_vertex.h
// Vertex.h by Frank Luna (C) 2011 All Rights Reserved.
// Defines vertex structures and input layouts.
////////////////
////////////////
#ifndef IA_VERTEX_H
#define IA_VERTEX_H
#include "ia_effect.h"
namespace imm
{
////////////////
// Basic 32-byte vertex structure.
////////////////
////////////////
namespace vertex
{
struct basic32
{
	vertex::basic32():
		pos(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), tex(0.0f, 0.0f) {;}
	vertex::basic32(const XMFLOAT3 &p, const XMFLOAT3 &n, const XMFLOAT2 &uv):
		pos(p), normal(n), tex(uv) {;}
	vertex::basic32(float px, float py, float pz, float nx, float ny, float nz, float u, float v):
		pos(px, py, pz), normal(nx, ny, nz), tex(u,v) {;}
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};
//
struct pntt
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT3 tangent_u;
};
//
struct pntt2
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT4 tangent_u;
};
//
struct terrain
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT2 bounds_y;
};
//
struct particle
{
	XMFLOAT3 init_pos;
	XMFLOAT3 init_vel;
	XMFLOAT2 size;
	float age;
	unsigned int type;
};
//
struct pntt_skinned
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT4 tangent_u;
	XMFLOAT3 weights;
	BYTE bone_indices[4];
};
//
struct pos_color
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};
} // namespace vertex
////////////////
// input_layout_desc
////////////////
////////////////
class input_layout_desc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC m_Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC m_Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC m_PNTT[4];
	static const D3D11_INPUT_ELEMENT_DESC m_PNTT2[4];
	static const D3D11_INPUT_ELEMENT_DESC m_PNTTSkinned[6];
	static const D3D11_INPUT_ELEMENT_DESC m_Terrain[3];
	static const D3D11_INPUT_ELEMENT_DESC m_Particle[5];
	static const D3D11_INPUT_ELEMENT_DESC m_PosColor[2];
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_Pos[1] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_Basic32[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_PNTT[4] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_PNTT2[4] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_PNTTSkinned[6] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_Terrain[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_Particle[5] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC input_layout_desc::m_PosColor[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
////////////////
// input_layouts
////////////////
////////////////
class input_layouts
{
public:
	static void init_all(ID3D11Device *device);
	static void destroy_all();
	static ID3D11InputLayout *m_Pos;
	static ID3D11InputLayout *m_Basic32;
	static ID3D11InputLayout *m_PNTT;
	static ID3D11InputLayout *m_PNTT2;
	static ID3D11InputLayout *m_PNTTSkinned;
	static ID3D11InputLayout *m_Terrain;
	static ID3D11InputLayout *m_Particle;
	static ID3D11InputLayout *m_PosColor;
};
//
ID3D11InputLayout *input_layouts::m_Pos = 0;
ID3D11InputLayout *input_layouts::m_Basic32 = 0;
ID3D11InputLayout *input_layouts::m_PNTT = 0;
ID3D11InputLayout *input_layouts::m_PNTT2 = 0;
ID3D11InputLayout *input_layouts::m_PNTTSkinned = 0;
ID3D11InputLayout *input_layouts::m_Terrain = 0;
ID3D11InputLayout *input_layouts::m_Particle = 0;
ID3D11InputLayout *input_layouts::m_PosColor = 0;
//
void input_layouts::destroy_all()
{
	RELEASE_COM(m_Pos);
	RELEASE_COM(m_Basic32);
	RELEASE_COM(m_PNTT);
	RELEASE_COM(m_PNTT2);
	RELEASE_COM(m_PNTTSkinned);
	RELEASE_COM(m_Terrain);
	RELEASE_COM(m_Particle);
}
//
void input_layouts::init_all(ID3D11Device *device)
{
	D3DX11_PASS_DESC pass_desc;
	// m_Pos
	effects::m_SkyFX->m_SkyTech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_Pos,
		1,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_Pos));
	// m_Basic32
	effects::m_BasicFX->m_Light3Tech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_Basic32,
		3,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_Basic32));
	// m_PNTT
	effects::m_NormalMapFX->m_Light3Tech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_PNTT,
		4,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_PNTT));
	// m_PNTT2
	effects::m_NormalMapFX->m_Light3Tech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_PNTT2,
		4,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_PNTT2));
	// m_PNTTSkinned
	effects::m_NormalMapFX->m_Light3TexSkinnedTech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_PNTTSkinned,
		6,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_PNTTSkinned));
	// m_Terrain
	effects::m_TerrainFX->m_Light3Tech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_Terrain,
		3,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_Terrain));
	// m_Particle
	effects::m_FireFX->m_StreamOutTech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_Particle,
		5,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_Particle));
	// m_PosColor
	effects::m_PosColorFX->m_PosColorTech->GetPassByIndex(0)->GetDesc(&pass_desc);
	HR(device->CreateInputLayout(
		input_layout_desc::m_PosColor,
		2,
		pass_desc.pIAInputSignature,
		pass_desc.IAInputSignatureSize,
		&m_PosColor));
}
}
#endif