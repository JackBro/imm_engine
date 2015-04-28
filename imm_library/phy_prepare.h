////////////////
// phy_prepare.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
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
	phy_property():
		velocity(0.0f, 0.0f, 0.0f),
		velocity_nm(0.0f, 0.0f, 0.0f),
		acceleration(0.0f, 0.0f, 0.0f),
		mass(0.0f),
		friction_rev(0.3f),
		bounce(0.3f),
		stand_from(-1),
		is_ground(false),
		is_touch_ground(false)
		{;}
	XMFLOAT3 velocity;
	XMFLOAT3 velocity_nm; // velcoity of no direct mechanical motion, for example: walk
	XMFLOAT3 acceleration;
	float mass;
	float friction_rev; // reversed coefficient of friction, 0 <= this <= 1, ignore original friction > 1
	float bounce;
	int stand_from;
	bool is_ground;
	bool is_touch_ground;
};
////////////////
// phy_set_aabb
////////////////
////////////////
template <typename T_vertices, typename T_pos>
void phy_set_aabb(BoundingBox &bbox, const T_vertices &vertices, const T_pos get_pos,
	size_t ix_begin = 0, size_t ix_end = 0)
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
// phy_bound_type
////////////////
////////////////
enum phy_bound_type
{
	box,
	ori_box,
	sphere,
	null
};
////////////////
// phy_bound_mgr
// manager various type of bounding
// abstract the common points
////////////////
////////////////
class phy_bound_mgr
{
public:
	phy_bound_mgr();
	std::vector<BoundingBox> b1;
	std::vector<BoundingOrientedBox> b2;
	std::vector<BoundingSphere> b3;
	std::vector<std::pair<phy_bound_type, size_t>> map;
	void push_empty(const phy_bound_type &type);
	void transform(const size_t &ix, phy_bound_mgr &out, CXMMATRIX &world);
	bool intersects(const size_t &ixA, const size_t &ixB);
	bool intersects(const size_t &ix, CXMVECTOR &origin, CXMVECTOR &direction, float &dist);
	template <typename T_bound>
	bool intersects(const size_t &ix, const T_bound &bound);
	void pick(const int &pos_x, const int &pos_y,
		const int &client_width, const int &clinet_height,
		CXMMATRIX cam_proj, CXMMATRIX cam_view, int &out_ix,
		const std::vector<size_t> &ix_list);
	//
	XMFLOAT3 center(const size_t &ix);
	float half_y(const size_t &ix);
	void remove_all();
};
//
phy_bound_mgr::phy_bound_mgr():
	b1(), b2(), b3(), map()
{}
//
void phy_bound_mgr::push_empty(const phy_bound_type &type)
{
	switch(type) {
		case box:
			b1.emplace_back(BoundingBox());
			map.emplace_back(std::make_pair(type, b1.size()-1));
			return;
		case ori_box:
			b2.emplace_back(BoundingOrientedBox());
			map.emplace_back(std::make_pair(type, b2.size()-1));
			return;
		case sphere:
			b3.emplace_back(BoundingSphere());
			map.emplace_back(std::make_pair(type, b3.size()-1));
			return;
		case null:
			map.emplace_back(std::make_pair(type, 0));
			return;
	}
	assert(false);
}
//
void phy_bound_mgr::transform(const size_t &ix, phy_bound_mgr &out, CXMMATRIX &world)
{
	switch(map[ix].first) {
		case box: b1[map[ix].second].Transform(out.b1[map[ix].second], world); return;
		case ori_box: b2[map[ix].second].Transform(out.b2[map[ix].second], world); return;
		case sphere: b3[map[ix].second].Transform(out.b3[map[ix].second], world); return;
	}
}
//
bool phy_bound_mgr::intersects(const size_t &ixA, const size_t &ixB)
{
	switch(map[ixA].first) {
		case box:
			switch(map[ixB].first) {
				case box: return b1[map[ixA].second].Intersects(b1[map[ixB].second]);
				case ori_box: return b1[map[ixA].second].Intersects(b2[map[ixB].second]);
				case sphere: return b1[map[ixA].second].Intersects(b3[map[ixB].second]);
			}
		case ori_box:
			switch(map[ixB].first) {
				case box: return b2[map[ixA].second].Intersects(b1[map[ixB].second]);
				case ori_box: return b2[map[ixA].second].Intersects(b2[map[ixB].second]);
				case sphere: return b2[map[ixA].second].Intersects(b3[map[ixB].second]);
			}
		case sphere:
			switch(map[ixB].first) {
				case box: return b3[map[ixA].second].Intersects(b1[map[ixB].second]);
				case ori_box: return b3[map[ixA].second].Intersects(b2[map[ixB].second]);
				case sphere: return b3[map[ixA].second].Intersects(b3[map[ixB].second]);
			}
	}
	assert(false);
	return false;
}
//
bool phy_bound_mgr::intersects(const size_t &ix, CXMVECTOR &origin, CXMVECTOR &direction, float &dist)
{
	switch(map[ix].first) {
		case box: return b1[map[ix].second].Intersects(origin, direction, dist);
		case ori_box: return b2[map[ix].second].Intersects(origin, direction, dist);
		case sphere: return b3[map[ix].second].Intersects(origin, direction, dist);
	}
	assert(false);
	return false;
}
//
template <typename T_bound>
bool phy_bound_mgr::intersects(const size_t &ix, const T_bound &bound)
{
	switch(map[ix].first) {
		case box: return b1[map[ix].second].Intersects(bound);
		case ori_box: return b2[map[ix].second].Intersects(bound);
		case sphere: return b3[map[ix].second].Intersects(bound);
	}
	assert(false);
	return false;
}
//
void phy_bound_mgr::pick(const int &pos_x, const int &pos_y,
	const int &client_width, const int &clinet_height,
	CXMMATRIX cam_proj, CXMMATRIX cam_view, int &out_ix,
	const std::vector<size_t> &ix_list = std::vector<size_t>())
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
	if (ix_list.size() == 0) {
		for (size_t ix = 0; ix != map.size(); ++ix) {
			float t = 0.0f;
			if (intersects(ix, ray_origin, ray_dir, t)) {
				if (t < tmin) {tmin = t; picked_box = static_cast<int>(ix);}
			}
		}
	}
	else {
		for (size_t index = 0; index != ix_list.size(); ++index) {
			size_t ix = ix_list[index];
			float t = 0.0f;
			if (intersects(ix, ray_origin, ray_dir, t)) {
				if (t < tmin) {tmin = t; picked_box = static_cast<int>(ix);}
			}
		}
	}
	out_ix = picked_box;
}
//
XMFLOAT3 phy_bound_mgr::center(const size_t &ix)
{
	switch(map[ix].first) {
		case box: return b1[map[ix].second].Center;
		case ori_box: return b2[map[ix].second].Center;
		case sphere: return b3[map[ix].second].Center;
	}
	assert(false);
	return XMFLOAT3();
}
//
float phy_bound_mgr::half_y(const size_t &ix)
{
	switch(map[ix].first) {
		case box: return b1[map[ix].second].Extents.y;
		case ori_box: return b2[map[ix].second].Extents.y;
		case sphere: return b3[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
void phy_bound_mgr::remove_all()
{
	b1.clear();
	b2.clear();
	b3.clear();
	map.clear();
	b1.shrink_to_fit();
	b2.shrink_to_fit();
	b3.shrink_to_fit();
	map.shrink_to_fit();
}
//
}
#endif