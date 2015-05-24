////////////////
// ui_dwrite.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef UI_DWRITE_H
#define UI_DWRITE_H
#include <d2d1_1.h>
#include <Dwrite.h>
#include "imm_core.h"
////////////////
// DWRITE_ALIG_STYLE
////////////////
////////////////
static const int DWRITE_ALIG_STYLE_CMD = 0;
static const int DWRITE_ALIG_STYLE_CENTER = 1;
static const int DWRITE_ALIG_STYLE_PAGE = 2;
////////////////
// dwrite_simple
////////////////
////////////////
namespace imm
{
struct dwrite_simple
{
	dwrite_simple();
	~dwrite_simple();
	void on_resize_CreateTextFormat(HWND &hwnd);
	// for m_LayoutRc as writting area
	void init_solo(
		ID2D1DeviceContext *d2d_dc,
		HWND &hwnd,
		const std::wstring &font_name,
		const FLOAT &margin_factor,
		const float &font_factor);
	// for draw rect form external
	void init_without_rect(
		ID2D1DeviceContext *d2d_dc,
		HWND &hwnd,
		const std::wstring &font_name,
		const float &font_factor,
		const int &text_alig_style);
	void on_resize_LayoutRc(HWND &hwnd, const FLOAT &margin_factor);
	void set_Brush(ID2D1DeviceContext *d2d_dc, D2D1::ColorF::Enum color);
	template <typename T_wstring>
	void on_resize_TextLayout(
		T_wstring &wst_text,
		const FLOAT &width,
		const FLOAT &height,
		const size_t &index,
		const FLOAT &title_font,
		const size_t &title_len);
	void build_resize_TextLayout(const size_t &index, const FLOAT &width, const FLOAT &height);
	void get_TextLayout_height(const size_t &index, float &height);
	template <typename T_wstring>
	void draw(ID2D1DeviceContext *d2d_dc, T_wstring &wst_text);
	template <typename T_wstring>
	void draw_Rect(ID2D1DeviceContext *d2d_dc, T_wstring &wst_text, D2D1_RECT_F &rect);
	void draw_TextLayout(ID2D1DeviceContext *d2d_dc, const D2D1_POINT_2F &origin, const size_t &index);
	IDWriteFactory *m_WriteFactory;
	IDWriteTextFormat *m_TextFormat;
	ID2D1SolidColorBrush *m_Brush;
	std::map<size_t, IDWriteTextLayout*> m_TextLayout;
	D2D1_RECT_F m_LayoutRc;
	size_t m_MaxSize;
	float m_FontFactor;
	std::wstring m_FontName;
	int m_TextAligStyle;
private:
	dwrite_simple(const dwrite_simple &rhs);
	dwrite_simple &operator=(const dwrite_simple &rhs);
};
//
dwrite_simple::dwrite_simple():
	m_WriteFactory(nullptr),
	m_TextFormat(nullptr),
	m_Brush(nullptr),
	m_LayoutRc(),
	m_MaxSize(2500),
	m_FontFactor(0.0f),
	m_FontName(L"Consolas"),
	m_TextAligStyle(DWRITE_ALIG_STYLE_CMD)
{
	;
}
//
dwrite_simple::~dwrite_simple()
{
	ReleaseCOM(m_WriteFactory);
	ReleaseCOM(m_TextFormat);
	ReleaseCOM(m_Brush);
	for (auto text_layout: m_TextLayout) ReleaseCOM(text_layout.second);
}
//
void dwrite_simple::on_resize_CreateTextFormat(HWND &hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	float width = static_cast<float>(rc.right - rc.left);
	float height = static_cast<float>(rc.bottom - rc.top);
	// font size according 786p (1366*768 resolution)
	float width_size = m_FontFactor*width/REF_RESOLUTION_WIDTH;
	float height_size = m_FontFactor*height/REF_RESOLUTION_HEIGHT;
	float font_size = (width_size > height_size) ? width_size : height_size;
	ReleaseCOM(m_TextFormat);
	HR(m_WriteFactory->CreateTextFormat(
		m_FontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		font_size,
		L"en-us",
		&m_TextFormat
	));
	switch (m_TextAligStyle) {
		case DWRITE_ALIG_STYLE_CMD:
			HR(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
			HR(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
			return;
		case DWRITE_ALIG_STYLE_CENTER:
			HR(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
			HR(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
			return;
		case DWRITE_ALIG_STYLE_PAGE:
			HR(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
			HR(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
			return;		
	}
	assert(false);
}
//
void dwrite_simple::init_solo(
	ID2D1DeviceContext *d2d_dc,
	HWND &hwnd,
	const std::wstring &font_name,
	const FLOAT &margin_factor,
	const float &font_factor)
{
	m_FontName = font_name;
	m_FontFactor = font_factor;
	HR(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_WriteFactory)));
	on_resize_CreateTextFormat(hwnd);
	// m_LayoutRc as writting area
	on_resize_LayoutRc(hwnd, margin_factor);
	HR(d2d_dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_Brush));
}
//
void dwrite_simple::init_without_rect(
	ID2D1DeviceContext *d2d_dc,
	HWND &hwnd,
	const std::wstring &font_name,
	const float &font_factor,
	const int &text_alig_style)
{
	m_FontName = font_name;
	m_TextAligStyle = text_alig_style;
	m_FontFactor = font_factor;
	HR(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_WriteFactory)));
	on_resize_CreateTextFormat(hwnd);
	//	
	HR(d2d_dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_Brush));
}
//
void dwrite_simple::on_resize_LayoutRc(HWND &hwnd, const FLOAT &margin_factor)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	FLOAT margin = (rc.bottom - rc.top)*margin_factor;
	m_LayoutRc = D2D1::RectF(
		static_cast<FLOAT>(rc.left+margin),
		static_cast<FLOAT>(rc.top+margin),
		static_cast<FLOAT>(rc.right-rc.left-margin),
		static_cast<FLOAT>(rc.bottom-rc.top-margin)
	);
}
//
void dwrite_simple::set_Brush(ID2D1DeviceContext *d2d_dc, D2D1::ColorF::Enum color)
{
	HR(d2d_dc->CreateSolidColorBrush(D2D1::ColorF(color), &m_Brush));
}
//
template <typename T_wstring>
void dwrite_simple::on_resize_TextLayout(
	T_wstring &wst_text,
	const FLOAT &width,
	const FLOAT &height,
	const size_t &index,
	const FLOAT &title_font,
	const size_t &title_len)
{
	m_TextLayout[index];
	ReleaseCOM(m_TextLayout[index]);
	HR(m_WriteFactory->CreateTextLayout(
		wst_text.c_str(),
		static_cast<UINT32>(wst_text.size()),
		m_TextFormat,
		width,
		height,
		&m_TextLayout[index]));
	//
	DWRITE_TEXT_RANGE range;
	range.startPosition = 0;
	range.length = static_cast<UINT32>(title_len);
	m_TextLayout[index]->SetFontSize(title_font, range);
	m_TextLayout[index]->SetFontWeight (DWRITE_FONT_WEIGHT_BOLD , range);
}
//
void dwrite_simple::build_resize_TextLayout(const size_t &index, const FLOAT &width, const FLOAT &height)
{
	HR(m_TextLayout[index]->SetMaxHeight(height));
	HR(m_TextLayout[index]->SetMaxWidth(width));
}
//
void dwrite_simple::get_TextLayout_height(const size_t &index, float &height)
{
	DWRITE_TEXT_METRICS text_metrics;
	HR(m_TextLayout[index]->GetMetrics(&text_metrics));
	height = text_metrics.height;
}
//
template <typename T_wstring>
void dwrite_simple::draw(ID2D1DeviceContext *d2d_dc, T_wstring &wst_text)
{
	if (wst_text.size() > m_MaxSize*2) {
		wst_text.assign(wst_text.end()-m_MaxSize, wst_text.end());
	}
	d2d_dc->DrawText(
		wst_text.c_str(),
		static_cast<UINT32>(wst_text.size()),
		m_TextFormat,
		m_LayoutRc,
		m_Brush
	);
}
//
template <typename T_wstring>
void dwrite_simple::draw_Rect(ID2D1DeviceContext *d2d_dc, T_wstring &wst_text, D2D1_RECT_F &rect)
{
	if (wst_text.size() == 0) return;
	d2d_dc->DrawText(
		wst_text.c_str(),
		static_cast<UINT32>(wst_text.size()),
		m_TextFormat,
		rect,
		m_Brush
	);
}
//
void dwrite_simple::draw_TextLayout(ID2D1DeviceContext *d2d_dc, const D2D1_POINT_2F &origin, const size_t &index)
{
	assert(m_TextLayout[index]);
	d2d_dc->DrawTextLayout(
		origin,
		m_TextLayout[index],
		m_Brush);
}
}
#endif