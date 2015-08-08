////////////////
// mesh_geometry.h
// from 3D Game Programming with DirectX 11 by Frank Luna, MeshGeometry.h
////////////////
////////////////
#ifndef MESH_GEOMETRY_H
#define MESH_GEOMETRY_H
#include "imm_core.h"
namespace imm
{
////////////////
// mesh_geometry
////////////////
////////////////
class mesh_geometry
{
public:
	struct subset
	{
		subset():
			id(0),
			vertex_start(0),
			vertex_count(0),
			face_start(0),
			face_count(0)
		{;}
		UINT id;
		UINT vertex_start;
		UINT vertex_count;
		UINT face_start;
		UINT face_count;
	};
public:
	mesh_geometry();
	~mesh_geometry();
	template <typename vertex_type>
	void set_Vertices(ID3D11Device *device, const vertex_type *vertices, UINT count);
	void set_Indices(ID3D11Device *device, const UINT *indices, UINT count);
	void set_SubsetTable(std::vector<subset> &subset_table);
	void draw(ID3D11DeviceContext *dc, UINT subsetid);
	void draw_segment(ID3D11DeviceContext *dc, UINT subsetid);
private:
	mesh_geometry(const mesh_geometry &rhs);
	mesh_geometry &operator=(const mesh_geometry &rhs);
	ID3D11Buffer *m_VB;
	ID3D11Buffer *m_IB;
	DXGI_FORMAT m_IndexBufferFormat; // Always 32-bit
	UINT m_VertexStride;
	std::vector<subset> m_SubsetTable;
};
//
template <typename vertex_type>
void mesh_geometry::set_Vertices(ID3D11Device *device, const vertex_type *vertices, UINT count)
{
	ReleaseCOM(m_VB);
	m_VertexStride = sizeof(vertex_type);
	D3D11_BUFFER_DESC vbd;
	vbd.Usage               = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth           = sizeof(vertex_type) * count;
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &vinit_data, &m_VB));
}
//
mesh_geometry::mesh_geometry():
	m_VB(0),
	m_IB(0),
	m_IndexBufferFormat(DXGI_FORMAT_R32_UINT),
	m_VertexStride(0)
{
}
//
mesh_geometry::~mesh_geometry()
{
	ReleaseCOM(m_VB);
	ReleaseCOM(m_IB);
}
//
void mesh_geometry::set_Indices(ID3D11Device *device, const UINT *indices, UINT count)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth           = sizeof(UINT) * count;
	ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags      = 0;
	ibd.MiscFlags           = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinit_data, &m_IB));
}
//
void mesh_geometry::set_SubsetTable(std::vector<subset> &subset_table) {m_SubsetTable = subset_table;}
//
void mesh_geometry::draw(ID3D11DeviceContext *dc, UINT subsetid)
{
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &m_VB, &m_VertexStride, &offset);
	dc->IASetIndexBuffer(m_IB, m_IndexBufferFormat, 0);
	dc->DrawIndexed(
		m_SubsetTable[subsetid].face_count*3,
		m_SubsetTable[subsetid].face_start*3,
		0);
}
//
void mesh_geometry::draw_segment(ID3D11DeviceContext *dc, UINT subsetid)
{
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &m_VB, &m_VertexStride, &offset);
	dc->IASetIndexBuffer(m_IB, m_IndexBufferFormat, 0);
	dc->DrawIndexed(
		m_SubsetTable[subsetid].face_count*3,
		m_SubsetTable[subsetid].face_start*3,
		m_SubsetTable[subsetid].vertex_start);
}
}
#endif