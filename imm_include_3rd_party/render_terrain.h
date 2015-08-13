////////////////
// render_terrain.h
// Terrain.h by Frank Luna (C) 2011 All Rights Reserved.
// Class that renders a terrain using hardware tessellation and multitexturing.
////////////////
////////////////
#ifndef RENDER_TERRAIN_H
#define RENDER_TERRAIN_H
#include "mesh_d3d_util.h"
#include "ia_vertex.h"
#include "imm_camera.h"
#include "render_shadow.h"
#include "DDSTextureLoader.h"
#include <DirectXPackedVector.h>
using namespace DirectX::PackedVector;
#include <algorithm>
namespace imm
{
////////////////
// terrain
////////////////
////////////////
class terrain
{
public:
	struct init_info
	{
		std::wstring height_map_filename;
		std::wstring layer_map_filename0;
		std::wstring layer_map_filename1;
		std::wstring layer_map_filename2;
		std::wstring layer_map_filename3;
		std::wstring layer_map_filename4;
		std::wstring blend_map_filename;
		float height_scale;
		UINT heightmap_width;
		UINT heightmap_height;
		float cell_spacing;
	};
	terrain();
	~terrain();
	float get_Width() const;
	float get_Depth() const;
	float get_Height(float x, float z) const;
	XMMATRIX get_World() const;
	void set_World(CXMMATRIX M);
	void init(
		ID3D11Device *device,
		ID3D11DeviceContext *dc,
		const init_info &init_info1);
	void draw(
		ID3D11DeviceContext *dc,
		const camera &cam1,
		light_dir lights[3],
		shadow_map *shadow,
		XMMATRIX &shadow_transform);
	bool is_initialized();
private:
	void load_heightmap();
	void smooth();
	bool in_bounds(int i, int j);
	float average(int i, int j);
	void calc_AllPatchBoundsY();
	void calc_PatchBoundsY(UINT i, UINT j);
	void build_QuadPatchVB(ID3D11Device *device);
	void build_QuadPatchIB(ID3D11Device *device);
	void build_HeightmapSRV(ID3D11Device *device);
public:
	init_info m_Info;
private:
	// Divide heightmap into patches such that each patch has CellsPerPatch cells
	// and CellsPerPatch+1 vertices.  Use 64 so that if we tessellate all the way
	// to 64, we use all the data from the heightmap.
	static const int m_CellsPerPatch = 64;
	ID3D11Buffer *m_QuadPatchVB;
	ID3D11Buffer *m_QuadPatchIB;
	ID3D11ShaderResourceView *m_LayerMapArraySRV;
	ID3D11ShaderResourceView *m_BlendMapSRV;
	ID3D11ShaderResourceView *m_HeightMapSRV;
	UINT m_NumPatchVertices;
	UINT m_NumPatchQuadFaces;
	UINT m_NumPatchVertRows;
	UINT m_NumPatchVertCols;
	XMFLOAT4X4 m_World;
	material m_Mat;
	std::vector<XMFLOAT2> m_PatchBoundsY;
	std::vector<float> m_Heightmap;
};
//
terrain::~terrain()
{
	RELEASE_COM(m_QuadPatchVB);
	RELEASE_COM(m_QuadPatchIB);
	RELEASE_COM(m_LayerMapArraySRV);
	RELEASE_COM(m_BlendMapSRV);
	RELEASE_COM(m_HeightMapSRV);
}
//
terrain::terrain():
	m_QuadPatchVB(nullptr),
	m_QuadPatchIB(nullptr),
	m_LayerMapArraySRV(nullptr),
	m_BlendMapSRV(nullptr),
	m_HeightMapSRV(nullptr),
	m_NumPatchVertices(0),
	m_NumPatchQuadFaces(0),
	m_NumPatchVertRows(0),
	m_NumPatchVertCols(0)
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
	m_Mat.ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	m_Mat.reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}
//
float terrain::get_Width() const {return (m_Info.heightmap_width-1)*m_Info.cell_spacing;}
float terrain::get_Depth() const {return (m_Info.heightmap_height-1)*m_Info.cell_spacing;}
XMMATRIX terrain::get_World() const {return XMLoadFloat4x4(&m_World);}
void terrain::set_World(CXMMATRIX M) {XMStoreFloat4x4(&m_World, M);}
//
float terrain::get_Height(float x, float z) const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*get_Width()) /  m_Info.cell_spacing;
	float d = (z - 0.5f*get_Depth()) / -m_Info.cell_spacing;
	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);
	// If out of range
	if (row < 0 ||
		col < 0 ||
		row > (int)m_Info.heightmap_height-2 ||
		col > (int)m_Info.heightmap_width-2) {
		return -FLT_MAX;
	}
	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = m_Heightmap[row*m_Info.heightmap_width + col];
	float B = m_Heightmap[row*m_Info.heightmap_width + col + 1];
	float C = m_Heightmap[(row+1)*m_Info.heightmap_width + col];
	float D = m_Heightmap[(row+1)*m_Info.heightmap_width + col + 1];
	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;
	// If upper triangle ABC.
	if (s + t <= 1.0f) {
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	// lower triangle DCB.
	else  {
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}
//
void terrain::init(
	ID3D11Device *device,
	ID3D11DeviceContext *dc,
	const init_info &init_info1)
{
	m_Info = init_info1;
	// Divide heightmap into patches such that each patch has CellsPerPatch.
	m_NumPatchVertRows = ((m_Info.heightmap_height-1) / m_CellsPerPatch) + 1;
	m_NumPatchVertCols = ((m_Info.heightmap_width-1) / m_CellsPerPatch) + 1;
	m_NumPatchVertices  = m_NumPatchVertRows*m_NumPatchVertCols;
	m_NumPatchQuadFaces = (m_NumPatchVertRows-1)*(m_NumPatchVertCols-1);
	load_heightmap();
	smooth();
	calc_AllPatchBoundsY();
	build_QuadPatchVB(device);
	build_QuadPatchIB(device);
	build_HeightmapSRV(device);
	std::vector<std::wstring> layer_filenames;
	layer_filenames.push_back(m_Info.layer_map_filename0);
	layer_filenames.push_back(m_Info.layer_map_filename1);
	layer_filenames.push_back(m_Info.layer_map_filename2);
	layer_filenames.push_back(m_Info.layer_map_filename3);
	layer_filenames.push_back(m_Info.layer_map_filename4);
	m_LayerMapArraySRV = create_Texture2DArraySRV(device, dc, layer_filenames);
	HR(CreateDDSTextureFromFile(device, m_Info.blend_map_filename.c_str(), 0, &m_BlendMapSRV, 0));
}
//
void terrain::draw(
	ID3D11DeviceContext *dc,
	const camera &cam1,
	light_dir lights[3],
	shadow_map *shadow,
	XMMATRIX &shadow_transform)
{
	if (m_QuadPatchVB == nullptr) return;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	dc->IASetInputLayout(input_layouts::m_Terrain);
	UINT stride = sizeof(vertex::terrain);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &m_QuadPatchVB, &stride, &offset);
	dc->IASetIndexBuffer(m_QuadPatchIB, DXGI_FORMAT_R16_UINT, 0);
	XMMATRIX view_proj = cam1.get_ViewProj();
	XMMATRIX world  = XMLoadFloat4x4(&m_World);
	XMFLOAT4 world_planes[6];
	extract_frustum_planes(world_planes, view_proj);
	// Set per frame constants.
	effects::m_TerrainFX->set_ViewProj(view_proj);
	effects::m_TerrainFX->set_ShadowTransform(world*shadow_transform);
	effects::m_TerrainFX->set_EyePosW(cam1.get_Position());
	effects::m_TerrainFX->set_DirLights(lights);
	effects::m_TerrainFX->set_MinDist(20.0f);
	effects::m_TerrainFX->set_MaxDist(500.0f);
	effects::m_TerrainFX->set_MinTess(0.0f);
	effects::m_TerrainFX->set_MaxTess(6.0f);
	effects::m_TerrainFX->set_TexelCellSpaceU(1.0f / m_Info.heightmap_width);
	effects::m_TerrainFX->set_TexelCellSpaceV(1.0f / m_Info.heightmap_height);
	effects::m_TerrainFX->set_WorldCellSpace(m_Info.cell_spacing);
	effects::m_TerrainFX->set_WorldFrustumPlanes(world_planes);
	effects::m_TerrainFX->set_LayerMapArray(m_LayerMapArraySRV);
	effects::m_TerrainFX->set_BlendMap(m_BlendMapSRV);
	effects::m_TerrainFX->set_HeightMap(m_HeightMapSRV);
	effects::m_TerrainFX->set_ShadowMap(shadow->get_DepthMapSRV());
	effects::m_TerrainFX->set_Material(m_Mat);
	ID3DX11EffectTechnique *tech = effects::m_TerrainFX->m_Light3Tech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for(UINT i = 0; i < tech_desc.Passes; ++i) {
		ID3DX11EffectPass *pass = tech->GetPassByIndex(i);
		pass->Apply(0, dc);
		dc->DrawIndexed(m_NumPatchQuadFaces*4, 0, 0);
	}
	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);
}
//
bool terrain::is_initialized()
{
	return (m_QuadPatchVB != nullptr);
}
//
void terrain::load_heightmap()
{
	// A height for each vertex
	std::vector<unsigned char> in(m_Info.heightmap_width * m_Info.heightmap_height);
	// Open the file.
	std::ifstream in_file;
	in_file.open(m_Info.height_map_filename.c_str(), std::ios_base::binary);
	if (in_file) {
		// Read the RAW bytes.
		in_file.read((char*)&in[0], (std::streamsize)in.size());
		// Done with file.
		in_file.close();
	}
	// Copy the array data into a float array and scale it.
	m_Heightmap.resize(m_Info.heightmap_height * m_Info.heightmap_width, 0);
	for(UINT i = 0; i < m_Info.heightmap_height * m_Info.heightmap_width; ++i) {
		m_Heightmap[i] = (in[i] / 255.0f)*m_Info.height_scale;
	}
}
//
void terrain::smooth()
{
	std::vector<float> dest(m_Heightmap.size());
	for(UINT i = 0; i < m_Info.heightmap_height; ++i) {
		for(UINT j = 0; j < m_Info.heightmap_width; ++j) dest[i*m_Info.heightmap_width+j] = average(i,j);
	}
	// Replace the old heightmap with the filtered one.
	m_Heightmap = dest;
}
//
bool terrain::in_bounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < (int)m_Info.heightmap_height &&
		j >= 0 && j < (int)m_Info.heightmap_width;
}
//
float terrain::average(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------
	float avg = 0.0f;
	float num = 0.0f;
	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for(int m = i-1; m <= i+1; ++m) {
		for(int n = j-1; n <= j+1; ++n) {
			if (in_bounds(m,n)) {
				avg += m_Heightmap[m*m_Info.heightmap_width + n];
				num += 1.0f;
			}
		}
	}
	return avg / num;
}
//
void terrain::calc_AllPatchBoundsY()
{
	m_PatchBoundsY.resize(m_NumPatchQuadFaces);
	// For each patch
	for(UINT i = 0; i < m_NumPatchVertRows-1; ++i) {
		for(UINT j = 0; j < m_NumPatchVertCols-1; ++j) calc_PatchBoundsY(i, j);
	}
}
//
void terrain::calc_PatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.
	UINT x0 = j*m_CellsPerPatch;
	UINT x1 = (j+1)*m_CellsPerPatch;
	UINT y0 = i*m_CellsPerPatch;
	UINT y1 = (i+1)*m_CellsPerPatch;
	float min_y = +FLT_MAX;
	float max_y = -FLT_MAX;
	for(UINT y = y0; y <= y1; ++y) {
		for(UINT x = x0; x <= x1; ++x) {
			UINT k = y*m_Info.heightmap_width + x;
			min_y = math::calc_min(min_y, m_Heightmap[k]);
			max_y = math::calc_max(max_y, m_Heightmap[k]);
		}
	}
	UINT patch_id = i*(m_NumPatchVertCols-1)+j;
	m_PatchBoundsY[patch_id] = XMFLOAT2(min_y, max_y);
}
//
void terrain::build_QuadPatchVB(ID3D11Device *device)
{
	std::vector<vertex::terrain> patch_vertices(m_NumPatchVertRows*m_NumPatchVertCols);
	float half_width = 0.5f*get_Width();
	float half_depth = 0.5f*get_Depth();
	float patch_width = get_Width() / (m_NumPatchVertCols-1);
	float patch_depth = get_Depth() / (m_NumPatchVertRows-1);
	float du = 1.0f / (m_NumPatchVertCols-1);
	float dv = 1.0f / (m_NumPatchVertRows-1);
	for(UINT i = 0; i < m_NumPatchVertRows; ++i) {
		float z = half_depth - i*patch_depth;
		for(UINT j = 0; j < m_NumPatchVertCols; ++j) {
			float x = -half_width + j*patch_width;
			patch_vertices[i*m_NumPatchVertCols+j].pos = XMFLOAT3(x, 0.0f, z);
			// Stretch texture over grid.
			patch_vertices[i*m_NumPatchVertCols+j].tex.x = j*du;
			patch_vertices[i*m_NumPatchVertCols+j].tex.y = i*dv;
		}
	}
	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for(UINT i = 0; i < m_NumPatchVertRows-1; ++i) {
		for(UINT j = 0; j < m_NumPatchVertCols-1; ++j) {
			UINT patch_id = i*(m_NumPatchVertCols-1)+j;
			patch_vertices[i*m_NumPatchVertCols+j].bounds_y = m_PatchBoundsY[patch_id];
		}
	}
	D3D11_BUFFER_DESC vbd;
	vbd.Usage               = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth           = static_cast<UINT>(sizeof(vertex::terrain) * patch_vertices.size());
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = &patch_vertices[0];
	HR(device->CreateBuffer(&vbd, &vinit_data, &m_QuadPatchVB));
}
//
void terrain::build_QuadPatchIB(ID3D11Device *device)
{
	std::vector<USHORT> indices(m_NumPatchQuadFaces*4); // 4 indices per quad face
	// Iterate over each quad and compute indices.
	int k = 0;
	for(UINT i = 0; i < m_NumPatchVertRows-1; ++i) {
		for(UINT j = 0; j < m_NumPatchVertCols-1; ++j) {
			// Top row of 2x2 quad patch
			indices[k]   = static_cast<USHORT>(i*m_NumPatchVertCols+j);
			indices[k+1] = static_cast<USHORT>(i*m_NumPatchVertCols+j+1);
			// Bottom row of 2x2 quad patch
			indices[k+2] = static_cast<USHORT>((i+1)*m_NumPatchVertCols+j);
			indices[k+3] = static_cast<USHORT>((i+1)*m_NumPatchVertCols+j+1);
			k += 4; // next quad
		}
	}
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth           = static_cast<UINT>(sizeof(USHORT) * indices.size());
	ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags      = 0;
	ibd.MiscFlags           = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinit_data, &m_QuadPatchIB));
}
//
void terrain::build_HeightmapSRV(ID3D11Device *device)
{
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width              = m_Info.heightmap_width;
	tex_desc.Height             = m_Info.heightmap_height;
	tex_desc.MipLevels          = 1;
	tex_desc.ArraySize          = 1;
	tex_desc.Format             = DXGI_FORMAT_R16_FLOAT;
	tex_desc.SampleDesc.Count   = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage              = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags     = 0;
	tex_desc.MiscFlags          = 0;
	// HALF is defined in xnamath.h, for storing 16-bit float.
	std::vector<HALF> hmap(m_Heightmap.size());
	std::transform(m_Heightmap.begin(), m_Heightmap.end(), hmap.begin(), XMConvertFloatToHalf);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem            = &hmap[0];
	data.SysMemPitch        = m_Info.heightmap_width*sizeof(HALF);
	data.SysMemSlicePitch   = 0;
	ID3D11Texture2D *hmap_tex = 0;
	HR(device->CreateTexture2D(&tex_desc, &data, &hmap_tex));
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format                     = tex_desc.Format;
	srv_desc.ViewDimension              = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip  = 0;
	srv_desc.Texture2D.MipLevels        = static_cast<UINT>(-1);
	HR(device->CreateShaderResourceView(hmap_tex, &srv_desc, &m_HeightMapSRV));
	// SRV saves reference.
	RELEASE_COM(hmap_tex);
}
//
}
#endif