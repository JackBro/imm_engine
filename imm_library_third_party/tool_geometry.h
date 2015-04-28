////////////////
// tool_geometry.h
// see 3D Game Programming with DirectX 11 by Frank Luna, GeometryGenerator.h
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
// Defines a static class for procedurally generating the geometry of 
// common mathematical objects.
// All triangles are generated "outward" facing.  If you want "inward" 
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
////////////////
////////////////
#ifndef TOOL_GEOMETRY_H
#define TOOL_GEOMETRY_H
#include "imm_core.h"
////////////////
// geometry
////////////////
////////////////
namespace imm
{
class geometry
{
public:
	struct vertex {
		vertex() {}
		vertex(const XMFLOAT3 &p, const XMFLOAT3 &n, const XMFLOAT3 &t, const XMFLOAT2 &uv):
			position(p), normal(n), tangent_u(t), tex_c(uv) {}
		vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v
		):
			position(px, py, pz), normal(nx, ny, nz),
			tangent_u(tx, ty, tz), tex_c(u, v) {}
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangent_u;
		XMFLOAT2 tex_c;
	};
	struct mesh_data {
		std::vector<vertex> vertices;
		std::vector<UINT> indices;
	};
	void create_box(float width, float height, float depth, mesh_data &mesh_data_o);
	void create_sphere(float radius, UINT slice_count, UINT stack_count, mesh_data &mesh_data_o);
	void create_geosphere(float radius, UINT num_subdivisions, mesh_data &mesh_data_o);
	void create_cylinder(
		float bottom_radius, float top_radius, float height, UINT slice_count, UINT stack_count, mesh_data &mesh_data_o);
	void create_grid(float width, float depth, UINT m, UINT n, mesh_data &mesh_data_o);
	void create_fullscreen_quad(mesh_data &mesh_data_o);
private:
	void subdivide(mesh_data &mesh_data_o);
	void build_cylinder_top_cap(float top_radius, float height, UINT slice_count,  mesh_data &mesh_data_o);
	void build_cylinder_bottom_cap(float bottom_radius, float height, UINT slice_count, mesh_data &mesh_data_o);
};
//
void geometry::create_box(float width, float height, float depth, mesh_data &mesh_data_o)
{
	//
	// Create the vertices.
	//
	vertex v[24];
	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;
	// Fill in the front face vertex data.
	v[0] = vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// Fill in the back face vertex data.
	v[4] = vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Fill in the top face vertex data.
	v[8]  = vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9]  = vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// Fill in the bottom face vertex data.
	v[12] = vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Fill in the left face vertex data.
	v[16] = vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	// Fill in the right face vertex data.
	v[20] = vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	mesh_data_o.vertices.assign(&v[0], &v[24]);
	//
	// Create the indices.
	//
	UINT i[36];
	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	// Fill in the back face index data
	i[6] = 4; i[7]	= 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;
	// Fill in the top face index data
	i[12] = 8; i[13] =	9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;
	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;
	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;
	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;
	mesh_data_o.indices.assign(&i[0], &i[36]);
}
//
void geometry::create_sphere(float radius, UINT slice_count, UINT stack_count, mesh_data &mesh_data_o)
{
	mesh_data_o.vertices.clear();
	mesh_data_o.indices.clear();
	// Compute the vertices stating at the top pole and moving down the stacks.
	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	vertex top_vertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertex bottom_vertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	mesh_data_o.vertices.push_back(top_vertex);
	float phi_step	= XM_PI/stack_count;
	float theta_step = 2.0f*XM_PI/slice_count;
	// Compute vertices for each stack ring (do not count the poles as rings).
	for(UINT i = 1; i <= stack_count-1; ++i) {
		float phi = i*phi_step;
		// vertices of ring.
		for(UINT j = 0; j <= stack_count; ++j) {
			float theta = j*theta_step;
			vertex v;
			// spherical to cartesian
			v.position.x = radius*sinf(phi)*cosf(theta);
			v.position.y = radius*cosf(phi);
			v.position.z = radius*sinf(phi)*sinf(theta);
			// Partial derivative of P with respect to theta
			v.tangent_u.x = -radius*sinf(phi)*sinf(theta);
			v.tangent_u.y = 0.0f;
			v.tangent_u.z = +radius*sinf(phi)*cosf(theta);
			XMVECTOR t = XMLoadFloat3(&v.tangent_u);
			XMStoreFloat3(&v.tangent_u, XMVector3Normalize(t));
			XMVECTOR p = XMLoadFloat3(&v.position);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));
			v.tex_c.x = theta / XM_2PI;
			v.tex_c.y = phi / XM_PI;
			mesh_data_o.vertices.push_back(v);
		}
	}
	mesh_data_o.vertices.push_back(bottom_vertex);
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	for(UINT i = 1; i <= slice_count; ++i) {
		mesh_data_o.indices.push_back(0);
		mesh_data_o.indices.push_back(i+1);
		mesh_data_o.indices.push_back(i);
	}
	// Compute indices for inner stacks (not connected to poles).
	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT base_index = 1;
	UINT ring_vertex_count = slice_count+1;
	for(UINT i = 0; i < stack_count-2; ++i) {
		for(UINT j = 0; j < slice_count; ++j) {
			mesh_data_o.indices.push_back(base_index + i*ring_vertex_count + j);
			mesh_data_o.indices.push_back(base_index + i*ring_vertex_count + j+1);
			mesh_data_o.indices.push_back(base_index + (i+1)*ring_vertex_count + j);
			mesh_data_o.indices.push_back(base_index + (i+1)*ring_vertex_count + j);
			mesh_data_o.indices.push_back(base_index + i*ring_vertex_count + j+1);
			mesh_data_o.indices.push_back(base_index + (i+1)*ring_vertex_count + j+1);
		}
	}
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	// South pole vertex was added last.
	UINT south_pole_index = (UINT)mesh_data_o.vertices.size()-1;
	// Offset the indices to the index of the first vertex in the last ring.
	base_index = south_pole_index - ring_vertex_count;
	for(UINT i = 0; i < slice_count; ++i) {
		mesh_data_o.indices.push_back(south_pole_index);
		mesh_data_o.indices.push_back(base_index+i);
		mesh_data_o.indices.push_back(base_index+i+1);
	}
}
//
void geometry::subdivide(mesh_data &mesh_data_o)
{
	// Save a copy of the input geometry.
	mesh_data input_copy = mesh_data_o;
	mesh_data_o.vertices.resize(0);
	mesh_data_o.indices.resize(0);
	//		 v1
	//		 *
	//		/ \
	//	   /   \
	//	m0*-----*m1
	//	 / \   / \
	//	/	\ /   \
	// *-----*-----*
	// v0	 m2 	v2
	UINT num_tris = static_cast<UINT>(input_copy.indices.size()/3);
	for(UINT i = 0; i < num_tris; ++i) {
		vertex v0 = input_copy.vertices[ input_copy.indices[i*3+0] ];
		vertex v1 = input_copy.vertices[ input_copy.indices[i*3+1] ];
		vertex v2 = input_copy.vertices[ input_copy.indices[i*3+2] ];
		//
		// Generate the midpoints.
		//
		vertex m0, m1, m2;
		// For subdivision, we just care about the position component.	We derive the other
		// vertex components in CreateGeosphere.
		m0.position = XMFLOAT3(
			0.5f*(v0.position.x+v1.position.x),
			0.5f*(v0.position.y+v1.position.y),
			0.5f*(v0.position.z+v1.position.z));
		m1.position = XMFLOAT3(
			0.5f*(v1.position.x+v2.position.x),
			0.5f*(v1.position.y+v2.position.y),
			0.5f*(v1.position.z+v2.position.z));
		m2.position = XMFLOAT3(
			0.5f*(v0.position.x+v2.position.x),
			0.5f*(v0.position.y+v2.position.y),
			0.5f*(v0.position.z+v2.position.z));
		//
		// Add new geometry.
		//
		mesh_data_o.vertices.push_back(v0); // 0
		mesh_data_o.vertices.push_back(v1); // 1
		mesh_data_o.vertices.push_back(v2); // 2
		mesh_data_o.vertices.push_back(m0); // 3
		mesh_data_o.vertices.push_back(m1); // 4
		mesh_data_o.vertices.push_back(m2); // 5
		//
		mesh_data_o.indices.push_back(i*6+0);
		mesh_data_o.indices.push_back(i*6+3);
		mesh_data_o.indices.push_back(i*6+5);
		//
		mesh_data_o.indices.push_back(i*6+3);
		mesh_data_o.indices.push_back(i*6+4);
		mesh_data_o.indices.push_back(i*6+5);
		//
		mesh_data_o.indices.push_back(i*6+5);
		mesh_data_o.indices.push_back(i*6+4);
		mesh_data_o.indices.push_back(i*6+2);
		//
		mesh_data_o.indices.push_back(i*6+3);
		mesh_data_o.indices.push_back(i*6+1);
		mesh_data_o.indices.push_back(i*6+4);
	}
}
//
void geometry::create_geosphere(float radius, UINT num_subdivisions, mesh_data &mesh_data_o)
{
	// Put a cap on the number of subdivisions.
	num_subdivisions = imm::calc_min(num_subdivisions, 5u);
	// Approximate a sphere by tessellating an icosahedron.
	const float x = 0.525731f;
	const float z = 0.850651f;
	XMFLOAT3 pos[12] = {
		XMFLOAT3(-x, 0.0f, z),	XMFLOAT3(x, 0.0f, z),
		XMFLOAT3(-x, 0.0f, -z), XMFLOAT3(x, 0.0f, -z),
		XMFLOAT3(0.0f, z, x),	XMFLOAT3(0.0f, z, -x),
		XMFLOAT3(0.0f, -z, x),	XMFLOAT3(0.0f, -z, -x),
		XMFLOAT3(z, x, 0.0f),	XMFLOAT3(-z, x, 0.0f),
		XMFLOAT3(z, -x, 0.0f),	XMFLOAT3(-z, -x, 0.0f)
	};
	DWORD k[60] = {
		1,4,0,	4,9,0,	4,5,9,	8,5,4,	1,8,4,
		1,10,8, 10,3,8, 8,3,5,	3,2,5,	3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,	11,2,7
	};
	mesh_data_o.vertices.resize(12);
	mesh_data_o.indices.resize(60);
	for(UINT i = 0; i < 12; ++i) mesh_data_o.vertices[i].position = pos[i];
	for(UINT i = 0; i < 60; ++i) mesh_data_o.indices[i] = k[i];
	for(UINT i = 0; i < num_subdivisions; ++i) subdivide(mesh_data_o);
	// Project vertices onto sphere and scale.
	for(UINT i = 0; i < mesh_data_o.vertices.size(); ++i) {
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mesh_data_o.vertices[i].position));
		// Project onto sphere.
		XMVECTOR p = radius*n;
		XMStoreFloat3(&mesh_data_o.vertices[i].position, p);
		XMStoreFloat3(&mesh_data_o.vertices[i].normal, n);
		// Derive texture coordinates from spherical coordinates.
		float theta = imm::angle_from_xy(
			mesh_data_o.vertices[i].position.x,
			mesh_data_o.vertices[i].position.z);
		float phi = acosf(mesh_data_o.vertices[i].position.y/radius);
		mesh_data_o.vertices[i].tex_c.x = theta/XM_2PI;
		mesh_data_o.vertices[i].tex_c.y = phi/XM_PI;
		// Partial derivative of P with respect to theta
		mesh_data_o.vertices[i].tangent_u.x = -radius*sinf(phi)*sinf(theta);
		mesh_data_o.vertices[i].tangent_u.y = 0.0f;
		mesh_data_o.vertices[i].tangent_u.z = +radius*sinf(phi)*cosf(theta);
		XMVECTOR t = XMLoadFloat3(&mesh_data_o.vertices[i].tangent_u);
		XMStoreFloat3(&mesh_data_o.vertices[i].tangent_u, XMVector3Normalize(t));
	}
}
//
void geometry::create_cylinder(
	float bottom_radius, float top_radius, float height, UINT slice_count, UINT stack_count, mesh_data &mesh_data_o)
{
	mesh_data_o.vertices.clear();
	mesh_data_o.indices.clear();
	// Build Stacks.
	float stack_height = height/stack_count;
	// Amount to increment radius as we move up each stack level from bottom to top.
	float radius_step = (top_radius-bottom_radius)/stack_count;
	UINT ring_count = stack_count+1;
	// Compute vertices for each stack ring starting at the bottom and moving up.
	for(UINT i = 0; i < ring_count; ++i) {
		float y = -0.5f*height+i*stack_height;
		float r = bottom_radius+i*radius_step;
		// vertices of ring
		float d_theta = 2.0f*XM_PI/slice_count;
		for(UINT j = 0; j <= slice_count; ++j) {
			vertex vertex;
			float c = cosf(j*d_theta);
			float s = sinf(j*d_theta);
			vertex.position = XMFLOAT3(r*c, y, r*s);
			vertex.tex_c.x = (float)j/slice_count;
			vertex.tex_c.y = 1.0f-(float)i/stack_count;
			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//	 Let r0 be the bottom radius and let r1 be the top radius.
			//	 y(v) = h - hv for v in [0,1].
			//	 r(v) = r1 + (r0-r1)v
			//
			//	 x(t, v) = r(v)*cos(t)
			//	 y(t, v) = h - hv
			//	 z(t, v) = r(v)*sin(t)
			//
			//	dx/dt = -r(v)*sin(t)
			//	dy/dt = 0
			//	dz/dt = +r(v)*cos(t)
			//
			//	dx/dv = (r0-r1)*cos(t)
			//	dy/dv = -h
			//	dz/dv = (r0-r1)*sin(t)
			//
			// This is unit length.
			vertex.tangent_u = XMFLOAT3(-s, 0.0f, c);
			float dr = bottom_radius-top_radius;
			XMFLOAT3 bitangent(dr*c, -height, dr*s);
			XMVECTOR t = XMLoadFloat3(&vertex.tangent_u);
			XMVECTOR b = XMLoadFloat3(&bitangent);
			XMVECTOR n = XMVector3Normalize(XMVector3Cross(t, b));
			XMStoreFloat3(&vertex.normal, n);
			mesh_data_o.vertices.push_back(vertex);
		}
	}
	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ring_vertex_count = slice_count+1;
	// Compute indices for each stack.
	for(UINT i = 0; i < stack_count; ++i) {
		for(UINT j = 0; j < slice_count; ++j) {
			mesh_data_o.indices.push_back(i*ring_vertex_count+j);
			mesh_data_o.indices.push_back((i+1)*ring_vertex_count+j);
			mesh_data_o.indices.push_back((i+1)*ring_vertex_count+j+1);
			mesh_data_o.indices.push_back(i*ring_vertex_count+j);
			mesh_data_o.indices.push_back((i+1)*ring_vertex_count+j+1);
			mesh_data_o.indices.push_back(i*ring_vertex_count+j+1);
		}
	}
	build_cylinder_top_cap(top_radius, height, slice_count, mesh_data_o);
	build_cylinder_bottom_cap(bottom_radius, height, slice_count, mesh_data_o);
}
//
void geometry::build_cylinder_top_cap(float top_radius, float height, UINT slice_count, mesh_data &mesh_data_o)
{
	UINT base_index = (UINT)mesh_data_o.vertices.size();
	float y = 0.5f*height;
	float d_theta = 2.0f*XM_PI/slice_count;
	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for(UINT i = 0; i <= slice_count; ++i) {
		float x = top_radius*cosf(i*d_theta);
		float z = top_radius*sinf(i*d_theta);
		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x/height + 0.5f;
		float v = z/height + 0.5f;
		mesh_data_o.vertices.push_back( vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
	}
	// Cap center vertex.
	mesh_data_o.vertices.push_back( vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );
	// Index of center vertex.
	UINT center_index = (UINT)mesh_data_o.vertices.size()-1;
	for(UINT i = 0; i < slice_count; ++i) {
		mesh_data_o.indices.push_back(center_index);
		mesh_data_o.indices.push_back(base_index+i+1);
		mesh_data_o.indices.push_back(base_index+i);
	}
}
//
void geometry::build_cylinder_bottom_cap(float bottom_radius, float height, UINT slice_count, mesh_data &mesh_data_o)
{
	// Build bottom cap.
	UINT base_index = (UINT)mesh_data_o.vertices.size();
	float y = -0.5f*height;
	// vertices of ring
	float d_theta = 2.0f*XM_PI/slice_count;
	for(UINT i = 0; i <= slice_count; ++i) {
		float x = bottom_radius*cosf(i*d_theta);
		float z = bottom_radius*sinf(i*d_theta);
		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x/height+0.5f;
		float v = z/height+0.5f;
		mesh_data_o.vertices.push_back(vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}
	// Cap center vertex.
	mesh_data_o.vertices.push_back(vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
	// Cache the index of center vertex.
	UINT center_index = (UINT)mesh_data_o.vertices.size()-1;
	for(UINT i = 0; i < slice_count; ++i) {
		mesh_data_o.indices.push_back(center_index);
		mesh_data_o.indices.push_back(base_index+i);
		mesh_data_o.indices.push_back(base_index+i+1);
	}
}
//
void geometry::create_grid(float width, float depth, UINT m, UINT n, mesh_data &mesh_data_o)
{
	UINT vertex_count = m*n;
	UINT face_count	 = (m-1)*(n-1)*2;
	// Create the vertices.
	float half_width = 0.5f*width;
	float half_depth = 0.5f*depth;
	float dx = width/(n-1);
	float dz = depth/(m-1);
	float du = 1.0f/(n-1);
	float dv = 1.0f/(m-1);
	mesh_data_o.vertices.resize(vertex_count);
	for(UINT i = 0; i < m; ++i) {
		float z = half_depth-i*dz;
		for(UINT j = 0; j < n; ++j) {
			float x = -half_width + j*dx;
			mesh_data_o.vertices[i*n+j].position = XMFLOAT3(x, 0.0f, z);
			mesh_data_o.vertices[i*n+j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mesh_data_o.vertices[i*n+j].tangent_u = XMFLOAT3(1.0f, 0.0f, 0.0f);
			// Stretch texture over grid.
			mesh_data_o.vertices[i*n+j].tex_c.x = j*du;
			mesh_data_o.vertices[i*n+j].tex_c.y = i*dv;
		}
	}
	// Create the indices.
	mesh_data_o.indices.resize(face_count*3); // 3 indices per face
	// Iterate over each quad and compute indices.
	UINT k = 0;
	for(UINT i = 0; i < m-1; ++i) {
		for(UINT j = 0; j < n-1; ++j) {
			mesh_data_o.indices[k]   = i*n+j;
			mesh_data_o.indices[k+1] = i*n+j+1;
			mesh_data_o.indices[k+2] = (i+1)*n+j;
			mesh_data_o.indices[k+3] = (i+1)*n+j;
			mesh_data_o.indices[k+4] = i*n+j+1;
			mesh_data_o.indices[k+5] = (i+1)*n+j+1;
			k += 6; // next quad
		}
	}
}
//
void geometry::create_fullscreen_quad(mesh_data &mesh_data_o)
{
	mesh_data_o.vertices.resize(4);
	mesh_data_o.indices.resize(6);
	// Position coordinates specified in NDC space.
	mesh_data_o.vertices[0] = vertex(
		-1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);
	mesh_data_o.vertices[1] = vertex(
		-1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);
	mesh_data_o.vertices[2] = vertex(
		+1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);
	mesh_data_o.vertices[3] = vertex(
		+1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);
	mesh_data_o.indices[0] = 0;
	mesh_data_o.indices[1] = 1;
	mesh_data_o.indices[2] = 2;
	mesh_data_o.indices[3] = 0;
	mesh_data_o.indices[4] = 2;
	mesh_data_o.indices[5] = 3;
}
}
#endif