////////////////
// control_mov_math.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
template <typename T_app>
void control_mov<T_app>::math_mouse_move_toward_hit(
	CXMVECTOR &plane_pos,
	const size_t &index,
	const float &speed = -1.0f)
{
	XMFLOAT4X4 &world = *app->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &rot_front = *app->m_Inst.m_Stat[index].get_RotFront();
	XMFLOAT3 *phy_velocity_nm = &app->m_Inst.m_Stat[index].phy.velocity_nm;
	XMVECTOR velocity_nm = XMLoadFloat3(phy_velocity_nm);
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	velocity_nm = XMVectorSubtract(plane_pos, W.r[3]);
	velocity_nm = XMVectorSetY(velocity_nm, 0.0f);
	velocity_nm = XMVector3Normalize(velocity_nm);
	// use velocity_nm as front direction
	math_mouse_face_rot_y(W, RF, velocity_nm);
	// when update stop, use object own speed
	if (speed < 0.0f) velocity_nm = XMVectorScale(velocity_nm, motion.speed);
	else velocity_nm = XMVectorScale(velocity_nm, speed);
	XMStoreFloat3(phy_velocity_nm, velocity_nm);
	XMStoreFloat4x4(&world, W);
}
//
template <typename T_app>
void control_mov<T_app>::math_mouse_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	CXMVECTOR &direction)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	float rot_radians = atan2(-XMVectorGetX(direction), -XMVectorGetZ(direction));
	R = XMMatrixRotationY(rot_radians);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
template <typename T_app>
void control_mov<T_app>::math_pad_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	XMVECTOR &direction,
	const float &rot_cam)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	// rot_quat uses as temp var
	float rot_pad = pad.L_radians();
	rot_quat =  XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rot_pad);
	direction = XMVector3Rotate(direction, rot_quat);
	//
	R = XMMatrixRotationY(rot_cam);
	R = XMMatrixMultiply(XMMatrixRotationY(rot_pad), R);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
template <typename T_app>
void control_mov<T_app>::math_mouse_hit_plane_y(
	const int &pos_x,
	const int &pos_y,
	XMVECTOR &plane_pos_out)
{
	CXMMATRIX cam_proj = app->m_Cam.get_Proj();
	CXMMATRIX cam_view = app->m_Cam.get_View();
	// Compute picking ray in view space.
	float vx = (+2.0f*pos_x/app->m_ClientWidth - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*pos_y/app->m_ClientHeight + 1.0f)/cam_proj.r[1].m128_f32[1];
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
	// Ray hit y=value plane at plane_p
	XMFLOAT4X4 *world = app->m_Inst.m_Stat[app->m_Inst.m_PlaneGroundIx].get_World();
	float plane_y = world->_42;
	plane_pos_out = ray_origin;
	float ratio_y = -XMVectorGetY(ray_origin)/XMVectorGetY(ray_dir)+(plane_y/XMVectorGetY(ray_dir));
	plane_pos_out = XMVectorSetY(plane_pos_out, plane_y);
	plane_pos_out = XMVectorSetX(plane_pos_out, XMVectorGetX(ray_origin)+ratio_y*XMVectorGetX(ray_dir));
	plane_pos_out = XMVectorSetZ(plane_pos_out, XMVectorGetZ(ray_origin)+ratio_y*XMVectorGetZ(ray_dir));
}
//
template <typename T_app>
void control_mov<T_app>::math_mouse_hit_terrain(
	const int &pos_x,
	const int &pos_y,
	XMVECTOR &plane_pos_out)
{
	CXMMATRIX cam_proj = app->m_Cam.get_Proj();
	CXMMATRIX cam_view = app->m_Cam.get_View();
	// Compute picking ray in view space.
	float vx = (+2.0f*pos_x/app->m_ClientWidth - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*pos_y/app->m_ClientHeight + 1.0f)/cam_proj.r[1].m128_f32[1];
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
	// Ray hit y=value plane at plane_p
	plane_pos_out = ray_origin;
	float plane_y = app->m_Scene.terrain1.m_Info.height_scale;
	float p_step = plane_y / 50.0f;
	while (plane_y > 0.0f) {
		float ratio_y = -XMVectorGetY(ray_origin)/XMVectorGetY(ray_dir)+(plane_y/XMVectorGetY(ray_dir));
		plane_pos_out = XMVectorSetY(plane_pos_out, plane_y);
		plane_pos_out = XMVectorSetX(plane_pos_out, XMVectorGetX(ray_origin)+ratio_y*XMVectorGetX(ray_dir));
		plane_pos_out = XMVectorSetZ(plane_pos_out, XMVectorGetZ(ray_origin)+ratio_y*XMVectorGetZ(ray_dir));
		float height = app->m_Scene.terrain1.get_Height(XMVectorGetX(plane_pos_out), XMVectorGetZ(plane_pos_out));
		if (plane_y < height) return;
		plane_y -= p_step;
	}
	plane_pos_out = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}
//
template <typename T_app>
void control_mov<T_app>::math_pad_move_toward()
{
	XMVECTOR velocity_nm = app->m_Cam.get_LookXM();
	float cam_radians = atan2(-XMVectorGetX(velocity_nm), -XMVectorGetZ(velocity_nm));
	XMFLOAT4X4 &world = *(app->m_Inst.m_Stat[player1].get_World());
	XMFLOAT4X4 &rot_front = *(app->m_Inst.m_Stat[player1].get_RotFront());
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	math_pad_face_rot_y(W, RF, velocity_nm, cam_radians);
	velocity_nm = XMVectorSetY(velocity_nm, 0.0f);
	velocity_nm = XMVector3Normalize(velocity_nm);
	velocity_nm = XMVectorScale(velocity_nm, motion.speed);
	XMStoreFloat4x4(&world, W);
	XMStoreFloat3(&app->m_Inst.m_Stat[player1].phy.velocity_nm, velocity_nm);
	app->m_Inst.m_Stat[player1].check_set_ClipName(motion.walk_run);
}
//
