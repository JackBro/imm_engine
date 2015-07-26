////////////////
// simple_secene
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "condition_trigger.h"
#include "phy_attack_box.h"
#include "control_system.h"
#include "imm_cmd.h"
#include "ui_mgr.h"
#include "stru_scene_mgr.h"
#include "stru_draw_instance.h"
#include "ia_render.h"
#include "imm_base.h"
#include "render_shadow.h"
//
#pragma comment(lib, "User32")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "D2d1")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "lua")
#pragma comment(lib, "Xinput1_3")
#pragma comment(lib, "DirectXTKAudioDX")
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
	void update_scene_keydown(float dt);
	void draw_scene();
	void on_mouse_down(WPARAM btn_state, int x, int y);
	void on_mouse_up(WPARAM btn_state, int x, int y);
	void on_mouse_move(WPARAM btn_state, int x, int y);
	void on_mouse_wheel(WPARAM btn_state, int x, int y);
	void on_input_char(WPARAM w_param, LPARAM l_param);
	void on_input_keydown(WPARAM w_param, LPARAM l_param);
	void build_shadow_transform();
	void draw_scene_d3d();
	void draw_scene_d3d_shadow();
	void draw_scene_d3d_slient();
	void draw_scene_d2d();
	//
	static const int m_SMapSize = 2048;
	bool m_IsSyncInterval;
	cmd_shell<imm_app> m_Cmd;
	POINT m_LastMousePos;
	camera m_Cam;
	shadow_map *m_Smap;
	scene_mgr<imm_app> m_Scene;
	instance_mgr<imm_app> m_Inst;
	phy_attack_arrange<imm_app> m_Attack;
	ui_mgr<imm_app> m_UiMgr;
	condition_trigger<imm_app> m_Condition;
	lua_config<imm_app> m_Config;
	control_mov<imm_app> m_Control;
	XMFLOAT4X4 m_LightView;
	XMFLOAT4X4 m_LightProj;
	XMFLOAT4X4 m_ShadowTransform;
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
	m_IsSyncInterval(false),
	m_Cmd(),
	m_Cam(),
	m_Smap(0),
	m_Scene(),
	m_Inst(),
	m_Attack(),
	m_UiMgr(),
	m_Condition(),
	m_Config(this),
	m_Control()
{
	m_WindowName = L"immature engine Demo";
	m_LastMousePos.x = 0;
	m_LastMousePos.y = 0;
	m_Cam.set_Position(0.0f, 7.0f, -25.0f);
}
//
imm_app::~imm_app()
{
	m_D3DDC->ClearState();
	delete m_Smap;
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
	m_Smap = new shadow_map(m_D3DDevice, m_SMapSize, m_SMapSize);
	m_Cam.set_Lens(0.25f*XM_PI, aspect_ratio(), 1.0f, 1000.0f);
	m_UiMgr.init(this);
	m_Cmd.init(this);
	m_Inst.init(this);
	m_Control.init(this);
	m_Scene.init_load(this);
	m_Condition.init(this);
	m_Config.init_additional();
	return true;
}
//
void imm_app::on_resize()
{
	m_Cmd.on_resize();
	m_UiMgr.on_resize();
	base_win<imm_app>::on_resize();
	m_Cam.set_Lens(0.25f*XM_PI, aspect_ratio(), 1.0f, 1000.0f);
	m_Inst.on_resize();
}
//
void imm_app::update_scene(float dt)
{
	m_Scene.update_listen_thread_for_reload();
	// m_UiMgr before m_Cmd, always update, m_Cmd will close m_UiMgr
	m_UiMgr.define_update(dt);
	if (m_Cmd.is_waiting_for_something()) return;
	if (!m_Cmd.is_active) update_scene_keydown(dt);
	// Ensure m_Cmd.is_waiting_for_something()
	if (m_Cmd.is_waiting_for_something()) return;
	build_shadow_transform();
	m_Cam.update_view_matrix();
	m_Condition.update();
	m_Control.update_scene(dt);
	m_Scene.update_atmosphere(dt);
}
//
void imm_app::update_scene_keydown(float dt)
{
	m_Control.update_keydown_and_pad(dt);
}
//
void imm_app::on_mouse_down(WPARAM btn_state, int x, int y)
{
	if (m_Cmd.is_waiting_for_something()) return;
	if (btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0) {
		m_LastMousePos.x = x;
		m_LastMousePos.y = y;
		SetCapture(this->get_hwnd());
	}
	m_Control.on_mouse_down(btn_state, x, y);
}
//
void imm_app::on_mouse_up(WPARAM btn_state, int x, int y)
{
	if (m_Cmd.is_waiting_for_something()) return;
	DUMMY(x);
	DUMMY(y);
	DUMMY(btn_state);
	ReleaseCapture();
}
//
void imm_app::on_mouse_move(WPARAM btn_state, int x, int y)
{
	if (m_Cmd.is_waiting_for_something()) return;
	m_Control.on_mouse_move(btn_state, x, y);
	if (btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0) {
		m_LastMousePos.x = x;
		m_LastMousePos.y = y;
	}
}
//
void imm_app::on_mouse_wheel(WPARAM btn_state, int x, int y)
{
	if (m_Cmd.is_waiting_for_something()) return;
	DUMMY(x);
	DUMMY(y);
	short z_delta = GET_WHEEL_DELTA_WPARAM(btn_state);
	m_Control.on_mouse_wheel(z_delta);
}
//
void imm_app::on_input_char(WPARAM w_param, LPARAM l_param)
{
	if (m_Cmd.is_active) m_Cmd.on_input_char(w_param, l_param);
}
//
void imm_app::on_input_keydown(WPARAM w_param, LPARAM l_param)
{
	m_Cmd.on_input_keydown(w_param, l_param);
	if (m_Cmd.is_waiting_for_something()) return;
	m_Control.on_input_keydown(w_param, l_param);
}
////////////////
// inl
////////////////
////////////////
#include "main_draw.cc"