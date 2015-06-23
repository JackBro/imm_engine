////////////////
// simu_wave.h
// Waves.h by Frank Luna (C) 2011 All Rights Reserved.
// Performs the calculations for the wave simulation.  After the simulation has been
// updated, the client must copy the current solution into vertex buffers for rendering.
// This class only does the calculations, it does not do any drawing.
////////////////
////////////////
#ifndef SIMU_WAVE_H
#define SIMU_WAVE_H
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <algorithm>
#include <cassert>
namespace imm
{
////////////////
// simu_wave
////////////////
////////////////
class simu_wave
{
public:
	simu_wave();
	~simu_wave();
	UINT row_count() const {return m_NumRows;}
	UINT column_count() const {return m_NumCols;}
	UINT vertex_count() const {return m_VertexCount;}
	UINT triangle_count() const {return m_TriangleCount;}
	float width() const {return m_NumCols*m_SpatialStep;}
	float depth() const {return m_NumRows*m_SpatialStep;}
	// Returns the solution at the ith grid point.
	const XMFLOAT3 &operator[](int i) const {return m_CurrSolution[i];}
	// Returns the solution normal at the ith grid point.
	const XMFLOAT3 &normal(int i) const {return m_Normals[i];}
	// Returns the unit tangent vector at the ith grid point in the local x-axis direction.
	const XMFLOAT3 &tangent_x(int i) const {return m_TangentX[i];}
	void init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void update(float dt);
	void disturb(UINT i, UINT j, float magnitude);
private:
	UINT m_NumRows;
	UINT m_NumCols;
	UINT m_VertexCount;
	UINT m_TriangleCount;
	// Simulation constants we can precompute.
	float m_K1;
	float m_K2;
	float m_K3;
	float m_TimeStep;
	float m_SpatialStep;
	XMFLOAT3 *m_PrevSolution;
	XMFLOAT3 *m_CurrSolution;
	XMFLOAT3 *m_Normals;
	XMFLOAT3 *m_TangentX;
};
//
simu_wave::simu_wave():
	m_NumRows(0),
	m_NumCols(0),
	m_VertexCount(0),
	m_TriangleCount(0),
	m_K1(0.0f),
	m_K2(0.0f),
	m_K3(0.0f),
	m_TimeStep(0.0f),
	m_SpatialStep(0.0f),
	m_PrevSolution(0),
	m_CurrSolution(0),
	m_Normals(0),
	m_TangentX(0)
{
}
//
simu_wave::~simu_wave()
{
	delete[] m_PrevSolution;
	delete[] m_CurrSolution;
	delete[] m_Normals;
	delete[] m_TangentX;
}
//
void simu_wave::init(UINT m, UINT n, float dx, float dt, float speed, float damping)
{
	m_NumRows  = m;
	m_NumCols  = n;
	m_VertexCount   = m*n;
	m_TriangleCount = (m-1)*(n-1)*2;
	m_TimeStep   = dt;
	m_SpatialStep = dx;
	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	m_K1    = (damping*dt-2.0f)/d;
	m_K2    = (4.0f-8.0f*e)/d;
	m_K3    = (2.0f*e)/d;
	// In case init() called again.
	delete[] m_PrevSolution;
	delete[] m_CurrSolution;
	delete[] m_Normals;
	delete[] m_TangentX;
	m_PrevSolution  = new XMFLOAT3[m*n];
	m_CurrSolution  = new XMFLOAT3[m*n];
	m_Normals       = new XMFLOAT3[m*n];
	m_TangentX      = new XMFLOAT3[m*n];
	// Generate grid vertices in system memory.
	float halfwidth = (n-1)*dx*0.5f;
	float halfdepth = (m-1)*dx*0.5f;
	for(UINT i = 0; i < m; ++i) {
		float z = halfdepth - i*dx;
		for(UINT j = 0; j < n; ++j) {
			float x = -halfwidth + j*dx;
			m_PrevSolution[i*n+j] = XMFLOAT3(x, 0.0f, z);
			m_CurrSolution[i*n+j] = XMFLOAT3(x, 0.0f, z);
			m_Normals[i*n+j]      = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_TangentX[i*n+j]     = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}
//
void simu_wave::update(float dt)
{
	static float t = 0;
	// Accumulate time.
	t += dt;
	// Only update the simulation at the specified time step.
	if (t >= m_TimeStep) {
		// Only update interior points; we use zero boundary conditions.
		for(UINT i = 1; i < m_NumRows-1; ++i) {
			for(UINT j = 1; j < m_NumCols-1; ++j) {
				// After this update we will be discarding the old previous
				// buffer, so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element)
				// because we won't need prev_ij again and the assignment happens last.
				//
				// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				// Moreover, our +z axis goes "down"; this is just to
				// keep consistent with our row indices going down.
				m_PrevSolution[i*m_NumCols+j].y =
					m_K1*m_PrevSolution[i*m_NumCols+j].y+
					m_K2*m_CurrSolution[i*m_NumCols+j].y+
					m_K3*(m_CurrSolution[(i+1)*m_NumCols+j].y+
						m_CurrSolution[(i-1)*m_NumCols+j].y+
						m_CurrSolution[i*m_NumCols+j+1].y+
						m_CurrSolution[i*m_NumCols+j-1].y);
			}
		}
		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(m_PrevSolution, m_CurrSolution);
		t = 0.0f; // reset time
		//
		// Compute normals using finite difference scheme.
		//
		for(UINT i = 1; i < m_NumRows-1; ++i) {
			for(UINT j = 1; j < m_NumCols-1; ++j) {
				float l = m_CurrSolution[i*m_NumCols+j-1].y;
				float r = m_CurrSolution[i*m_NumCols+j+1].y;
				float t = m_CurrSolution[(i-1)*m_NumCols+j].y;
				float b = m_CurrSolution[(i+1)*m_NumCols+j].y;
				m_Normals[i*m_NumCols+j].x = -r+l;
				m_Normals[i*m_NumCols+j].y = 2.0f*m_SpatialStep;
				m_Normals[i*m_NumCols+j].z = b-t;
				XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&m_Normals[i*m_NumCols+j]));
				XMStoreFloat3(&m_Normals[i*m_NumCols+j], n);
				m_TangentX[i*m_NumCols+j] = XMFLOAT3(2.0f*m_SpatialStep, r-l, 0.0f);
				XMVECTOR mt = XMVector3Normalize(XMLoadFloat3(&m_TangentX[i*m_NumCols+j]));
				XMStoreFloat3(&m_TangentX[i*m_NumCols+j], mt);
			}
		}
	}
}
//
void simu_wave::disturb(UINT i, UINT j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < m_NumRows-2);
	assert(j > 1 && j < m_NumCols-2);
	float halfMag = 0.5f*magnitude;
	// disturb the ijth vertex height and its neighbors.
	m_CurrSolution[i*m_NumCols+j].y     += magnitude;
	m_CurrSolution[i*m_NumCols+j+1].y   += halfMag;
	m_CurrSolution[i*m_NumCols+j-1].y   += halfMag;
	m_CurrSolution[(i+1)*m_NumCols+j].y += halfMag;
	m_CurrSolution[(i-1)*m_NumCols+j].y += halfMag;
}
}
#endif