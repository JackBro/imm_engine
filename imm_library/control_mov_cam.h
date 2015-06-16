////////////////
// control_mov_cam.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
template <typename T_app>
void control_mov<T_app>::pad_camera_update(const float &dt)
{
	if (pad.is_R_active()) {
		float dx = XMConvertToRadians(50.0f*pad.state.Gamepad.sThumbRX/32767*dt);
		float dy = XMConvertToRadians(50.0f*pad.state.Gamepad.sThumbRY/32767*dt);
		app->m_Cam.pitch(dy);
		app->m_Cam.rotate_y(-dx);
	}
	if (app->m_UiMgr.is_ui_appear()) return;
	if (style_p1 & CONTORL_CAM_FREE) {
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FREE_UP) app->m_Cam.up_down(10.0f*dt);
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FREE_DOWN) app->m_Cam.up_down(-10.0f*dt);
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FREE_LEFT) app->m_Cam.strafe(-10.0f*dt);
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FREE_RIGHT) app->m_Cam.strafe(10.0f*dt);
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FREE_FORWARD) app->m_Cam.walk(10.0f*dt);
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FREE_BACKWARD) app->m_Cam.walk(-10.0f*dt);
	}
	else {
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FOLLOW_FORWARD) cam_follow_walk += 10.0f*dt;
		if (pad.state.Gamepad.wButtons & XGPAD_CAM_FOLLOW_BACKWARD) cam_follow_walk += -10.0f*dt;
		cam_follow_walk = calc_clamp(cam_follow_walk, -50.0f, -10.0f);
		cam_follow_up = cam_follow_walk * (cam_follow_up_def/cam_follow_walk_def);
	}
}
//
template <typename T_app>
void control_mov<T_app>::on_pad_camera_follow(const WORD &vkey)
{
	if (style_p1 & CONTORL_CAM_FREE) return;
	if (vkey == PAD_CAM_FOLLOW_RESET) is_pad_cam_follow_reset = true;
}
//
template <typename T_app>
void control_mov<T_app>::key_camera_free_update(const float &dt)
{
	if (!(style_p1 & CONTORL_CAM_FREE)) return;
	if (GetKeyState(KEY_CAM_FREE_LEFT) & 0x8000) app->m_Cam.strafe(-10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_RIGHT) & 0x8000) app->m_Cam.strafe(10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_UP) & 0x8000) app->m_Cam.up_down(10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_DOWN) & 0x8000) app->m_Cam.up_down(-10.0f*dt);
}
//
template <typename T_app>
void control_mov<T_app>::mouse_camera_wheel(const short &z_delta)
{
	if (style_p1 & CONTORL_CAM_FREE) {
		app->m_Cam.walk(z_delta/120*1.0f);
	}
	else {
		cam_follow_walk += z_delta/120*1.0f;
		cam_follow_walk = calc_clamp(cam_follow_walk, -50.0f, -10.0f);
		cam_follow_up = cam_follow_walk * (cam_follow_up_def/cam_follow_walk_def);
	}
}
//
template <typename T_app>
void control_mov<T_app>::mouse_camera_move(const int &pos_x, const int &pos_y)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(pos_x - app->m_LastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(pos_y - app->m_LastMousePos.y));
	app->m_Cam.pitch(dy);
	app->m_Cam.rotate_y(dx);
	// keep move continuous
	cam_follow_update();
}
//
template <typename T_app>
void control_mov<T_app>::cam_follow_update()
{
	if (player1 < 0 || style_p1 & CONTORL_CAM_FREE) return;
	XMFLOAT4X4 player_world = *(app->m_Inst.m_Stat[player1].get_World());
	XMMATRIX W = XMLoadFloat4x4(&player_world);
	XMVECTOR scale, rot_quat, rot_front_conj, pos, L, U, R;
	XMMatrixDecompose(&scale, &rot_quat, &pos, W);
	// walk, up
	scale = XMVectorReplicate(cam_follow_walk);
	L = XMLoadFloat3(&app->m_Cam.m_Look);
	pos = XMVectorMultiplyAdd(scale, L, pos);
	scale = XMVectorReplicate(cam_follow_up);
	U = XMLoadFloat3(&app->m_Cam.m_Up);
	pos = XMVectorMultiplyAdd(scale, U, pos);
	XMStoreFloat3(&app->m_Cam.m_Position, pos);
	if (GetKeyState(KEY_CAM_FOLLOW_RESET) & 0x8000 || is_pad_cam_follow_reset) {
		is_pad_cam_follow_reset = false;
		// reset
		cam_follow_walk = cam_follow_walk_def;
		if (!map_rot_front_c.count(picked1)) {
			XMMATRIX RF = XMLoadFloat4x4(app->m_Inst.m_Stat[player1].get_RotFront());
			// L as dummy
			XMMatrixDecompose(&scale, &rot_front_conj, &L, RF);
			rot_front_conj = XMQuaternionConjugate(rot_front_conj);
			XMStoreFloat4(&map_rot_front_c[player1], rot_front_conj);
		}
		else {
			rot_front_conj = XMLoadFloat4(&map_rot_front_c[player1]);
		}
		L = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		L = XMVector3Rotate(L, rot_front_conj);
		L = XMVector3Rotate(L, rot_quat);
		L = XMVector3Normalize(L);
		U = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		R = XMVector3Normalize(XMVector3Cross(U, L));
		U = XMVector3Cross(L, R);
		// pitch, rotate cam to default angle, about 0.34f
		XMMATRIX M_R = XMMatrixRotationAxis(R, 0.34f);
		U = XMVector3TransformNormal(U, M_R);
		L = XMVector3TransformNormal(L, M_R);
		XMStoreFloat3(&app->m_Cam.m_Look, L);
		XMStoreFloat3(&app->m_Cam.m_Right, R);
		XMStoreFloat3(&app->m_Cam.m_Up, U);
	}
}
//
