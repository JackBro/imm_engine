////////////////
// mesh_texture_mgr.h
// from 3D Game Programming with DirectX 11 by Frank Luna, TextureMgr.h
////////////////
////////////////
#ifndef MESH_TEXTURE_MGR_H
#define MESH_TEXTURE_MGR_H
#include "imm_core.h"
#include "DDSTextureLoader.h"
namespace imm
{
////////////////
// basic_model
// Simple texture manager to avoid loading duplicate textures from file.  That can
// happen, for example, if multiple meshes reference the same texture filename.
////////////////
////////////////
class texture_mgr
{
public:
	texture_mgr();
	~texture_mgr();
	void init(ID3D11Device *device);
	ID3D11ShaderResourceView *create_texture(std::wstring filename);
private:
	texture_mgr(const texture_mgr &rhs);
	texture_mgr &operator=(const texture_mgr &rhs);
	ID3D11Device *m_D3DDevice;
	std::map<std::wstring, ID3D11ShaderResourceView*> m_TextureSRV;
};
//
texture_mgr::texture_mgr():
	m_D3DDevice(0)
{
}
//
texture_mgr::~texture_mgr()
{
	for (auto it = m_TextureSRV.begin(); it != m_TextureSRV.end(); ++it) ReleaseCOM(it->second);
	m_TextureSRV.clear();
}
//
void texture_mgr::init(ID3D11Device *device) {m_D3DDevice = device;}
//
ID3D11ShaderResourceView *texture_mgr::create_texture(std::wstring filename)
{
	ID3D11ShaderResourceView *srv = 0;
	// Does it already exist?
	if (m_TextureSRV.find(filename) != m_TextureSRV.end()) srv = m_TextureSRV[filename];
	else {
		HRESULT hr = CreateDDSTextureFromFile(m_D3DDevice, filename.c_str(), 0, &srv, 0);
		if (FAILED(hr)) {
			std::wostringstream os;
			os << "Texture file load error: " << filename << "\nHRESULT: " << hr;
			ERROR_MES(os.str().c_str());
		}
		m_TextureSRV[filename] = srv;
	}
	return srv;
}
}
#endif