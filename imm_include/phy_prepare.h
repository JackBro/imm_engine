////////////////
// phy_prepare.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef PHY_PREPARE_H
#define PHY_PREPARE_H
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
using namespace DirectX;
namespace imm
{
////////////////
// phy_property
////////////////
////////////////
struct phy_property
{
	phy_property();
	XMFLOAT3 velocity;
	XMFLOAT3 vel_indirect; // velcoity of no direct mechanical motion, for example: walk, run
	XMFLOAT3 vel_bring;
	XMFLOAT3 acceleration;
	float mass; // not use now
	float friction_rev; // reversed coefficient of friction, 0 <= this <= 1, ignore original friction > 1
	float bounce;
	int stand_on;
	bool is_ground;
	bool is_on_ground;
	bool is_abnormal;
};
//
phy_property::phy_property():
	velocity(0.0f, 0.0f, 0.0f),
	vel_indirect(0.0f, 0.0f, 0.0f),
	vel_bring(0.0f, 0.0f, 0.0f),
	acceleration(0.0f, 0.0f, 0.0f),
	mass(0.0f),
	friction_rev(0.3f),
	bounce(0.3f),
	stand_on(-1),
	is_ground(false),
	is_on_ground(false),
	is_abnormal(false)	
{
	;
}
////////////////
// phy_set_box
////////////////
////////////////
template <typename T_vertices, typename T_pos, typename T_bound>
void phy_set_box(
	T_bound &bbox,
	const T_vertices &vertices,
	const T_pos get_pos,
	size_t ix_begin = 0,
	size_t ix_end = 0)
{
	if (ix_end == 0) ix_end = vertices.size();
	if (ix_begin > ix_end) ix_begin = 0;
	XMFLOAT3 v_minf3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	XMFLOAT3 v_maxf3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	XMVECTOR v_min = XMLoadFloat3(&v_minf3);
	XMVECTOR v_max = XMLoadFloat3(&v_maxf3);
	for (; ix_begin != ix_end; ++ix_begin) {
		XMVECTOR P = XMLoadFloat3(get_pos(vertices[ix_begin]));
		v_min = XMVectorMin(v_min, P);
		v_max = XMVectorMax(v_max, P);
	}
	XMStoreFloat3(&bbox.Center, XMVectorScale(XMVectorAdd(v_min, v_max), 0.5f));
	XMStoreFloat3(&bbox.Extents, XMVectorScale(XMVectorSubtract(v_max, v_min), 0.5f));
	return;
}
////////////////
// phy_set_sphere
////////////////
////////////////
template <typename T_vertices, typename T_pos>
void phy_set_sphere(
	BoundingSphere &sphere,
	const T_vertices &vertices,
	const T_pos get_pos,
	size_t ix_begin = 0,
	size_t ix_end = 0)
{
	if (ix_end == 0) ix_end = vertices.size();
	if (ix_begin > ix_end) ix_begin = 0;
	XMFLOAT3 v_minf3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	XMFLOAT3 v_maxf3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	XMVECTOR v_min = XMLoadFloat3(&v_minf3);
	XMVECTOR v_max = XMLoadFloat3(&v_maxf3);
	for (; ix_begin != ix_end; ++ix_begin) {
		XMVECTOR P = XMLoadFloat3(get_pos(vertices[ix_begin]));
		v_min = XMVectorMin(v_min, P);
		v_max = XMVectorMax(v_max, P);
	}
	XMStoreFloat3(&sphere.Center, XMVectorScale(XMVectorAdd(v_min, v_max), 0.5f));
	XMStoreFloat3(&sphere.Radius, XMVectorScale(XMVectorSubtract(v_max, v_min), 0.5f));
	return;
}
////////////////
// phy_set_box_scale
////////////////
////////////////
template <typename T_bound>
void phy_set_box_scale(T_bound &bbox, const std::vector<float> &offset)
{
	assert(offset.size() == 3);
	bbox.Extents.x = bbox.Extents.x*offset[0];
	bbox.Extents.y = bbox.Extents.y*offset[1];
	bbox.Extents.z = bbox.Extents.z*offset[2];
}
////////////////
// PHY_BOUND_TYPE
////////////////
////////////////
enum PHY_BOUND_TYPE
{
	PHY_BOUND_BOX     = 0,
	PHY_BOUND_ORI_BOX = 1,
	PHY_BOUND_SPHERE  = 2,
	PHY_BOUND_NULL    = 3,
};
////////////////
// phy_bound_mgr
// manager various types of bounding
////////////////
////////////////
template <typename T_app>
class phy_bound_mgr
{
public:
	phy_bound_mgr();
	T_app *app;
	std::vector<BoundingBox> bd0;
	std::vector<BoundingOrientedBox> bd1;
	std::vector<BoundingSphere> bd2;
	std::vector<std::pair<PHY_BOUND_TYPE, size_t>> map;
	void init(T_app *app_in);
	void push_back_empty(const PHY_BOUND_TYPE &type);
	void transform(const size_t &ix, phy_bound_mgr &out, CXMMATRIX &world);
	bool intersects(const size_t &ixA, const size_t &ixB);
	bool intersects(const size_t &ix, CXMVECTOR &origin, CXMVECTOR &direction, float &dist);
	template <typename T_bound>
	bool intersects(const size_t &ix, const T_bound &bound);
	template <typename T_object>
	ContainmentType contains(const size_t &ix, const T_object &object);
	void pick(
		const int &pos_x,
		const int &pos_y,
		const int &client_width,
		const int &clinet_height,
		CXMMATRIX cam_proj,
		CXMMATRIX cam_view,
		int &out_ix);
	//
	const XMFLOAT3& center(const size_t &ix);
	float extents_y(const size_t &ix);
	float extents_x(const size_t &ix);
	float extents_z(const size_t &ix);
	void remove_all();
};
//
template <typename T_app>
phy_bound_mgr<T_app>::phy_bound_mgr():
	bd0(),
	bd1(),
	bd2(),
	map()
{
	;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::push_back_empty(const PHY_BOUND_TYPE &type)
{
	switch(type) {
	case PHY_BOUND_BOX:
		bd0.emplace_back();
		map.emplace_back(type, bd0.size()-1);
		return;
	case PHY_BOUND_ORI_BOX:
		bd1.emplace_back();
		map.emplace_back(type, bd1.size()-1);
		return;
	case PHY_BOUND_SPHERE:
		bd2.emplace_back();
		map.emplace_back(type, bd2.size()-1);
		return;
	case PHY_BOUND_NULL:
		map.emplace_back(type, 0);
		return;
	}
	assert(false);
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::transform(const size_t &ix, phy_bound_mgr &out, CXMMATRIX &world)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: bd0[map[ix].second].Transform(out.bd0[map[ix].second], world); return;
	case PHY_BOUND_ORI_BOX: bd1[map[ix].second].Transform(out.bd1[map[ix].second], world); return;
	case PHY_BOUND_SPHERE: bd2[map[ix].second].Transform(out.bd2[map[ix].second], world); return;
	}
}
//
template <typename T_app>
bool phy_bound_mgr<T_app>::intersects(const size_t &ixA, const size_t &ixB)
{
	switch(map[ixA].first) {
	case PHY_BOUND_BOX:
		switch(map[ixB].first) {
		case PHY_BOUND_BOX: return bd0[map[ixA].second].Intersects(bd0[map[ixB].second]);
		case PHY_BOUND_ORI_BOX: return bd0[map[ixA].second].Intersects(bd1[map[ixB].second]);
		case PHY_BOUND_SPHERE: return bd0[map[ixA].second].Intersects(bd2[map[ixB].second]);
		}
	case PHY_BOUND_ORI_BOX:
		switch(map[ixB].first) {
		case PHY_BOUND_BOX: return bd1[map[ixA].second].Intersects(bd0[map[ixB].second]);
		case PHY_BOUND_ORI_BOX: return bd1[map[ixA].second].Intersects(bd1[map[ixB].second]);
		case PHY_BOUND_SPHERE: return bd1[map[ixA].second].Intersects(bd2[map[ixB].second]);
		}
	case PHY_BOUND_SPHERE:
		switch(map[ixB].first) {
		case PHY_BOUND_BOX: return bd2[map[ixA].second].Intersects(bd0[map[ixB].second]);
		case PHY_BOUND_ORI_BOX: return bd2[map[ixA].second].Intersects(bd1[map[ixB].second]);
		case PHY_BOUND_SPHERE: return bd2[map[ixA].second].Intersects(bd2[map[ixB].second]);
		}
	}
	assert(false);
	return false;
}
//
template <typename T_app>
bool phy_bound_mgr<T_app>::intersects(const size_t &ix, CXMVECTOR &origin, CXMVECTOR &direction, float &dist)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Intersects(origin, direction, dist);
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Intersects(origin, direction, dist);
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Intersects(origin, direction, dist);
	}
	assert(false);
	return false;
}
//
template <typename T_app>
template <typename T_bound>
bool phy_bound_mgr<T_app>::intersects(const size_t &ix, const T_bound &bound)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Intersects(bound);
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Intersects(bound);
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Intersects(bound);
	}
	assert(false);
	return false;
}
//
template <typename T_app>
template <typename T_object>
ContainmentType phy_bound_mgr<T_app>::contains(const size_t &ix, const T_object &object)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Contains(object);
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Contains(object);
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Contains(object);
	}
	assert(false);
	return ContainmentType::DISJOINT;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::pick(
	const int &pos_x,
	const int &pos_y,
	const int &client_width,
	const int &clinet_height,
	CXMMATRIX cam_proj,
	CXMMATRIX cam_view,
	int &out_ix)
{
	// Compute picking ray in view space.
	float vx = (+2.0f*pos_x/client_width  - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*pos_y/clinet_height + 1.0f)/cam_proj.r[1].m128_f32[1];
	// Ray definition in view space.
	XMVECTOR ray_origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR ray_dir = XMVectorSet(vx, vy, 1.0f, 0.0f);
	// Tranform ray to world.
	XMVECTOR det_view = XMMatrixDeterminant(cam_view);
	XMMATRIX inv_view = XMMatrixInverse(&det_view, cam_view);
	ray_origin = XMVector3TransformCoord(ray_origin, inv_view);
	ray_dir = XMVector3TransformNormal(ray_dir, inv_view);
	// Make the ray direction unit length for the intersection tests.
	ray_dir = XMVector3Normalize(ray_dir);
	int picked_box = -1;
	float tmin = FLT_MAX;
	// Find the nearest ray/box intersection.
	for (size_t ix = 0; ix != map.size(); ++ix) {
		float t = 0.0f;
		if (!app->m_Inst.m_Stat[ix].is_invoke_physics()) continue;
		if (intersects(ix, ray_origin, ray_dir, t)) {
			if (t < tmin) {tmin = t; picked_box = static_cast<int>(ix);}
		}
	}
	out_ix = picked_box;
}
//
template <typename T_app>
const XMFLOAT3& phy_bound_mgr<T_app>::center(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Center;
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Center;
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Center;
	}
	assert(false);
	return bd0[map[ix].second].Center;
}
//
template <typename T_app>
float phy_bound_mgr<T_app>::extents_y(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Extents.y;
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Extents.y;
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
template <typename T_app>
float phy_bound_mgr<T_app>::extents_x(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Extents.x;
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Extents.x;
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
template <typename T_app>
float phy_bound_mgr<T_app>::extents_z(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Extents.z;
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Extents.z;
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::remove_all()
{
	bd0.clear();
	bd1.clear();
	bd2.clear();
	map.clear();
	bd0.shrink_to_fit();
	bd1.shrink_to_fit();
	bd2.shrink_to_fit();
	map.shrink_to_fit();
}
//
}
#endif