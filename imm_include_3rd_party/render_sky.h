////////////////
// render_sky.h
// Sky.h by Frank Luna (C) 2011 All Rights Reserved.
// Simple class that renders a sky using a cube map.
////////////////
////////////////
#ifndef RENDER_SKY_H
#define RENDER_SKY_H
#include "imm_camera.h"
#include "mesh_geometry_gen.h"
#include "pipe_vertex.h"
#include "DDSTextureLoader.h"
namespace imm
{
////////////////
// sky
////////////////
////////////////
class sky
{
public:
	sky();
	~sky();
	sky(ID3D11Device* device, const std::wstring& cubemap_filename, float sky_sphere_radius);
	ID3D11ShaderResourceView* get_CubeMapSRV();
	void draw(ID3D11DeviceContext* dc, const camera& cam1);
private:
	sky (const sky& rhs);
	sky &operator=(const sky& rhs);
	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;
	ID3D11ShaderResourceView* m_CubeMapSRV;
	UINT m_ISize;
};
//
sky::sky():
	m_VB(nullptr),
	m_IB(nullptr),
	m_CubeMapSRV(nullptr),
	m_ISize(0)
{
	;
}
//
sky::~sky()
{
	RELEASE_COM(m_VB);
	RELEASE_COM(m_IB);
	RELEASE_COM(m_CubeMapSRV);
}
//
sky::sky(ID3D11Device* device, const std::wstring& cubemap_filename, float sky_sphere_radius)
{
	HR(CreateDDSTextureFromFile(device, cubemap_filename.c_str(), 0, &m_CubeMapSRV, 0));
	geometry::mesh_data sphere;
	geometry geo_gen;
	geo_gen.create_sphere(sky_sphere_radius, 30, 30, sphere);
	std::vector<XMFLOAT3> vertices(sphere.vertices.size());
	for(size_t i = 0; i < sphere.vertices.size(); ++i) vertices[i] = sphere.vertices[i].position;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage               = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth           = static_cast<UINT>(sizeof(XMFLOAT3) * vertices.size());
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinit_data, &m_VB));
	m_ISize = static_cast<UINT>(sphere.indices.size());
	D3D11_BUFFER_DESC ibd;
	ibd.Usage               = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth           = sizeof(USHORT) * m_ISize;
	ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags      = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags           = 0;
	std::vector<USHORT> indices16;
	indices16.assign(sphere.indices.begin(), sphere.indices.end());
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = &indices16[0];
	HR(device->CreateBuffer(&ibd, &iinit_data, &m_IB));
}
//
ID3D11ShaderResourceView* sky::get_CubeMapSRV()
{
	return m_CubeMapSRV;
}
//
void sky::draw(ID3D11DeviceContext* dc, const camera& cam1)
{
	if (m_VB == nullptr) return;
	// center Sky about eye in world space
	XMFLOAT3 eye_pos = cam1.get_Position();
	XMMATRIX T = XMMatrixTranslation(eye_pos.x, eye_pos.y, eye_pos.z);
	XMMATRIX WVP = XMMatrixMultiply(T, cam1.get_ViewProj());
	effects::m_SkyFX->set_WorldViewProj(WVP);
	effects::m_SkyFX->set_CubeMap(m_CubeMapSRV);
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	dc->IASetIndexBuffer(m_IB, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(input_layouts::m_Pos);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3DX11_TECHNIQUE_DESC tech_desc;
	effects::m_SkyFX->m_SkyTech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		ID3DX11EffectPass* pass = effects::m_SkyFX->m_SkyTech->GetPassByIndex(p);
		pass->Apply(0, dc);
		dc->DrawIndexed(m_ISize, 0, 0);
	}
}
//
}
#endif