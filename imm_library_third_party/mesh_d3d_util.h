////////////////
// mesh_d3d_util.h
// d3dUtil.h by Frank Luna (C) 2011 All Rights Reserved.
////////////////
////////////////
#ifndef MESH_D3D_UTIL_H
#define MESH_D3D_UTIL_H
#include "imm_core.h"
#include "DDSTextureLoader.h"
namespace imm
{
////////////////
// create_Texture2DArraySRV
////////////////
////////////////
ID3D11ShaderResourceView *create_Texture2DArraySRV
	(ID3D11Device *device, ID3D11DeviceContext *context, std::vector<std::wstring> &filenames)
{
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	size_t size = filenames.size();
	std::vector<ID3D11Texture2D*> src_tex(size);
	for(size_t i = 0; i < size; ++i) {
		HRESULT hr = CreateDDSTextureFromFileEx(
			device,
			filenames[i].c_str(), 0,
			D3D11_USAGE_STAGING, 0,
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ, 0, 0,
			(ID3D11Resource**)&src_tex[i], 0);
		if FAILED(hr) {
			std::string err_str("Texture file load error: ");
			err_str += wstr_to_str(filenames[i]);
			err_str += "\nHRESULT: ";
			err_str += std::to_string(hr);		
			ERROR_MESA(err_str.c_str());
		}
	}
	// Create the texture array.  Each element in the texture
	// array has the same format/dimensions.
	D3D11_TEXTURE2D_DESC tex_element_desc;
	src_tex[0]->GetDesc(&tex_element_desc);
	//
	D3D11_TEXTURE2D_DESC tex_array_desc;
	tex_array_desc.Width				= tex_element_desc.Width;
	tex_array_desc.Height				= tex_element_desc.Height;
	tex_array_desc.MipLevels			= tex_element_desc.MipLevels;
	tex_array_desc.ArraySize			= static_cast<UINT>(size);
	tex_array_desc.Format				= tex_element_desc.Format;
	tex_array_desc.SampleDesc.Count		= 1;
	tex_array_desc.SampleDesc.Quality	= 0;
	tex_array_desc.Usage				= D3D11_USAGE_DEFAULT;
	tex_array_desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex_array_desc.CPUAccessFlags		= 0;
	tex_array_desc.MiscFlags			= 0;
	//
	ID3D11Texture2D *tex_array = 0;
	HR(device->CreateTexture2D(&tex_array_desc, 0, &tex_array));
	// Copy individual texture elements into texture array.
	// for each texture element...
	for(UINT tex_element = 0; tex_element < size; ++tex_element) {
		// for each mipmap level...
		for(UINT mip_level = 0; mip_level < tex_element_desc.MipLevels; ++mip_level) {
			D3D11_MAPPED_SUBRESOURCE mapped_tex2d;
			HR(context->Map(src_tex[tex_element], mip_level, D3D11_MAP_READ, 0, &mapped_tex2d));
 			context->UpdateSubresource(tex_array,
				D3D11CalcSubresource(mip_level, tex_element, tex_element_desc.MipLevels),
				0, mapped_tex2d.pData, mapped_tex2d.RowPitch, mapped_tex2d.DepthPitch);
 			context->Unmap(src_tex[tex_element], mip_level);
		}
	}
	// Create a resource view to the texture array.
 	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;
	view_desc.Format							= tex_array_desc.Format;
	view_desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	view_desc.Texture2DArray.MostDetailedMip	= 0;
	view_desc.Texture2DArray.MipLevels			= tex_array_desc.MipLevels;
	view_desc.Texture2DArray.FirstArraySlice	= 0;
	view_desc.Texture2DArray.ArraySize			= static_cast<UINT>(size);
	//
	ID3D11ShaderResourceView *tex_arraySRV = 0;
	HR(device->CreateShaderResourceView(tex_array, &view_desc, &tex_arraySRV));
	// Cleanup--we only need the resource view.
	ReleaseCOM(tex_array);
	for(UINT i = 0; i < size; ++i) ReleaseCOM(src_tex[i]);
	return tex_arraySRV;
}
////////////////
// create_RandomTexture1DSRV
////////////////
////////////////
ID3D11ShaderResourceView *create_RandomTexture1DSRV(ID3D11Device* device)
{
	// Create the random data.
	XMFLOAT4 random_values[1024];
	for(int i = 0; i < 1024; ++i) {
		random_values[i].x = calc_randf(-1.0f, 1.0f);
		random_values[i].y = calc_randf(-1.0f, 1.0f);
		random_values[i].z = calc_randf(-1.0f, 1.0f);
		random_values[i].w = calc_randf(-1.0f, 1.0f);
	}
	D3D11_SUBRESOURCE_DATA init_data;
	init_data.pSysMem			= random_values;
	init_data.SysMemPitch		= 1024*sizeof(XMFLOAT4);
	init_data.SysMemSlicePitch	= 0;
	// Create the texture.
	D3D11_TEXTURE1D_DESC tex_desc;
	tex_desc.Width			= 1024;
	tex_desc.MipLevels		= 1;
	tex_desc.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.Usage			= D3D11_USAGE_IMMUTABLE;
	tex_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags	= 0;
	tex_desc.MiscFlags		= 0;
	tex_desc.ArraySize		= 1;
	//
	ID3D11Texture1D* random_tex = 0;
	HR(device->CreateTexture1D(&tex_desc, &init_data, &random_tex));
	// Create the resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;
	view_desc.Format					= tex_desc.Format;
	view_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE1D;
	view_desc.Texture1D.MipLevels		= tex_desc.MipLevels;
	view_desc.Texture1D.MostDetailedMip	= 0;
	ID3D11ShaderResourceView* random_tex_srv = 0;
	HR(device->CreateShaderResourceView(random_tex, &view_desc, &random_tex_srv));
	ReleaseCOM(random_tex);
	return random_tex_srv;
}
////////////////
// extract_frustum_planes
////////////////
////////////////
void extract_frustum_planes(XMFLOAT4 planes[6], CXMMATRIX M)
{
	// Left
	planes[0].x = M.r[0].m128_f32[3] + M.r[0].m128_f32[0];
	planes[0].y = M.r[1].m128_f32[3] + M.r[1].m128_f32[0];
	planes[0].z = M.r[2].m128_f32[3] + M.r[2].m128_f32[0];
	planes[0].w = M.r[3].m128_f32[3] + M.r[3].m128_f32[0];
	// Right
	planes[1].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[0];
	planes[1].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[0];
	planes[1].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[0];
	planes[1].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[0];
	// Bottom
	planes[2].x = M.r[0].m128_f32[3] + M.r[0].m128_f32[1];
	planes[2].y = M.r[1].m128_f32[3] + M.r[1].m128_f32[1];
	planes[2].z = M.r[2].m128_f32[3] + M.r[2].m128_f32[1];
	planes[2].w = M.r[3].m128_f32[3] + M.r[3].m128_f32[1];	
	// Top
	planes[3].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[1];
	planes[3].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[1];
	planes[3].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[1];
	planes[3].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[1];	
	// Near
	planes[4].x = M.r[0].m128_f32[2];
	planes[4].y = M.r[1].m128_f32[2];
	planes[4].z = M.r[2].m128_f32[2];
	planes[4].w = M.r[3].m128_f32[2];
	// Far
	planes[5].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[2];
	planes[5].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[2];
	planes[5].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[2];
	planes[5].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[2];
	// Normalize the plane equations.
	for(int i = 0; i < 6; ++i) {
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}
}
#endif