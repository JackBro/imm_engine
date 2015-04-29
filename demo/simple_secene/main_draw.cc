////////////////
// main_draw.cc
////////////////
////////////////
void imm_app::draw_scene()
{
	if (!m_Cmd.is_slient && !m_Cmd.is_loading) draw_scene_d3d();
	else draw_scene_d3d_slient();
	draw_scene_d2d();
	m_SwapChain->Present(0, 0);
	//m_SwapChain->Present1(0, 0, &m_DXGIPresentPara);
}
//
void imm_app::build_shadow_transform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR light_dir = XMLoadFloat3(&m_DirLights[0].direction);
	XMVECTOR light_pos = -2.0f*m_SceneBounds.Radius*light_dir;
	XMVECTOR target_pos = XMLoadFloat3(&m_SceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(light_pos, target_pos, up);
	// Transform bounding sphere to light space.
	XMFLOAT3 sphere_center_ls;
	XMStoreFloat3(&sphere_center_ls, XMVector3TransformCoord(target_pos, V));
	// Ortho frustum in light space encloses scene.
	float l = sphere_center_ls.x - m_SceneBounds.Radius;
	float b = sphere_center_ls.y - m_SceneBounds.Radius;
	float n = sphere_center_ls.z - m_SceneBounds.Radius;
	float r = sphere_center_ls.x + m_SceneBounds.Radius;
	float t = sphere_center_ls.y + m_SceneBounds.Radius;
	float f = sphere_center_ls.z + m_SceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX S = V*P*T;
	XMStoreFloat4x4(&m_LightView, V);
	XMStoreFloat4x4(&m_LightProj, P);
	XMStoreFloat4x4(&m_ShadowTransform, S);
}
//
void imm_app::draw_scene_d3d_shadow()
{
	XMMATRIX view = XMLoadFloat4x4(&m_LightView);
	XMMATRIX proj = XMLoadFloat4x4(&m_LightProj);
	XMMATRIX view_proj = XMMatrixMultiply(view, proj);
	auto fx_shadow = effects::m_BuildShadowMapFX;
	fx_shadow->set_EyePosW(m_Cam.get_Position());
	fx_shadow->set_ViewProj(view_proj);
	ID3DX11EffectTechnique *tech_shadow = fx_shadow->m_BuildShadowMapTech;
	ID3DX11EffectTechnique *tech_shadow_alpha = fx_shadow->m_BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique *tech_shadow_skinned = fx_shadow->m_BuildShadowMapSkinnedTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// draw simple b32
	m_D3DDC->IASetInputLayout(input_layouts::m_Basic32);
	draw_inst_simple_pntt_shadow(
		m_D3DDC,
		tech_shadow,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstB32,
		view_proj
	);
	// draw simple pntt
	m_D3DDC->IASetInputLayout(input_layouts::m_PosNormalTexTan);
	draw_inst_simple_pntt_shadow(
		m_D3DDC,
		tech_shadow,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstPNTT,
		view_proj
	);
	// draw basic 
	m_D3DDC->IASetInputLayout(input_layouts::m_PosNormalTexTan2);
	draw_inst_basic_shadow(
		m_D3DDC,
		tech_shadow,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstBasic,
		false,
		view_proj
	);
	// draw basic alpha
	draw_inst_basic_shadow(
		m_D3DDC,
		tech_shadow_alpha,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstBasicAlpha,
		true,
		view_proj
	);
	// draw skinned
	m_D3DDC->IASetInputLayout(input_layouts::m_PosNormalTexTanSkinned);
	draw_inst_skinned_shadow(
		m_D3DDC,
		tech_shadow_skinned,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstSkinned,
		view_proj
	);
}
//
void imm_app::draw_scene_d3d()
{
	// Render the scene to the shadow map.
	m_Smap->bind_dsv_and_set_null_render_target(m_D3DDC);
	draw_scene_d3d_shadow();
	m_D3DDC->RSSetState(0);
	// Restore the back and depth buffer and viewport to the OM stage.
	ID3D11RenderTargetView* render_targets[1] = {m_RenderTargetView};
	m_D3DDC->OMSetRenderTargets(1, render_targets, m_DepthStencilView);
	m_D3DDC->RSSetViewports(1, &m_ScreenViewport);
	m_D3DDC->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_D3DDC->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	//
	m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	auto *fx_basic = effects::m_BasicFX;
	auto *fx_normal = effects::m_NormalMapFX;
	// Set per frame constants.
	fx_basic->set_DirLights(m_DirLights);
	fx_basic->set_EyePosW(m_Cam.get_Position());
	fx_basic->set_CubeMap(m_Sky->get_CubeMapSRV());
	fx_basic->set_ShadowMap(m_Smap->get_DepthMapSRV());
	fx_normal->set_DirLights(m_DirLights);
	fx_normal->set_EyePosW(m_Cam.get_Position());
	fx_normal->set_CubeMap(m_Sky->get_CubeMapSRV());
	fx_normal->set_ShadowMap(m_Smap->get_DepthMapSRV());
	m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3DX11EffectTechnique *tech_basic = fx_normal->m_Light3TexTech;
	ID3DX11EffectTechnique *tech_basic_alpha = fx_normal->m_Light3TexAlphaClipTech;
	ID3DX11EffectTechnique *tech_simple_b32 = fx_basic->m_Light3Tech;
	ID3DX11EffectTechnique *tech_skinned = fx_normal->m_Light3TexSkinnedTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	XMMATRIX view_proj = m_Cam.get_ViewProj();	
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX to_tex_space(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX shadow_transform = XMLoadFloat4x4(&m_ShadowTransform);
	// draw simple b32
	m_D3DDC->IASetInputLayout(input_layouts::m_Basic32);
	draw_inst_simple_b32(
		m_D3DDC,
		tech_simple_b32,
		tech_desc,
		fx_basic,
		m_Inst.m_Model.m_InstB32,
		view_proj,
		shadow_transform
	);
	// draw simple pntt
	m_D3DDC->IASetInputLayout(input_layouts::m_PosNormalTexTan);
	draw_inst_simple_pntt(
		m_D3DDC,
		tech_basic,
		tech_simple_b32,
		tech_desc,
		fx_normal,
		fx_basic,
		m_Inst.m_Model.m_InstPNTT,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// draw basic
	m_D3DDC->IASetInputLayout(input_layouts::m_PosNormalTexTan2);
	draw_inst_basic(
		m_D3DDC,
		tech_basic,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstBasic,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// draw basic alpha
	m_D3DDC->RSSetState(render::m_NoCullRS);
	draw_inst_basic(
		m_D3DDC,
		tech_basic_alpha,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstBasicAlpha,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// draw skkined
	m_D3DDC->IASetInputLayout(input_layouts::m_PosNormalTexTanSkinned);
	draw_inst_skinned(
		m_D3DDC,
		tech_skinned,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstSkinned,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// sky
	m_Sky->draw(m_D3DDC, m_Cam);
	// restore default states, as the SkyFX changes them in the effect file.
	m_D3DDC->RSSetState(0);
	m_D3DDC->OMSetDepthStencilState(0, 0);
	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// to it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView *null_srv[16] = {0};
	m_D3DDC->PSSetShaderResources(0, 16, null_srv);
}
//
void imm_app::draw_scene_d3d_slient()
{
	assert(m_D3DDC);
	assert(m_SwapChain);
	m_D3DDC->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&m_Cmd.back_color));
	m_D3DDC->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}
//
void imm_app::draw_scene_d2d()
{
	assert(m_D2DDC);
	m_D2DDC->BeginDraw();
	m_D2DDC->SetTransform(D2D1::IdentityMatrix());
	m_Cmd.draw();
	m_UI.draw();
	HR(m_D2DDC->EndDraw());
}
