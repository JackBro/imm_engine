////////////////
// stru_effect_ssao.h
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
////////////////
////////////////
#ifndef STRU_EFFECT_SSAO_H
#define STRU_EFFECT_SSAO_H
#include "imm_core.h"
namespace imm
{
////////////////
// ssao_normal_depth_effect
////////////////
////////////////
class ssao_normal_depth_effect: public effect
{
public:
	ssao_normal_depth_effect(ID3D11Device *device, const std::wstring &filename);
	~ssao_normal_depth_effect() {;}
	void set_WorldView(CXMMATRIX M) 						{m_WorldView->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTransposeView(CXMMATRIX M) 			{m_WorldInvTransposeView->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_BoneTransforms(const XMFLOAT4X4 *M, int cnt)	{m_BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);}
	void set_WorldViewProj(CXMMATRIX M) 					{m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX M)						{m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)		{m_DiffuseMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_NormalDepthTech;
	ID3DX11EffectTechnique *m_NormalDepthAlphaClipTech;
	ID3DX11EffectTechnique *m_NormalDepthSkinnedTech;
	ID3DX11EffectTechnique *m_NormalDepthAlphaClipSkinnedTech;
	ID3DX11EffectMatrixVariable *m_WorldView;
	ID3DX11EffectMatrixVariable *m_WorldInvTransposeView;
	ID3DX11EffectMatrixVariable *m_BoneTransforms;	
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
};
//
ssao_normal_depth_effect::ssao_normal_depth_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_NormalDepthTech					= m_FX->GetTechniqueByName("NormalDepth");
	m_NormalDepthAlphaClipTech			= m_FX->GetTechniqueByName("NormalDepthAlphaClip");
	m_NormalDepthSkinnedTech			= m_FX->GetTechniqueByName("NormalDepthSkinned");
	m_NormalDepthAlphaClipSkinnedTech	= m_FX->GetTechniqueByName("NormalDepthAlphaClipSkinned");
	m_WorldView 			= m_FX->GetVariableByName("gWorldView")->AsMatrix();
	m_WorldInvTransposeView = m_FX->GetVariableByName("gWorldInvTransposeView")->AsMatrix();
	m_BoneTransforms		= m_FX->GetVariableByName("gBoneTransforms")->AsMatrix();
	m_WorldViewProj 		= m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_TexTransform			= m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_DiffuseMap			= m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}
////////////////
// ssao_effect
////////////////
////////////////
class ssao_effect: public effect
{
public:
	ssao_effect(ID3D11Device *device, const std::wstring &filename);
	~ssao_effect() {;}
	void set_ViewToTexSpace(CXMMATRIX M)					{m_ViewToTexSpace->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_OffsetVectors(const XMFLOAT4 v[14])			{m_OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14);}
	void set_FrustumCorners(const XMFLOAT4 v[4])			{m_FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4);}
	void set_OcclusionRadius(float f)						{m_OcclusionRadius->SetFloat(f);}
	void set_OcclusionFadeStart(float f)					{m_OcclusionFadeStart->SetFloat(f);}
	void set_OcclusionFadeEnd(float f)						{m_OcclusionFadeEnd->SetFloat(f);}
	void set_SurfaceEpsilon(float f)						{m_SurfaceEpsilon->SetFloat(f);}
	void set_NormalDepthMap(ID3D11ShaderResourceView *srv)	{m_NormalDepthMap->SetResource(srv);}
	void set_RandomVecMap(ID3D11ShaderResourceView *srv)	{m_RandomVecMap->SetResource(srv);}
	ID3DX11EffectTechnique *m_SsaoTech;
	ID3DX11EffectMatrixVariable *m_ViewToTexSpace;
	ID3DX11EffectVectorVariable *m_OffsetVectors;
	ID3DX11EffectVectorVariable *m_FrustumCorners;
	ID3DX11EffectScalarVariable *m_OcclusionRadius;
	ID3DX11EffectScalarVariable *m_OcclusionFadeStart;
	ID3DX11EffectScalarVariable *m_OcclusionFadeEnd;
	ID3DX11EffectScalarVariable *m_SurfaceEpsilon;
	ID3DX11EffectShaderResourceVariable *m_NormalDepthMap;
	ID3DX11EffectShaderResourceVariable *m_RandomVecMap;
};
//
ssao_effect::ssao_effect(ID3D11Device *device, const std::wstring &filename):
	effect(device, filename)
{
	m_SsaoTech				= m_FX->GetTechniqueByName("Ssao");
	m_ViewToTexSpace		= m_FX->GetVariableByName("gViewToTexSpace")->AsMatrix();
	m_OffsetVectors 		= m_FX->GetVariableByName("gOffsetVectors")->AsVector();
	m_FrustumCorners		= m_FX->GetVariableByName("gFrustumCorners")->AsVector();
	m_OcclusionRadius		= m_FX->GetVariableByName("gOcclusionRadius")->AsScalar();
	m_OcclusionFadeStart	= m_FX->GetVariableByName("gOcclusionFadeStart")->AsScalar();
	m_OcclusionFadeEnd		= m_FX->GetVariableByName("gOcclusionFadeEnd")->AsScalar();
	m_SurfaceEpsilon		= m_FX->GetVariableByName("gSurfaceEpsilon")->AsScalar();
	m_NormalDepthMap		= m_FX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	m_RandomVecMap			= m_FX->GetVariableByName("gRandomVecMap")->AsShaderResource();
}
////////////////
// ssao_blur_effect
////////////////
////////////////
class ssao_blur_effect: public effect
{
public:
	ssao_blur_effect(ID3D11Device *device, const std::wstring &filename);
	~ssao_blur_effect() {;}
	void set_TexelWidth(float f)							{m_TexelWidth->SetFloat(f);}
	void set_TexelHeight(float f)							{m_TexelHeight->SetFloat(f);}
	void set_NormalDepthMap(ID3D11ShaderResourceView *srv)	{m_NormalDepthMap->SetResource(srv);}
	void set_InputImage(ID3D11ShaderResourceView *srv)		{m_InputImage->SetResource(srv);}
	ID3DX11EffectTechnique *m_HorzBlurTech;
	ID3DX11EffectTechnique *m_VertBlurTech;
	ID3DX11EffectScalarVariable *m_TexelWidth;
	ID3DX11EffectScalarVariable *m_TexelHeight;
	ID3DX11EffectShaderResourceVariable *m_NormalDepthMap;
	ID3DX11EffectShaderResourceVariable *m_InputImage;
};
//
ssao_blur_effect::ssao_blur_effect(ID3D11Device *device, const std::wstring &filename)
	: effect(device, filename)
{
	m_HorzBlurTech	= m_FX->GetTechniqueByName("HorzBlur");
	m_VertBlurTech	= m_FX->GetTechniqueByName("VertBlur");
	m_TexelWidth		= m_FX->GetVariableByName("gTexelWidth")->AsScalar();
	m_TexelHeight		= m_FX->GetVariableByName("gTexelHeight")->AsScalar();
	m_NormalDepthMap	= m_FX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	m_InputImage		= m_FX->GetVariableByName("gInputImage")->AsShaderResource();
}
}
#endif