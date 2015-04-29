////////////////
// control_mov_cam.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
template <typename T_app>
void control_mov<T_app>::pad_camera_free(const float &dt)
{
	if (pad.is_R_active()) {
		float dx = XMConvertToRadians(50.0f*pad.state.Gamepad.sThumbRX/32767*dt);
		float dy = XMConvertToRadians(50.0f*pad.state.Gamepad.sThumbRY/32767*dt);
		app->m_Cam.pitch(-dy);
		app->m_Cam.rotate_y(dx);
	}
	if (app->m_UI.is_ui_appear()) return;	
	if (pad.state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) app->m_Cam.up_down(10.0f*dt);
	if (pad.state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) app->m_Cam.up_down(-10.0f*dt);
	if (pad.state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) app->m_Cam.strafe(-10.0f*dt);
	if (pad.state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) app->m_Cam.strafe(10.0f*dt);
	if (pad.state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) app->m_Cam.walk(10.0f*dt);
	if (pad.state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) app->m_Cam.walk(-10.0f*dt);
}
//
template <typename T_app>
void control_mov<T_app>::key_camera_free(const float &dt)
{
	if (GetAsyncKeyState('A') & 0x8000) app->m_Cam.strafe(-10.0f*dt);
	if (GetAsyncKeyState('D') & 0x8000) app->m_Cam.strafe(10.0f*dt);
	if (GetAsyncKeyState('W') & 0x8000) app->m_Cam.up_down(10.0f*dt);
	if (GetAsyncKeyState('S') & 0x8000) app->m_Cam.up_down(-10.0f*dt);
}
//
template <typename T_app>
void control_mov<T_app>::mouse_camera_wheel(const short &z_delta)
{
	app->m_Cam.walk(z_delta/120*1.0f);
}
//
template <typename T_app>
void control_mov<T_app>::mouse_camera_move(const float &dx, const float &dy)
{
	app->m_Cam.pitch(dy);
	app->m_Cam.rotate_y(dx);
}
//
template <typename T_app>
void control_mov<T_app>::cam_follow()
{
	if (picked < 0) return;
	
	XMFLOAT4X4 player_world = *(app->m_Inst.m_Stat[picked].get_World());
	XMMATRIX W = XMLoadFloat4x4(&player_world);
	XMVECTOR scale, rot_quat, translation, rot_front_conj, pos;
	XMMatrixDecompose(&scale, &rot_quat, &pos, W);
	
	
	
	// walk, up_down
	float walk = -30.0f;
	float up_down = 5.0f;
	XMVECTOR s = XMVectorReplicate(walk);
	XMVECTOR l = XMLoadFloat3(&app->m_Cam.m_Look);
	pos = XMVectorMultiplyAdd(s, l, pos);
	s = XMVectorReplicate(up_down);
	l = XMLoadFloat3(&app->m_Cam.m_Up);
	pos = XMVectorMultiplyAdd(s, l, pos);
	XMStoreFloat3(&app->m_Cam.m_Position, pos);
	
	
	
	
	
	if (GetAsyncKeyState('Z')) {
		
		map_rot_front_j.count(picked);
		
		XMFLOAT4X4 &rot_front = *app->m_Inst.m_Stat[picked].get_RotFront();
		XMMATRIX RF = XMLoadFloat4x4(&rot_front);
		XMMatrixDecompose(&scale, &rot_front_conj, &translation, RF);
		rot_front_conj = XMQuaternionConjugate(rot_front_conj);
		
		
		
		XMVECTOR L = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		L = XMVector3Rotate(L, rot_front_conj);
		L = XMVector3Rotate(L, rot_quat);
		L = XMVector3Normalize(L);
		XMVECTOR world_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(world_up, L));
		XMVECTOR U = XMVector3Cross(L, R);
		
		
		
		
		
		XMStoreFloat3(&app->m_Cam.m_Look, L);
		XMStoreFloat3(&app->m_Cam.m_Right, R);
		XMStoreFloat3(&app->m_Cam.m_Up, U);
		
		
		
	}
	
	
	
	
	
	
	
	
}