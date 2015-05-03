////////////////
// terrain
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "cast_sky.h"
#include "cast_particle.h"
#include "cast_terrain.h"
#include "stru_render.h"
#include "imm_camera.h"
#include "imm_base.h"
#include <DirectXColors.h>
#pragma comment(lib, "User32")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "D2d1")
using namespace imm;
//
class imm_app: public base_win<imm_app>
{
public:
	imm_app();
	~imm_app();
	bool init_imm();
	void on_resize();
	void update_scene(float dt);
	void draw_scene();
	void on_mouse_down(WPARAM btn_state, int x, int y);
	void on_mouse_up(WPARAM btn_state, int x, int y);
	void on_mouse_move(WPARAM btn_state, int x, int y);
private:
	lit_dir m_DirLights[3];
	POINT m_LastMousePos;
	camera m_Cam;
	sky *m_Sky;
	terrain m_Terrain;
	bool m_WalkCamMode;
	ID3D11ShaderResourceView* m_FlareTexSRV;
	ID3D11ShaderResourceView* m_RainTexSRV;
	ID3D11ShaderResourceView* m_RandomTexSRV;
	particle m_Fire;
	particle m_Rain;
};
////////////////
//
////////////////
////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	DUMMY(pCmdLine);
	DUMMY(hInstance);
	imm_app win;
	if (!win.init_win()) return 0;
	if (!win.init_d3d()) return 0;
	ShowWindow(win.get_hwnd(), nCmdShow);
	UpdateWindow(win.get_hwnd());
	if (!win.init_imm()) return 0;
	return win.run();
}
////////////////
//
////////////////
////////////////
imm_app::imm_app():
	base_win<imm_app>(),
	//
	m_Cam(),
	m_Sky(0),
	m_Terrain(),
	m_WalkCamMode(false),
	m_FlareTexSRV(0),
	m_RainTexSRV(0),
	m_RandomTexSRV(0),
	m_Fire(),
	m_Rain()
{
	m_WindowName = L"Box Demo";
	m_LastMousePos.x = 0;
	m_LastMousePos.y = 0;
	m_Cam.set_Position(0.0f, 2.0f, 100.0f);
	//
	m_DirLights[0].ambient		= XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[0].diffuse		= XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].specular 	= XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].direction	= XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	m_DirLights[1].ambient		= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[1].diffuse		= XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_DirLights[1].specular 	= XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_DirLights[1].direction	= XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
	m_DirLights[2].ambient		= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].diffuse		= XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].specular 	= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].direction	= XMFLOAT3(0.0f, -0.707f, -0.707f);
}
//
imm_app::~imm_app()
{
	m_D3DDC->ClearState();
	ReleaseCOM(m_FlareTexSRV);
	ReleaseCOM(m_RainTexSRV);
	ReleaseCOM(m_RandomTexSRV);
	delete m_Sky;
	effects::destroy_all();
	input_layouts::destroy_all();
	render::destroy_all();
}
//
bool imm_app::init_imm()
{
	effects::init_all(m_D3DDevice);
	input_layouts::init_all(m_D3DDevice);
	render::init_all(m_D3DDevice);
	//
	std::wstring path_tex(GLOBAL["path_tex"].begin(), GLOBAL["path_tex"].end());
	m_Sky = new sky(m_D3DDevice, path_tex+L"snowcube1024.dds", 5000.0f);
	terrain::init_info tii;
	tii.height_map_filename = path_tex+L"terrain.raw";
	tii.layer_map_filename0 = path_tex+L"grass.dds";
	tii.layer_map_filename1 = path_tex+L"darkdirt.dds";
	tii.layer_map_filename2 = path_tex+L"stone.dds";
	tii.layer_map_filename3 = path_tex+L"lightdirt.dds";
	tii.layer_map_filename4 = path_tex+L"snow.dds";
	tii.blend_map_filename = path_tex+L"blend.dds";
	tii.height_scale = 50.0f;
	tii.heightmap_width = 2049;
	tii.heightmap_height = 2049;
	tii.cell_spacing = 0.5f;
	m_Terrain.init(m_D3DDevice, m_D3DDC, tii);
	//
	m_RandomTexSRV = create_RandomTexture1DSRV(m_D3DDevice);
	std::vector<std::wstring> flares;
	flares.push_back(path_tex+L"flare0.dds");
	m_FlareTexSRV = create_Texture2DArraySRV(m_D3DDevice, m_D3DDC, flares);
	m_Fire.init(m_D3DDevice, effects::m_FireFX, m_FlareTexSRV, m_RandomTexSRV, 500);
	m_Fire.set_EmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));
	std::vector<std::wstring> raindrops;
	raindrops.push_back(path_tex+L"raindrop.dds");
	m_RainTexSRV = create_Texture2DArraySRV(m_D3DDevice, m_D3DDC, raindrops);
	m_Rain.init(m_D3DDevice, effects::m_RainFX, m_RainTexSRV, m_RandomTexSRV, 10000);
	return true;
}
//
void imm_app::on_resize()
{
	base_win<imm_app>::on_resize();
	m_Cam.set_Lens(0.25f*XM_PI, aspect_ratio(), 1.0f, 1000.0f);
}
//
void imm_app::update_scene(float dt)
{
	// Control the camera.
	if(GetAsyncKeyState('W') & 0x8000) m_Cam.walk(10.0f*dt);
	if(GetAsyncKeyState('S') & 0x8000) m_Cam.walk(-10.0f*dt);
	if(GetAsyncKeyState('A') & 0x8000) m_Cam.strafe(-10.0f*dt);
	if(GetAsyncKeyState('D') & 0x8000) m_Cam.strafe(10.0f*dt);
	if(GetAsyncKeyState('Z') & 0x8000) m_Cam.up_down(10.0f*dt);
	if(GetAsyncKeyState('X') & 0x8000) m_Cam.up_down(-10.0f*dt);
	//
	if(GetAsyncKeyState('O') & 0x8000) m_WalkCamMode = true;
	if(GetAsyncKeyState('P') & 0x8000) m_WalkCamMode = false;
	if(m_WalkCamMode) {
		XMFLOAT3 cam_pos = m_Cam.get_Position();
		float y = m_Terrain.get_Height(cam_pos.x, cam_pos.z);
		m_Cam.set_Position(cam_pos.x, y + 3.0f, cam_pos.z);
	}
	//
	if(GetAsyncKeyState('R') & 0x8000) {
		m_Fire.reset();
		m_Rain.reset();
	}
	m_Fire.update(dt, m_Timer.total_time());
	m_Rain.update(dt, m_Timer.total_time());
	m_Cam.update_view_matrix();
}
//
void imm_app::draw_scene()
{
	m_D3DDC->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_D3DDC->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	float blend_factor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	if(GetAsyncKeyState('1') & 0x8000) m_D3DDC->RSSetState(render::m_WireframeRS);
	if(GetAsyncKeyState('2') & 0x8000) m_D3DDC->RSSetState(0);
	// sky
	m_Sky->draw(m_D3DDC, m_Cam);
	// restore default states, as the SkyFX changes them in the effect file.
	m_D3DDC->RSSetState(0);
	m_D3DDC->OMSetDepthStencilState(0, 0);
	m_Terrain.draw(m_D3DDC, m_Cam, m_DirLights);
	// Draw particle systems last so it is blended with scene.
	m_Fire.set_EyePos(m_Cam.get_Position());
	m_Fire.draw(m_D3DDC, m_Cam);
	m_D3DDC->OMSetBlendState(0, blend_factor, 0xffffffff); // restore default
	m_Rain.set_EyePos(m_Cam.get_Position());
	m_Rain.set_EmitPos(m_Cam.get_Position());
	m_Rain.draw(m_D3DDC, m_Cam);
	// restore default states.
	m_D3DDC->RSSetState(0);
	m_D3DDC->OMSetDepthStencilState(0, 0);
	m_D3DDC->OMSetBlendState(0, blend_factor, 0xffffffff);
	HR(m_SwapChain->Present(0, 0));
}
//
void imm_app::on_mouse_down(WPARAM btn_state, int x, int y)
{
	if ((btn_state & MK_LBUTTON) != 0) {
		m_LastMousePos.x = x;
		m_LastMousePos.y = y;
		SetCapture(this->get_hwnd());
	}
}
//
void imm_app::on_mouse_up(WPARAM btn_state, int x, int y)
{
	DUMMY(x);
	DUMMY(y);
	DUMMY(btn_state);
	ReleaseCapture();
}
//
void imm_app::on_mouse_move(WPARAM btn_state, int x, int y)
{
	if((btn_state & MK_LBUTTON) != 0) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));
		m_Cam.pitch(dy);
		m_Cam.rotate_y(dx);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}
//
