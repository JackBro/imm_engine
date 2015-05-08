////////////////
// ia_effect.h
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
////////////////
////////////////
#ifndef IA_EFFECT_H
#define IA_EFFECT_H
#include "imm_core.h"
#include "ia_effect_misc.h"
#include "ia_effect_ssao.h"
namespace imm
{
////////////////
// basic_effect
////////////////
////////////////
class basic_effect: public effect
{
public:
	basic_effect(ID3D11Device *device, const std::wstring &filename);
	~basic_effect() {;}
	void set_WorldViewProj(CXMMATRIX m) 				{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_WorldViewProjTex(CXMMATRIX M)				{m_WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX m) 						{m_World->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_WorldInvTranspose(CXMMATRIX m) 			{m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_ShadowTransform(CXMMATRIX M)				{m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX m)					{m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_EyePosW(const XMFLOAT3 &v) 				{m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const imm::lit_dir *lights)		{m_DirLights->SetRawValue(lights, 0, 3*sizeof(imm::lit_dir));}
	void set_Material(const imm::material &mat)			{m_Mat->SetRawValue(&mat, 0, sizeof(imm::material));}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)	{m_DiffuseMap->SetResource(tex);}
	void set_CubeMap(ID3D11ShaderResourceView *tex) 	{m_CubeMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)	{m_ShadowMap->SetResource(tex);}
	void set_SsaoMap(ID3D11ShaderResourceView* tex) 	{m_SsaoMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectTechnique *m_Light3TexTech;
	ID3DX11EffectTechnique *m_Light3TexAlphaClipTech;
	ID3DX11EffectTechnique *m_Light3ReflectTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProjTex;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
	ID3DX11EffectShaderResourceVariable *m_SsaoMap;
};
//
basic_effect::basic_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_Light3Tech				= m_FX->GetTechniqueByName("Light3");
	m_Light3TexTech 			= m_FX->GetTechniqueByName("Light3Tex");
	m_Light3TexAlphaClipTech	= m_FX->GetTechniqueByName("Light3TexAlphaClip");
	m_Light3ReflectTech 		= m_FX->GetTechniqueByName("Light3Reflect");
	m_WorldViewProj 	= m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_WorldViewProjTex	= m_FX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	m_World 			= m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_ShadowTransform	= m_FX->GetVariableByName("gShadowTransform")->AsMatrix();
	m_TexTransform		= m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_EyePosW			= m_FX->GetVariableByName("gEyePosW")->AsVector();
	m_DirLights 		= m_FX->GetVariableByName("gDirLights");
	m_Mat				= m_FX->GetVariableByName("gMaterial");
	m_DiffuseMap		= m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_CubeMap			= m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
	m_ShadowMap 		= m_FX->GetVariableByName("gShadowMap")->AsShaderResource();
	m_SsaoMap			= m_FX->GetVariableByName("gSsaoMap")->AsShaderResource();
}
////////////////
// normal_map_effect
////////////////
////////////////
class normal_map_effect: public effect
{
public:
	normal_map_effect(ID3D11Device *device, const std::wstring &filename);
	~normal_map_effect() {;}
	void set_WorldViewProj(CXMMATRIX M) 					{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldViewProjTex(CXMMATRIX M)					{m_WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX M) 							{m_World->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTranspose(CXMMATRIX M) 				{m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_BoneTransforms(const XMFLOAT4X4 *M, int cnt)	{m_BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);}
	void set_ShadowTransform(CXMMATRIX M)					{m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX M)						{m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v) 					{m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const imm::lit_dir *lights)			{m_DirLights->SetRawValue(lights, 0, 3*sizeof(imm::lit_dir));}
	void set_Material(const imm::material &mat)				{m_Mat->SetRawValue(&mat, 0, sizeof(imm::material));}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)		{m_DiffuseMap->SetResource(tex);}
	void set_CubeMap(ID3D11ShaderResourceView *tex) 		{m_CubeMap->SetResource(tex);}
	void set_NormalMap(ID3D11ShaderResourceView *tex)		{m_NormalMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)		{m_ShadowMap->SetResource(tex);}
	void set_SsaoMap(ID3D11ShaderResourceView* tex) 		{m_SsaoMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectTechnique *m_Light3TexTech;
	ID3DX11EffectTechnique *m_Light3TexAlphaClipTech;
	ID3DX11EffectTechnique *m_Light3ReflectTech;
	ID3DX11EffectTechnique *m_Light3TexSkinnedTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProjTex;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_BoneTransforms;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
	ID3DX11EffectShaderResourceVariable *m_NormalMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
	ID3DX11EffectShaderResourceVariable *m_SsaoMap;
};
//
normal_map_effect::normal_map_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_Light3Tech				= m_FX->GetTechniqueByName("Light3");
	m_Light3TexTech 			= m_FX->GetTechniqueByName("Light3Tex");
	m_Light3TexAlphaClipTech	= m_FX->GetTechniqueByName("Light3TexAlphaClip");
	m_Light3ReflectTech 		= m_FX->GetTechniqueByName("Light3Reflect");
	m_Light3TexSkinnedTech		= m_FX->GetTechniqueByName("Light3TexSkinned");
	m_WorldViewProj 	= m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_WorldViewProjTex	= m_FX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	m_World 			= m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_WorldInvTranspose	= m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_BoneTransforms	= m_FX->GetVariableByName("gBoneTransforms")->AsMatrix();
	m_ShadowTransform	= m_FX->GetVariableByName("gShadowTransform")->AsMatrix();
	m_TexTransform		= m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_EyePosW			= m_FX->GetVariableByName("gEyePosW")->AsVector();
	m_DirLights 		= m_FX->GetVariableByName("gDirLights");
	m_Mat				= m_FX->GetVariableByName("gMaterial");
	m_DiffuseMap		= m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_CubeMap			= m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
	m_NormalMap 		= m_FX->GetVariableByName("gNormalMap")->AsShaderResource();
	m_ShadowMap 		= m_FX->GetVariableByName("gShadowMap")->AsShaderResource();
	m_SsaoMap			= m_FX->GetVariableByName("gSsaoMap")->AsShaderResource();
}
////////////////
// displacement_map_effect
////////////////
////////////////
class displacement_map_effect: public effect
{
public:
	displacement_map_effect(ID3D11Device *device, const std::wstring &filename);
	~displacement_map_effect() {;}
	void set_ViewProj(CXMMATRIX M)						{m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldViewProj(CXMMATRIX M) 				{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX M) 						{m_World->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTranspose(CXMMATRIX M) 			{m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_ShadowTransform(CXMMATRIX M)				{m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX M)					{m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v) 				{m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const imm::lit_dir *lights)		{m_DirLights->SetRawValue(lights, 0, 3*sizeof(imm::lit_dir));}
	void set_Mat(const imm::material &mat)				{m_Mat->SetRawValue(&mat, 0, sizeof(imm::material));}
	void set_HeightScale(float f)						{m_HeightScale->SetFloat(f);}
	void set_MaxTessDistance(float f)					{m_MaxTessDistance->SetFloat(f);}
	void set_MinTessDistance(float f)					{m_MinTessDistance->SetFloat(f);}
	void set_MinTessFactor(float f) 					{m_MinTessFactor->SetFloat(f);}
	void set_MaxTessFactor(float f) 					{m_MaxTessFactor->SetFloat(f);}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)	{m_DiffuseMap->SetResource(tex);}
	void set_CubeMap(ID3D11ShaderResourceView *tex) 	{m_CubeMap->SetResource(tex);}
	void set_NormalMap(ID3D11ShaderResourceView *tex)	{m_NormalMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)	{m_ShadowMap->SetResource(tex);}
	void set_SsaoMap(ID3D11ShaderResourceView* tex) 	{m_SsaoMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectTechnique *m_Light3TexTech;
	ID3DX11EffectTechnique *m_Light3TexAlphaClipTech;
	ID3DX11EffectTechnique *m_Light3ReflectTech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectScalarVariable *m_HeightScale;
	ID3DX11EffectScalarVariable *m_MaxTessDistance;
	ID3DX11EffectScalarVariable *m_MinTessDistance;
	ID3DX11EffectScalarVariable *m_MinTessFactor;
	ID3DX11EffectScalarVariable *m_MaxTessFactor;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
	ID3DX11EffectShaderResourceVariable *m_NormalMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
	ID3DX11EffectShaderResourceVariable *m_SsaoMap;
};
//
displacement_map_effect::displacement_map_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_Light3Tech				= m_FX->GetTechniqueByName("Light3");
	m_Light3TexTech 			= m_FX->GetTechniqueByName("Light3Tex");
	m_Light3TexAlphaClipTech	= m_FX->GetTechniqueByName("Light3TexAlphaClip");
	m_Light3ReflectTech 		= m_FX->GetTechniqueByName("Light3Reflect");
	m_ViewProj			= m_FX->GetVariableByName("gViewProj")->AsMatrix();
	m_WorldViewProj 	= m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_World 			= m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_WorldInvTranspose	= m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_ShadowTransform	= m_FX->GetVariableByName("gShadowTransform")->AsMatrix();
	m_TexTransform		= m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_EyePosW			= m_FX->GetVariableByName("gEyePosW")->AsVector();
	m_DirLights 		= m_FX->GetVariableByName("gDirLights");
	m_Mat				= m_FX->GetVariableByName("gMaterial");
	m_HeightScale		= m_FX->GetVariableByName("gHeightScale")->AsScalar();
	m_MaxTessDistance	= m_FX->GetVariableByName("gMaxTessDistance")->AsScalar();
	m_MinTessDistance	= m_FX->GetVariableByName("gMinTessDistance")->AsScalar();
	m_MinTessFactor 	= m_FX->GetVariableByName("gMinTessFactor")->AsScalar();
	m_MaxTessFactor 	= m_FX->GetVariableByName("gMaxTessFactor")->AsScalar();
	m_DiffuseMap		= m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_CubeMap			= m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
	m_NormalMap 		= m_FX->GetVariableByName("gNormalMap")->AsShaderResource();
	m_ShadowMap 		= m_FX->GetVariableByName("gShadowMap")->AsShaderResource();
	m_SsaoMap			= m_FX->GetVariableByName("gSsaoMap")->AsShaderResource();
}
////////////////
// build_shadow_map_effect
////////////////
////////////////
class build_shadow_map_effect: public effect
{
public:
	build_shadow_map_effect(ID3D11Device *device, const std::wstring &filename);
	~build_shadow_map_effect() {;}
	void set_ViewProj(CXMMATRIX M)							{m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldViewProj(CXMMATRIX M) 					{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX M) 							{m_World->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTranspose(CXMMATRIX M) 				{m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_BoneTransforms(const XMFLOAT4X4 *M, int cnt)	{m_BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);}
	void set_TexTransform(CXMMATRIX M)						{m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v) 					{m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_HeightScale(float f)							{m_HeightScale->SetFloat(f);}
	void set_MaxTessDistance(float f)						{m_MaxTessDistance->SetFloat(f);}
	void set_MinTessDistance(float f)						{m_MinTessDistance->SetFloat(f);}
	void set_MinTessFactor(float f) 						{m_MinTessFactor->SetFloat(f);}
	void set_MaxTessFactor(float f) 						{m_MaxTessFactor->SetFloat(f);}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)		{m_DiffuseMap->SetResource(tex);}
	void set_NormalMap(ID3D11ShaderResourceView *tex)		{m_NormalMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_BuildShadowMapTech;
	ID3DX11EffectTechnique *m_BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique *m_BuildShadowMapSkinnedTech;
	ID3DX11EffectTechnique *m_BuildShadowMapAlphaClipSkinnedTech;
	ID3DX11EffectTechnique *m_TessBuildShadowMapTech;
	ID3DX11EffectTechnique *m_TessBuildShadowMapAlphaClipTech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_BoneTransforms;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectScalarVariable *m_HeightScale;
	ID3DX11EffectScalarVariable *m_MaxTessDistance;
	ID3DX11EffectScalarVariable *m_MinTessDistance;
	ID3DX11EffectScalarVariable *m_MinTessFactor;
	ID3DX11EffectScalarVariable *m_MaxTessFactor;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_NormalMap;
};
//
build_shadow_map_effect::build_shadow_map_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_BuildShadowMapTech					= m_FX->GetTechniqueByName("BuildShadowMapTech");
	m_BuildShadowMapAlphaClipTech			= m_FX->GetTechniqueByName("BuildShadowMapAlphaClipTech");
	m_BuildShadowMapSkinnedTech 			= m_FX->GetTechniqueByName("BuildShadowMapSkinnedTech");
	m_BuildShadowMapAlphaClipSkinnedTech	= m_FX->GetTechniqueByName("BuildShadowMapAlphaClipSkinnedTech");
	m_TessBuildShadowMapTech				= m_FX->GetTechniqueByName("TessBuildShadowMapTech");
	m_TessBuildShadowMapAlphaClipTech		= m_FX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");
	m_ViewProj			= m_FX->GetVariableByName("gViewProj")->AsMatrix();
	m_WorldViewProj 	= m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_World 			= m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_BoneTransforms	= m_FX->GetVariableByName("gBoneTransforms")->AsMatrix();
	m_TexTransform		= m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_EyePosW			= m_FX->GetVariableByName("gEyePosW")->AsVector();
	m_HeightScale		= m_FX->GetVariableByName("gHeightScale")->AsScalar();
	m_MaxTessDistance	= m_FX->GetVariableByName("gMaxTessDistance")->AsScalar();
	m_MinTessDistance	= m_FX->GetVariableByName("gMinTessDistance")->AsScalar();
	m_MinTessFactor 	= m_FX->GetVariableByName("gMinTessFactor")->AsScalar();
	m_MaxTessFactor 	= m_FX->GetVariableByName("gMaxTessFactor")->AsScalar();
	m_DiffuseMap		= m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_NormalMap 		= m_FX->GetVariableByName("gNormalMap")->AsShaderResource();
}
////////////////
// effects
////////////////
////////////////
class effects
{
public:
	static void init_all(ID3D11Device *device);
	static void destroy_all();
	static basic_effect *m_BasicFX;
	static normal_map_effect *m_NormalMapFX;
	static displacement_map_effect *m_DisplacementMapFX;
	static build_shadow_map_effect *m_BuildShadowMapFX;
	static sky_effect *m_SkyFX;
	static terrain_effect *m_TerrainFX;
	static particle_effect *m_FireFX;
	static particle_effect *m_RainFX;
	static debug_tex_effect *m_DebugTexFX;
	static ssao_normal_depth_effect *m_SsaoNormalDepthFX;
	static ssao_effect *m_SsaoFX;
	static ssao_blur_effect *m_SsaoBlurFX;
};
basic_effect *effects::m_BasicFX = 0;
normal_map_effect *effects::m_NormalMapFX = 0;
displacement_map_effect *effects::m_DisplacementMapFX = 0;
build_shadow_map_effect *effects::m_BuildShadowMapFX = 0;
sky_effect *effects::m_SkyFX = 0;
terrain_effect *effects::m_TerrainFX = 0;
particle_effect *effects::m_FireFX = 0;
particle_effect *effects::m_RainFX = 0;
debug_tex_effect *effects::m_DebugTexFX = 0;
ssao_normal_depth_effect *effects::m_SsaoNormalDepthFX = 0;
ssao_effect *effects::m_SsaoFX = 0;
ssao_blur_effect *effects::m_SsaoBlurFX = 0;
void effects::destroy_all()
{
	SafeDelete(m_BasicFX);
	SafeDelete(m_NormalMapFX);
	SafeDelete(m_DisplacementMapFX);
	SafeDelete(m_BuildShadowMapFX);
	SafeDelete(m_SkyFX);
	SafeDelete(m_TerrainFX);
	SafeDelete(m_FireFX);
	SafeDelete(m_RainFX);
	SafeDelete(m_DebugTexFX);
	SafeDelete(m_SsaoNormalDepthFX);
	SafeDelete(m_SsaoFX);
	SafeDelete(m_SsaoBlurFX);
}
//
void effects::init_all(ID3D11Device *device)
{
	std::wstring path(GLOBAL["path_fx"].begin(), GLOBAL["path_fx"].end());
	m_BasicFX = new basic_effect(device, path+L"basic.fxo");
	m_NormalMapFX = new normal_map_effect(device, path+L"normal_map_shadow.fxo");
	m_DisplacementMapFX = new displacement_map_effect(device, path+L"displacement_map.fxo");
	m_BuildShadowMapFX = new build_shadow_map_effect(device, path+L"build_shadow_map.fxo");
	m_SkyFX = new sky_effect(device, path+L"sky.fxo");
	m_TerrainFX = new terrain_effect(device, path+L"terrain.fxo");
	m_FireFX = new particle_effect(device, path+L"fire.fxo");
	m_RainFX = new particle_effect(device, path+L"rain.fxo");
	m_DebugTexFX = new debug_tex_effect(device, path+L"debug_texture.fxo");
	m_SsaoNormalDepthFX = new ssao_normal_depth_effect(device, path+L"ssao_normal_depth.fxo");
	m_SsaoFX = new ssao_effect(device, path+L"ssao.fxo");
	m_SsaoBlurFX = new ssao_blur_effect(device, path+L"ssao_blur.fxo");
}
}
#endif