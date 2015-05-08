////////////////
// mesh_basic_model.h
// from 3D Game Programming with DirectX 11 by Frank Luna, BasicModel.h, SkinnedModel.h
////////////////
////////////////
#ifndef MESH_BASIC_MODEL_H
#define MESH_BASIC_MODEL_H
#include "mesh_load_m3d.h"
#include "mesh_texture_mgr.h"
#include "mesh_skinned_data.h"
namespace imm
{
////////////////
// basic_model
////////////////
////////////////
class basic_model
{
public:
	basic_model() {;}
	void set(
		ID3D11Device *device, texture_mgr &tex_mgr, const std::string &model_filename,
		const std::wstring &texture_path);
	void set(
		ID3D11Device *device, texture_mgr &tex_mgr, const std::wstring &texture_path);
	virtual ~basic_model() {;}
	std::pair <size_t, size_t> get_VertexRange(UINT sub_id);
	size_t m_SubsetCount;
	std::vector<material> m_Mat;
	std::vector<std::string> m_Tex;
	std::vector<ID3D11ShaderResourceView*> m_DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> m_NormalMapSRV;
	// Keep CPU copies of the mesh data to read from.
	std::vector<pos_normal_tex_tan2> m_Vertices;
	std::vector<UINT> m_Indices;
	std::vector<mesh_geometry::subset> m_Subsets;
	mesh_geometry m_ModelMesh;
};
////////////////
// basic_model_instance
////////////////
////////////////
struct basic_model_instance
{
	basic_model_instance():
		model(nullptr),
		is_appear(true) {
			XMStoreFloat4x4(&world, XMMatrixIdentity());
			XMStoreFloat4x4(&rot_front, XMMatrixIdentity());
		}
	basic_model *model;
	XMFLOAT4X4 world;
	XMFLOAT4X4 rot_front;
	bool is_appear;
};
//
void basic_model::set(
	ID3D11Device *device, texture_mgr &tex_mgr, const std::string &model_filename,
	const std::wstring &texture_path)
{
	std::vector<m3d_material> mats;
	m3d_loader m3dloader;
	m3dloader.load_m3d(model_filename, m_Vertices, m_Indices, m_Subsets, mats);
	m_ModelMesh.set_Vertices(device, &m_Vertices[0], static_cast<UINT>(m_Vertices.size()));
	m_ModelMesh.set_Indices(device, &m_Indices[0], static_cast<UINT>(m_Indices.size()));
	m_ModelMesh.set_SubsetTable(m_Subsets);
	m_SubsetCount = mats.size();
	for (size_t i = 0; i < m_SubsetCount; ++i) {
		m_Mat.push_back(mats[i].mat);
		ID3D11ShaderResourceView *diffuse_map_srv = tex_mgr.create_texture(texture_path + mats[i].diffuse_map_name);
		m_DiffuseMapSRV.push_back(diffuse_map_srv);
		ID3D11ShaderResourceView *normal_map_srv = tex_mgr.create_texture(texture_path + mats[i].normal_map_name);
		m_NormalMapSRV.push_back(normal_map_srv);
		// m_Tex keep for binary file
		m_Tex.push_back(std::string(mats[i].diffuse_map_name.begin(), mats[i].diffuse_map_name.end()));
		m_Tex.push_back(std::string(mats[i].normal_map_name.begin(), mats[i].normal_map_name.end()));
	}
}
//
void basic_model::set(
	ID3D11Device *device, texture_mgr &tex_mgr, const std::wstring &texture_path)
{
	m_ModelMesh.set_Vertices(device, &m_Vertices[0], static_cast<UINT>(m_Vertices.size()));
	m_ModelMesh.set_Indices(device, &m_Indices[0], static_cast<UINT>(m_Indices.size()));
	m_ModelMesh.set_SubsetTable(m_Subsets);
	m_SubsetCount = m_Mat.size();
	for (size_t ix = 0; ix != m_SubsetCount; ++ix) {
		std::wstring tex_dif(m_Tex[(ix+1)*2-2].begin(), m_Tex[(ix+1)*2-2].end());
		std::wstring tex_nrm(m_Tex[(ix+1)*2-1].begin(), m_Tex[(ix+1)*2-1].end());
		ID3D11ShaderResourceView *diffuse_map_srv = tex_mgr.create_texture(texture_path + tex_dif);
		m_DiffuseMapSRV.push_back(diffuse_map_srv);
		ID3D11ShaderResourceView *normal_map_srv = tex_mgr.create_texture(texture_path + tex_nrm);
		m_NormalMapSRV.push_back(normal_map_srv);
	}
}
//
std::pair <size_t, size_t> basic_model::get_VertexRange(UINT sub_id){
	std::pair <size_t, size_t> range(
		m_Subsets[sub_id].vertex_start, m_Subsets[sub_id].vertex_start+m_Subsets[sub_id].vertex_count);
	return range;	
}
////////////////
// skinned_model
////////////////
////////////////
class skinned_model: public basic_model
{
public:
	skinned_model() {;}
	~skinned_model() {;}
	void set(
		ID3D11Device *device, texture_mgr &tex_mgr, const std::string &model_filename,
		const std::wstring &texture_path);
	void set(
		ID3D11Device *device, texture_mgr &tex_mgr, const std::wstring &texture_path);
	std::vector<pos_normal_tex_tan_skinned> m_Vertices;
	skinned_data m_SkinnedData;
};
////////////////
// skinned_model_instance
////////////////
////////////////
struct skinned_model_instance
{
	skinned_model_instance():
		model(nullptr),
		clip_name(),
		final_transforms(),
		time_pos(0.0f),
		is_appear(true) {
			XMStoreFloat4x4(&world, XMMatrixIdentity());
			XMStoreFloat4x4(&rot_front, XMMatrixIdentity());
		}
	skinned_model *model;
	XMFLOAT4X4 world;
	XMFLOAT4X4 rot_front;
	std::string clip_name;	
	std::vector<XMFLOAT4X4> final_transforms;
	float time_pos;
	bool is_appear;
	void update(float dt);
	void set_ClipName(const std::string &clip_name);
	void check_set_ClipName(const std::string &clip_name);
};
//
void skinned_model::set(
	ID3D11Device *device, texture_mgr &tex_mgr, const std::string &model_filename, const std::wstring &texture_path)
{
	std::vector<m3d_material> mats;
	m3d_loader loader1;
	loader1.load_m3d(model_filename, m_Vertices, m_Indices, m_Subsets, mats, m_SkinnedData);
	m_ModelMesh.set_Vertices(device, &m_Vertices[0], static_cast<UINT>(m_Vertices.size()));
	m_ModelMesh.set_Indices(device, &m_Indices[0], static_cast<UINT>(m_Indices.size()));
	m_ModelMesh.set_SubsetTable(m_Subsets);
	m_SubsetCount = static_cast<UINT>(mats.size());
	for(UINT i = 0; i < m_SubsetCount; ++i) {
		m_Mat.push_back(mats[i].mat);
		ID3D11ShaderResourceView *diffuse_map_srv = tex_mgr.create_texture(texture_path + mats[i].diffuse_map_name);
		m_DiffuseMapSRV.push_back(diffuse_map_srv);
		ID3D11ShaderResourceView *normal_map_srv = tex_mgr.create_texture(texture_path + mats[i].normal_map_name);
		m_NormalMapSRV.push_back(normal_map_srv);
		// m_Tex keep for binary file
		m_Tex.push_back(std::string(mats[i].diffuse_map_name.begin(), mats[i].diffuse_map_name.end()));
		m_Tex.push_back(std::string(mats[i].normal_map_name.begin(), mats[i].normal_map_name.end()));
	}
}
//
void skinned_model::set(
	ID3D11Device *device, texture_mgr &tex_mgr, const std::wstring &texture_path)
{
	m_ModelMesh.set_Vertices(device, &m_Vertices[0], static_cast<UINT>(m_Vertices.size()));
	m_ModelMesh.set_Indices(device, &m_Indices[0], static_cast<UINT>(m_Indices.size()));
	m_ModelMesh.set_SubsetTable(m_Subsets);
	m_SubsetCount = m_Mat.size();
	for (size_t ix = 0; ix != m_SubsetCount; ++ix) {
		std::wstring tex_dif(m_Tex[(ix+1)*2-2].begin(), m_Tex[(ix+1)*2-2].end());
		std::wstring tex_nrm(m_Tex[(ix+1)*2-1].begin(), m_Tex[(ix+1)*2-1].end());
		ID3D11ShaderResourceView *diffuse_map_srv = tex_mgr.create_texture(texture_path + tex_dif);
		m_DiffuseMapSRV.push_back(diffuse_map_srv);
		ID3D11ShaderResourceView *normal_map_srv = tex_mgr.create_texture(texture_path + tex_nrm);
		m_NormalMapSRV.push_back(normal_map_srv);
	}
}
//
void skinned_model_instance::update(float dt)
{
	time_pos += dt;
	model->m_SkinnedData.get_final_transforms(clip_name, time_pos, final_transforms);
	// Loop animation
	if (time_pos > model->m_SkinnedData.get_clip_end_time(clip_name)) time_pos = 0.0f;
}
//
void skinned_model_instance::set_ClipName(const std::string &c_name)
{
	if (!model->m_SkinnedData.check_clip_name(c_name)) {
		std::wostringstream os;
		os << ".m3d file clip name error: " << c_name.c_str();
		ERROR_MES(os.str().c_str());
	}
	else clip_name = c_name;
}
//
void skinned_model_instance::check_set_ClipName(const std::string &c_name)
{
	if (model->m_SkinnedData.check_clip_name(c_name)) clip_name = c_name;
}
////////////////
// simple_model
////////////////
////////////////
template <typename vertex_type>
class simple_model: public basic_model
{
	public:
	simple_model() {;}
	~simple_model() {;}
	std::vector<vertex_type> m_Vertices;
	void set_MeshBuffer(ID3D11Device *device);
	void set_SingleSubset();
	void set_MapSRV(texture_mgr &tex_mgr, const std::wstring diffuse_file, const std::wstring normal_file);
};
////////////////
// simple_model_instance
////////////////
////////////////
template <typename vertex_type>
struct simple_model_instance
{
	simple_model_instance():
		model(nullptr),
		subid(0),
		is_textrue(true),
		is_appear(true) {
			XMStoreFloat4x4(&world, XMMatrixIdentity());
			XMStoreFloat4x4(&rot_front, XMMatrixIdentity());
			XMStoreFloat4x4(&tex_transform, XMMatrixIdentity());
		}
	simple_model<vertex_type> *model;
	XMFLOAT4X4 world;
	XMFLOAT4X4 rot_front;	
	XMFLOAT4X4 tex_transform;
	UINT subid;
	bool is_textrue;
	bool is_appear;
};
//
template <typename vertex_type>
void simple_model<vertex_type>::set_MeshBuffer(ID3D11Device *device)
{
	m_ModelMesh.set_Vertices(device, &m_Vertices[0], static_cast<UINT>(m_Vertices.size()));
	m_ModelMesh.set_Indices(device, &m_Indices[0], static_cast<UINT>(m_Indices.size()));
	//
	if (m_Subsets.size() == 0) this->set_SingleSubset();
	m_SubsetCount = m_Subsets.size();
	m_ModelMesh.set_SubsetTable(m_Subsets);
}
//
template <typename vertex_type>
void simple_model<vertex_type>::set_SingleSubset()
{
	m_Subsets.clear();
	m_Subsets.push_back(mesh_geometry::subset());
	m_Subsets[0].id = 0;
	m_Subsets[0].vertex_start = 0;
	m_Subsets[0].vertex_count = static_cast<UINT>(m_Vertices.size());
	m_Subsets[0].face_start = 0;
	m_Subsets[0].face_count = static_cast<UINT>(m_Indices.size()/3);
}
//
template <typename vertex_type>
void simple_model<vertex_type>::set_MapSRV
	(texture_mgr &tex_mgr, const std::wstring diffuse_file, const std::wstring normal_file)
{
	ID3D11ShaderResourceView *diffuse_map_srv = tex_mgr.create_texture(diffuse_file);
	m_DiffuseMapSRV.push_back(diffuse_map_srv);
	ID3D11ShaderResourceView *normal_map_srv = tex_mgr.create_texture(normal_file);
	m_NormalMapSRV.push_back(normal_map_srv);
}
}
#endif