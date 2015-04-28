////////////////
// control_mov_cam.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
template <typename T_app>
void control_mov<T_app>::cam_follow()
{
	
	if (picked < 0) return;
	
	XMFLOAT4X4 player_world = *(app->m_Inst.m_Stat[picked].get_World());
	
	XMMATRIX W = XMLoadFloat4x4(&player_world);
	
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	
	XMFLOAT4X4 &rot_front = *app->m_Inst.m_Stat[picked].get_RotFront();
	
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	
	
	
	//app->m_Cam.set_Position();
	//app->m_Cam.set_Position(player_world._41, player_world._42, player_world._43);
	
	
	
	
	
}