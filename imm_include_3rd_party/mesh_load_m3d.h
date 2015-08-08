////////////////
// mesh_load_m3d.h
// from 3D Game Programming with DirectX 11 by Frank Luna, LoadM3d.h
////////////////
////////////////
#ifndef MESH_LOAD_M3D_H
#define MESH_LOAD_M3D_H
#include "mesh_skinned_data.h"
#include "mesh_geometry.h"
#include "ia_vertex.h"
namespace imm
{
////////////////
// m3d_material
////////////////
////////////////
struct m3d_material
{
	material mat;
	bool alpha_clip;
	std::string effect_type_name;
	std::wstring diffuse_map_name;
	std::wstring normal_map_name;
};
////////////////
// m3d_loader
////////////////
////////////////
class m3d_loader
{
public:
	bool load_m3d(
		const std::string &filename,
		std::vector<vertex::pntt2> &vertices,
		std::vector<UINT> &indices,
		std::vector<mesh_geometry::subset> &subsets,
		std::vector<m3d_material> &mats);
	bool load_m3d(
		const std::string &filename,
		std::vector<vertex::pntt_skinned> &vertices,
		std::vector<UINT> &indices,
		std::vector<mesh_geometry::subset> &subsets,
		std::vector<m3d_material> &mats,
		skinned_data &skin_info);
private:
	void read_Materials(std::ifstream &fin, UINT num_materials, std::vector<m3d_material> &mats);
	void read_SubsetTable(std::ifstream &fin, UINT num_subsets, std::vector<mesh_geometry::subset> &subsets);
	void read_Vertices(std::ifstream &fin, UINT num_vertices, std::vector<vertex::pntt2> &vertices);
	void read_Triangles(std::ifstream &fin, UINT num_triangles, std::vector<UINT> &indices);
	//
	void read_SkinnedVertices(std::ifstream &fin, UINT num_vertices, std::vector<vertex::pntt_skinned> &vertices);
	void read_BoneOffsets(std::ifstream &fin, UINT num_bones, std::vector<XMFLOAT4X4> &bone_offsets);
	void read_BoneHierarchy(std::ifstream &fin, UINT num_bones, std::vector<int> &bone_index_to_parent_index);
	void read_AnimationClips(std::ifstream &fin, UINT num_bones, UINT num_animation_clips, std::map<std::string, animation_clip> &animations);
	void read_BoneKeyframes(std::ifstream &fin, bone_animation &bone_anim);
};
//
bool m3d_loader::load_m3d(
	const std::string &filename,
	std::vector<vertex::pntt2> &vertices,
	std::vector<UINT> &indices,
	std::vector<mesh_geometry::subset> &subsets,
	std::vector<m3d_material> &mats)
{
	std::ifstream fin(filename);
	UINT num_materials = 0;
	UINT num_vertices = 0;
	UINT num_triangles = 0;
	UINT num_bones = 0;
	UINT num_animation_clips = 0;
	std::string ignore;
	if (fin.is_open()) {
		fin >> ignore; // file header text
		fin >> ignore >> num_materials;
		fin >> ignore >> num_vertices;
		fin >> ignore >> num_triangles;
		fin >> ignore >> num_bones;
		fin >> ignore >> num_animation_clips;
		read_Materials(fin, num_materials, mats);
		read_SubsetTable(fin, num_materials, subsets);
		read_Vertices(fin, num_vertices, vertices);
		read_Triangles(fin, num_triangles, indices);
		fin.close();
		return true;
	}
	else {
		std::string err_str(".m3d file load error: ");
		err_str += filename;
		ERROR_MESA(err_str.c_str());
	}
	return false;
}
//
bool m3d_loader::load_m3d(
	const std::string &filename,
	std::vector<vertex::pntt_skinned> &vertices,
	std::vector<UINT> &indices,
	std::vector<mesh_geometry::subset> &subsets,
	std::vector<m3d_material> &mats,
	skinned_data &skin_info)
{
	std::ifstream fin(filename);
	UINT num_materials = 0;
	UINT num_vertices = 0;
	UINT num_triangles = 0;
	UINT num_bones = 0;
	UINT num_animation_clips = 0;
	std::string ignore;
	if (fin.is_open()) {
		fin >> ignore; // file header text
		fin >> ignore >> num_materials;
		fin >> ignore >> num_vertices;
		fin >> ignore >> num_triangles;
		fin >> ignore >> num_bones;
		fin >> ignore >> num_animation_clips;
		std::vector<XMFLOAT4X4> bone_offsets;
		std::vector<int> bone_index_to_parent_index;
		std::map<std::string, animation_clip> animations;
		read_Materials(fin, num_materials, mats);
		read_SubsetTable(fin, num_materials, subsets);
		read_SkinnedVertices(fin, num_vertices, vertices);
		read_Triangles(fin, num_triangles, indices);
		read_BoneOffsets(fin, num_bones, bone_offsets);
		read_BoneHierarchy(fin, num_bones, bone_index_to_parent_index);
		read_AnimationClips(fin, num_bones, num_animation_clips, animations);
		skin_info.set(bone_index_to_parent_index, bone_offsets, animations);
		fin.close();
		return true;
	}
	else {
		std::string err_str(".m3d file load error: ");
		err_str += filename;
		ERROR_MESA(err_str.c_str());
	}
	return false;
}
//
void m3d_loader::read_Materials(std::ifstream &fin, UINT num_materials, std::vector<m3d_material> &mats)
{
	 std::string ignore;
	 mats.resize(num_materials);
	 std::string diffuse_map_name;
	 std::string normal_map_name;
	 fin >> ignore; // materials header text
	 for (UINT i = 0; i < num_materials; ++i) {
		fin >> ignore >> mats[i].mat.ambient.x  >> mats[i].mat.ambient.y    >> mats[i].mat.ambient.z;
		fin >> ignore >> mats[i].mat.diffuse.x  >> mats[i].mat.diffuse.y    >> mats[i].mat.diffuse.z;
		fin >> ignore >> mats[i].mat.specular.x >> mats[i].mat.specular.y   >> mats[i].mat.specular.z;
		fin >> ignore >> mats[i].mat.specular.w;
		fin >> ignore >> mats[i].mat.reflect.x  >> mats[i].mat.reflect.y    >> mats[i].mat.reflect.z;
		fin >> ignore >> mats[i].alpha_clip;
		fin >> ignore >> mats[i].effect_type_name;
		fin >> ignore >> diffuse_map_name;
		fin >> ignore >> normal_map_name;
		mats[i].diffuse_map_name.resize(diffuse_map_name.size(), ' ');
		mats[i].normal_map_name.resize(normal_map_name.size(), ' ');
		std::copy(diffuse_map_name.begin(), diffuse_map_name.end(), mats[i].diffuse_map_name.begin());
		std::copy(normal_map_name.begin(), normal_map_name.end(), mats[i].normal_map_name.begin());
	}
}
//
void m3d_loader::read_SubsetTable(std::ifstream &fin, UINT num_subsets, std::vector<mesh_geometry::subset> &subsets)
{
	std::string ignore;
	subsets.resize(num_subsets);
	fin >> ignore; // subset header text
	for (UINT i = 0; i < num_subsets; ++i) {
		fin >> ignore >> subsets[i].id;
		fin >> ignore >> subsets[i].vertex_start;
		fin >> ignore >> subsets[i].vertex_count;
		fin >> ignore >> subsets[i].face_start;
		fin >> ignore >> subsets[i].face_count;
	}
}
//
void m3d_loader::read_Vertices(std::ifstream &fin, UINT num_vertices, std::vector<vertex::pntt2> &vertices)
{
	std::string ignore;
	vertices.resize(num_vertices);
	fin >> ignore; // vertices header text
	for (UINT i = 0; i < num_vertices; ++i) {
		fin >> ignore >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
		fin >> ignore >> vertices[i].tangent_u.x >> vertices[i].tangent_u.y >> vertices[i].tangent_u.z >>
			vertices[i].tangent_u.w;
		fin >> ignore >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
		fin >> ignore >> vertices[i].tex.x >> vertices[i].tex.y;
	}
}
//
void m3d_loader::read_Triangles(std::ifstream &fin, UINT num_triangles, std::vector<UINT> &indices)
{
	std::string ignore;
	indices.resize(num_triangles*3);
	fin >> ignore; // triangles header text
	for (UINT i = 0; i < num_triangles; ++i) fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
}
//
void m3d_loader::read_SkinnedVertices(
	std::ifstream &fin, UINT num_vertices, std::vector<vertex::pntt_skinned> &vertices)
{
	std::string ignore;
	vertices.resize(num_vertices);
	fin >> ignore; // vertices header text
	int bone_indices[4];
	float weights[4];
	for (UINT i = 0; i < num_vertices; ++i) {
		fin >> ignore >> vertices[i].pos.x       >> vertices[i].pos.y       >> vertices[i].pos.z;
		fin >> ignore >> vertices[i].tangent_u.x >> vertices[i].tangent_u.y >> vertices[i].tangent_u.z >> vertices[i].tangent_u.w;
		fin >> ignore >> vertices[i].normal.x    >> vertices[i].normal.y    >> vertices[i].normal.z;
		fin >> ignore >> vertices[i].tex.x       >> vertices[i].tex.y;
		fin >> ignore >> weights[0]              >> weights[1]              >> weights[2]              >> weights[3];
		fin >> ignore >> bone_indices[0]         >> bone_indices[1]         >> bone_indices[2]         >> bone_indices[3];
		vertices[i].weights.x = weights[0];
		vertices[i].weights.y = weights[1];
		vertices[i].weights.z = weights[2];
		vertices[i].bone_indices[0] = (BYTE)bone_indices[0];
		vertices[i].bone_indices[1] = (BYTE)bone_indices[1];
		vertices[i].bone_indices[2] = (BYTE)bone_indices[2];
		vertices[i].bone_indices[3] = (BYTE)bone_indices[3];
	}
}
//
void m3d_loader::read_BoneOffsets(std::ifstream &fin, UINT num_bones, std::vector<XMFLOAT4X4> &bone_offsets)
{
	std::string ignore;
	bone_offsets.resize(num_bones);
	fin >> ignore; // BoneOffsets header text
	for (UINT i = 0; i < num_bones; ++i) {
		fin >> ignore >>
			bone_offsets[i](0,0) >> bone_offsets[i](0,1) >> bone_offsets[i](0,2) >> bone_offsets[i](0,3) >>
			bone_offsets[i](1,0) >> bone_offsets[i](1,1) >> bone_offsets[i](1,2) >> bone_offsets[i](1,3) >>
			bone_offsets[i](2,0) >> bone_offsets[i](2,1) >> bone_offsets[i](2,2) >> bone_offsets[i](2,3) >>
			bone_offsets[i](3,0) >> bone_offsets[i](3,1) >> bone_offsets[i](3,2) >> bone_offsets[i](3,3);
	}
}
//
void m3d_loader::read_BoneHierarchy(std::ifstream &fin, UINT num_bones, std::vector<int> &bone_index_to_parent_index)
{
	std::string ignore;
	bone_index_to_parent_index.resize(num_bones);
	fin >> ignore; // BoneHierarchy header text
	for (UINT i = 0; i < num_bones; ++i) fin >> ignore >> bone_index_to_parent_index[i];
}
//
void m3d_loader::read_AnimationClips(
	std::ifstream &fin, UINT num_bones, UINT num_animation_clips, std::map<std::string, animation_clip> &animations)
{
	std::string ignore;
	fin >> ignore; // animation_clips header text
	for (UINT clip_index = 0; clip_index < num_animation_clips; ++clip_index) {
		std::string clip_name;
		fin >> ignore >> clip_name;
		fin >> ignore; // {
		animation_clip clip;
		clip.bone_animations.resize(num_bones);
		for (UINT bone_index = 0; bone_index < num_bones; ++bone_index) {
			read_BoneKeyframes(fin, clip.bone_animations[bone_index]);
		}
		fin >> ignore; // }
		animations[clip_name] = clip;
	}
}
//
void m3d_loader::read_BoneKeyframes(std::ifstream &fin, bone_animation &bone_anim)
{
	std::string ignore;
	UINT num_keyframes = 0;
	fin >> ignore >> ignore >> num_keyframes;
	fin >> ignore; // {
	bone_anim.keyframes.resize(num_keyframes);
	for (UINT i = 0; i < num_keyframes; ++i) {
		float t = 0.0f;
		XMFLOAT3 p(0.0f, 0.0f, 0.0f);
		XMFLOAT3 s(1.0f, 1.0f, 1.0f);
		XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);
		fin >> ignore >> t;
		fin >> ignore >> p.x >> p.y >> p.z;
		fin >> ignore >> s.x >> s.y >> s.z;
		fin >> ignore >> q.x >> q.y >> q.z >> q.w;
		bone_anim.keyframes[i].time_pos = t;
		bone_anim.keyframes[i].translation = p;
		bone_anim.keyframes[i].scale = s;
		bone_anim.keyframes[i].rotation_quat = q;
	}
	fin >> ignore; // }
}
}
#endif