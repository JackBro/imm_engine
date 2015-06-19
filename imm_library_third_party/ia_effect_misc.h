////////////////
// ia_effect_misc.h
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
////////////////
////////////////
#ifndef IA_EFFECT_MISC_H
#define IA_EFFECT_MISC_H
#include "imm_core.h"
namespace imm
{
////////////////
// sky_effect
////////////////
////////////////
class sky_effect: public effect
{
public:
	sky_effect(ID3D11Device *device, const std::wstring &filename);
	~sky_effect() {;}
	void set_WorldViewProj(CXMMATRIX M) 				{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_CubeMap(ID3D11ShaderResourceView *cubemap) {m_CubeMap->SetResource(cubemap);}
	ID3DX11EffectTechnique *m_SkyTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
};
//
sky_effect::sky_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_SkyTech = m_FX->GetTechniqueByName("SkyTech");
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_CubeMap		= m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
}
////////////////
// terrain_effect
////////////////
////////////////
class terrain_effect: public effect
{
public:
	terrain_effect(ID3D11Device *device, const std::wstring &filename);
	~terrain_effect() {;}
	void set_ViewProj(CXMMATRIX M)							{m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_ShadowTransform(CXMMATRIX M)					{m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v) 					{m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const lit_dir *lights)				{m_DirLights->SetRawValue(lights, 0, 3*sizeof(lit_dir));}
	void set_Material(const material &mat)					{m_Mat->SetRawValue(&mat, 0, sizeof(material));}
	void set_MinDist(float f)								{m_MinDist->SetFloat(f);}
	void set_MaxDist(float f)								{m_MaxDist->SetFloat(f);}
	void set_MinTess(float f)								{m_MinTess->SetFloat(f);}
	void set_MaxTess(float f)								{m_MaxTess->SetFloat(f);}
	void set_TexelCellSpaceU(float f)						{m_TexelCellSpaceU->SetFloat(f);}
	void set_TexelCellSpaceV(float f)						{m_TexelCellSpaceV->SetFloat(f);}
	void set_WorldCellSpace(float f)						{m_WorldCellSpace->SetFloat(f);}
	void set_WorldFrustumPlanes(XMFLOAT4 planes[6]) 		{m_WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6);}
	void set_LayerMapArray(ID3D11ShaderResourceView *tex)	{m_LayerMapArray->SetResource(tex);}
	void set_BlendMap(ID3D11ShaderResourceView *tex)		{m_BlendMap->SetResource(tex);}
	void set_HeightMap(ID3D11ShaderResourceView *tex)		{m_HeightMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)		{m_ShadowMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectScalarVariable *m_ScreenWidth;
	ID3DX11EffectScalarVariable *m_ScreenHeight;
	ID3DX11EffectScalarVariable *m_MinDist;
	ID3DX11EffectScalarVariable *m_MaxDist;
	ID3DX11EffectScalarVariable *m_MinTess;
	ID3DX11EffectScalarVariable *m_MaxTess;
	ID3DX11EffectScalarVariable *m_TexelCellSpaceU;
	ID3DX11EffectScalarVariable *m_TexelCellSpaceV;
	ID3DX11EffectScalarVariable *m_WorldCellSpace;
	ID3DX11EffectVectorVariable *m_WorldFrustumPlanes;
	ID3DX11EffectShaderResourceVariable *m_LayerMapArray;
	ID3DX11EffectShaderResourceVariable *m_BlendMap;
	ID3DX11EffectShaderResourceVariable *m_HeightMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
};
//
terrain_effect::terrain_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_Light3Tech = m_FX->GetTechniqueByName("Light3");
	m_ViewProj				= m_FX->GetVariableByName("gViewProj")->AsMatrix();
	m_ShadowTransform		= m_FX->GetVariableByName("gShadowTransform")->AsMatrix();
	m_EyePosW				= m_FX->GetVariableByName("gEyePosW")->AsVector();
	m_DirLights 			= m_FX->GetVariableByName("gDirLights");
	m_Mat					= m_FX->GetVariableByName("gMaterial");
	m_MinDist				= m_FX->GetVariableByName("gMinDist")->AsScalar();
	m_MaxDist				= m_FX->GetVariableByName("gMaxDist")->AsScalar();
	m_MinTess				= m_FX->GetVariableByName("gMinTess")->AsScalar();
	m_MaxTess				= m_FX->GetVariableByName("gMaxTess")->AsScalar();
	m_TexelCellSpaceU		= m_FX->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	m_TexelCellSpaceV		= m_FX->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	m_WorldCellSpace		= m_FX->GetVariableByName("gWorldCellSpace")->AsScalar();
	m_WorldFrustumPlanes	= m_FX->GetVariableByName("gWorldFrustumPlanes")->AsVector();
	m_LayerMapArray 		= m_FX->GetVariableByName("gLayerMapArray")->AsShaderResource();
	m_BlendMap				= m_FX->GetVariableByName("gBlendMap")->AsShaderResource();
	m_HeightMap 			= m_FX->GetVariableByName("gHeightMap")->AsShaderResource();
	m_ShadowMap 		= m_FX->GetVariableByName("gShadowMap")->AsShaderResource();
}
////////////////
// particle_effect
////////////////
////////////////
class particle_effect: public effect
{
public:
	particle_effect(ID3D11Device *device, const std::wstring &filename);
	~particle_effect() {;}
	void set_ViewProj(CXMMATRIX M)						 {m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_GameTime(float f)							 {m_GameTime->SetFloat(f);}
	void set_TimeStep(float f)							 {m_TimeStep->SetFloat(f);}
	void set_EyePosW(const XMFLOAT3 &v) 				 {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_EmitPosW(const XMFLOAT3 &v)				 {m_EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_EmitDirW(const XMFLOAT3 &v)				 {m_EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_TexArray(ID3D11ShaderResourceView *tex)	 {m_TexArray->SetResource(tex);}
	void set_RandomTex(ID3D11ShaderResourceView *tex)	 {m_RandomTex->SetResource(tex);}
	ID3DX11EffectTechnique *m_StreamOutTech;
	ID3DX11EffectTechnique *m_DrawTech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectScalarVariable *m_GameTime;
	ID3DX11EffectScalarVariable *m_TimeStep;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVectorVariable *m_EmitPosW;
	ID3DX11EffectVectorVariable *m_EmitDirW;
	ID3DX11EffectShaderResourceVariable *m_TexArray;
	ID3DX11EffectShaderResourceVariable *m_RandomTex;
};
//
particle_effect::particle_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_StreamOutTech = m_FX->GetTechniqueByName("StreamOutTech");
	m_DrawTech		= m_FX->GetTechniqueByName("DrawTech");
	m_ViewProj	= m_FX->GetVariableByName("gViewProj")->AsMatrix();
	m_GameTime	= m_FX->GetVariableByName("gGameTime")->AsScalar();
	m_TimeStep	= m_FX->GetVariableByName("gTimeStep")->AsScalar();
	m_EyePosW	= m_FX->GetVariableByName("gEyePosW")->AsVector();
	m_EmitPosW	= m_FX->GetVariableByName("gEmitPosW")->AsVector();
	m_EmitDirW	= m_FX->GetVariableByName("gEmitDirW")->AsVector();
	m_TexArray	= m_FX->GetVariableByName("gTexArray")->AsShaderResource();
	m_RandomTex = m_FX->GetVariableByName("gRandomTex")->AsShaderResource();
}
////////////////
// debug_tex_effect
////////////////
////////////////
class debug_tex_effect: public effect
{
public:
	debug_tex_effect(ID3D11Device *device, const std::wstring &filename);
	~debug_tex_effect() {;}
	void set_WorldViewProj(CXMMATRIX M)				{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_Texture(ID3D11ShaderResourceView *tex)	{m_Texture->SetResource(tex);}
	ID3DX11EffectTechnique *m_ViewArgbTech;
	ID3DX11EffectTechnique *m_ViewRedTech;
	ID3DX11EffectTechnique *m_ViewGreenTech;
	ID3DX11EffectTechnique *m_ViewBlueTech;
	ID3DX11EffectTechnique *m_ViewAlphaTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectShaderResourceVariable *m_Texture;
};
//
debug_tex_effect::debug_tex_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_ViewArgbTech	= m_FX->GetTechniqueByName("ViewArgbTech");
	m_ViewRedTech	= m_FX->GetTechniqueByName("ViewRedTech");
	m_ViewGreenTech	= m_FX->GetTechniqueByName("ViewGreenTech");
	m_ViewBlueTech	= m_FX->GetTechniqueByName("ViewBlueTech");
	m_ViewAlphaTech	= m_FX->GetTechniqueByName("ViewAlphaTech");
	m_WorldViewProj	= m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_Texture		= m_FX->GetVariableByName("gTexture")->AsShaderResource();
}
////////////////
// color_effect
////////////////
////////////////
class color_effect: public effect
{
public:
	color_effect(ID3D11Device *device, const std::wstring &filename);
	~color_effect() {;}
	void set_WorldViewProj(CXMMATRIX M) {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	ID3DX11EffectTechnique *m_ColorTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
};
//
color_effect::color_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_ColorTech		= m_FX->GetTechniqueByName("ColorTech");
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
}
//
}
#endif