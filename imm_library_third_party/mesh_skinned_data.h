////////////////
// mesh_skinned_data.h
// from 3D Game Programming with DirectX 11 by Frank Luna, SkinnedData.h
////////////////
////////////////
#ifndef MESH_SKINNED_DATA_H
#define MESH_SKINNED_DATA_H
#include "imm_core.h"
namespace imm
{
////////////////
// keyframe
// A keyframe defines the bone transformation at an instant in time.
////////////////
////////////////
struct keyframe
{
	keyframe();
	~keyframe();
	float time_pos;
	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT4 rotation_quat;
};
//
keyframe::~keyframe()
{
	;
}
//
keyframe::keyframe():
	time_pos(0.0f),
	translation(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f, 1.0f),
	rotation_quat(0.0f, 0.0f, 0.0f, 1.0f)
{
	;
}
////////////////
// bone_animation
// A bone_animation is defined by a list of keyframes.	For time
// values inbetween two keyframes, we interpolate between the
// two nearest keyframes that bound the time.
// We assume an animation always has two keyframes.
////////////////
////////////////
struct bone_animation
{
	float get_start_time() const;
	float get_end_time() const;
	void interpolate(float t, XMFLOAT4X4 &M) const;
	std::vector<keyframe> keyframes;
};
//
// keyframes are sorted by time, so first keyframe gives start time.
float bone_animation::get_start_time() const
{
	return keyframes.front().time_pos;
}
// keyframes are sorted by time, so last keyframe gives end time.
float bone_animation::get_end_time() const
{
	return keyframes.back().time_pos;
}
//
void bone_animation::interpolate(float t, XMFLOAT4X4 &M) const
{
	if (t <= keyframes.front().time_pos) {
		XMVECTOR S = XMLoadFloat3(&keyframes.front().scale);
		XMVECTOR P = XMLoadFloat3(&keyframes.front().translation);
		XMVECTOR Q = XMLoadFloat4(&keyframes.front().rotation_quat);
		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= keyframes.back().time_pos) {
		XMVECTOR S = XMLoadFloat3(&keyframes.back().scale);
		XMVECTOR P = XMLoadFloat3(&keyframes.back().translation);
		XMVECTOR Q = XMLoadFloat4(&keyframes.back().rotation_quat);
		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else {
		for (UINT i = 0; i < static_cast<int>(keyframes.size()-1); ++i) {
			if (t >= keyframes[i].time_pos && t <= keyframes[i+1].time_pos) {
				float lerpPercent = (t - keyframes[i].time_pos) / (keyframes[i+1].time_pos - keyframes[i].time_pos);
				XMVECTOR s0 = XMLoadFloat3(&keyframes[i].scale);
				XMVECTOR s1 = XMLoadFloat3(&keyframes[i+1].scale);
				XMVECTOR p0 = XMLoadFloat3(&keyframes[i].translation);
				XMVECTOR p1 = XMLoadFloat3(&keyframes[i+1].translation);
				XMVECTOR q0 = XMLoadFloat4(&keyframes[i].rotation_quat);
				XMVECTOR q1 = XMLoadFloat4(&keyframes[i+1].rotation_quat);
				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);
				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
				break;
			}
		}
	}
}
////////////////
// animation_clip
////////////////
////////////////
struct animation_clip
{
	float get_clip_start_time() const;
	float get_clip_end_time() const;
	void interpolate(float t, std::vector<XMFLOAT4X4> &bone_transforms) const;
	std::vector<bone_animation> bone_animations;
};
//
float animation_clip::get_clip_start_time() const
{
	// Find smallest start time over all bones in this clip.
	float t = FLT_MAX;
	for (UINT i = 0; i < bone_animations.size(); ++i) {
		t = calc_min(t, bone_animations[i].get_start_time());
	}
	return t;
}
//
float animation_clip::get_clip_end_time() const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for (UINT i = 0; i < bone_animations.size(); ++i) {
		t = calc_max(t, bone_animations[i].get_end_time());
	}
	return t;
}
//
void animation_clip::interpolate(float t, std::vector<XMFLOAT4X4> &bone_transforms) const
{
	for (UINT i = 0; i < bone_animations.size(); ++i) {
		bone_animations[i].interpolate(t, bone_transforms[i]);
	}
}
////////////////
// skinned_data
////////////////
////////////////
class skinned_data
{
public:
	UINT m_BoneCount() const;
	float get_clip_start_time(const std::string &clip_name) const;
	float get_clip_end_time(const std::string &clip_name) const;
	void set(
		std::vector<int> &bone_hierarchy,
		std::vector<XMFLOAT4X4> &bone_offsets,
		std::map<std::string, animation_clip> &animations);
	 // In a real project, you'd want to cache the result if there was a chance
	 // that you were calling this several times with the same clip_name at
	 // the same time_pos.
	void get_final_transforms(
		const std::string &clip_name, float time_pos, std::vector<XMFLOAT4X4> &final_transforms) const;
	bool check_clip_name(const std::string &clip_name);
	// Gives parent_index of ith bone.
	std::vector<int> m_BoneHierarchy;
	std::vector<XMFLOAT4X4> m_BoneOffsets;
	std::map<std::string, animation_clip> m_Animations;
};
//
float skinned_data::get_clip_start_time(const std::string &clip_name) const
{
	auto clip = m_Animations.find(clip_name);
	return clip->second.get_clip_start_time();
}
//
float skinned_data::get_clip_end_time(const std::string &clip_name) const
{
	auto clip = m_Animations.find(clip_name);
	return clip->second.get_clip_end_time();
}
//
UINT skinned_data::m_BoneCount() const
{
	return static_cast<UINT>(m_BoneHierarchy.size());
}
//
void skinned_data::set(
	std::vector<int> &bone_hierarchy,
	std::vector<XMFLOAT4X4> &bone_offsets,
	std::map<std::string, animation_clip> &animations)
{
	m_BoneHierarchy = bone_hierarchy;
	m_BoneOffsets = bone_offsets;
	m_Animations = animations;
}
//
void skinned_data::get_final_transforms(
	const std::string &clip_name, float time_pos, std::vector<XMFLOAT4X4> &final_transforms) const
{
	size_t num_bones = m_BoneOffsets.size();
	std::vector<XMFLOAT4X4> to_parent_transforms(num_bones);
	// interpolate all the bones of this clip at the given time instance.
	auto clip = m_Animations.find(clip_name);
	clip->second.interpolate(time_pos, to_parent_transforms);
	// Traverse the hierarchy and transform all the bones to the root space.
	std::vector<XMFLOAT4X4> to_root_transforms(num_bones);
	// Now find the to_rootTransform of the children.
	for (UINT i = 0; i < num_bones; ++i) {
		// parent_index < 0 indicate this is a root bone
		// The root bone has no parent, so its to_rootTransform is just its local bone transform.
		int parent_index = m_BoneHierarchy[i];
		if (parent_index < 0) {
			to_root_transforms[i] = to_parent_transforms[i];
		}
		else {
			XMMATRIX to_parent = XMLoadFloat4x4(&to_parent_transforms[i]);
			XMMATRIX parent_to_root = XMLoadFloat4x4(&to_root_transforms[parent_index]);
			XMMATRIX to_root = XMMatrixMultiply(to_parent, parent_to_root);
			XMStoreFloat4x4(&to_root_transforms[i], to_root);
		}
	}
	// Premultiply by the bone offset transform to get the final transform.
	for (UINT i = 0; i < num_bones; ++i) {
		XMMATRIX offset = XMLoadFloat4x4(&m_BoneOffsets[i]);
		XMMATRIX to_root = XMLoadFloat4x4(&to_root_transforms[i]);
		XMStoreFloat4x4(&final_transforms[i], XMMatrixMultiply(offset, to_root));
	}
}
bool skinned_data::check_clip_name(const std::string &clip_name)
{
	auto clip = m_Animations.find(clip_name);
	if (clip == m_Animations.end()) return false;
	return true;
}
}
#endif