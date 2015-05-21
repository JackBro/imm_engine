////////////////
// ui_simple_define_rc.h
////////////////
////////////////
template <typename T_app>
void ui_simple<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, 32.0f, 2);
	m_Dwrite["24"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, 24.0f, 2);
	m_Dwrite["20_alig"].init_without_rect(m_App->m_D2DDC, m_App->m_hwnd, 20.0f, 3);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.5f, "black");
	m_Brush["yellow"];
	set_Brush(D2D1::ColorF::Yellow, 0.5f, "yellow");
	m_Brush["red"];
	set_Brush(D2D1::ColorF::Red, 0.5f, "red");
	////////////////
	// fps
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "fps";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "fps";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"FPS";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.85f, 0.0f, 0.0f, 0.9f);
	////////////////
	// menu
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_title";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Main Menu";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_exit";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Exit";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.2f, 0.3f, 0.72f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_help";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Help";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.35f, 0.3f, 0.57f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_credit";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Credit";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.5f, 0.3f, 0.42f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "menu_about";
	m_Rect.back().parent_str = "menu_backg";
	m_Rect.back().group = "menu";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"About";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.3f, 0.65f, 0.3f, 0.27f);
	////////////////
	// exit
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.15f, 0.0f, 0.15f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_yes";
	m_Rect.back().parent_str = "exit_backg";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Yes";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.25f, 0.6f, 0.6f, 0.3f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_no";
	m_Rect.back().parent_str = "exit_backg";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"No";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.6f, 0.6f, 0.25f, 0.3f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "exit_title";
	m_Rect.back().parent_str = "exit_backg";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Exit the Demo?";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f);
	////////////////
	// help
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "help_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "help";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "help_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "help";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.75f, 0.82f, 0.05f, 0.1f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "help_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "help";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("help");
	m_Rect.back().dwrite_ix = "20_alig";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	// credit
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "credit_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "credit_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.75f, 0.82f, 0.05f, 0.1f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "credit_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("credit");
	m_Rect.back().dwrite_ix = "20_alig";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	// about
	////////////////
	////////////////
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "about_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "about_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"yellow"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.75f, 0.82f, 0.05f, 0.1f);
	//
	m_Rect.emplace_back(ui_rect());
	m_Rect.back().id_str = "about_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("about");
	m_Rect.back().dwrite_ix = "20_alig";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	//
	////////////////
	////////////////
	build_rect_map();
	//
	for (auto &rect: m_Rect) {
		rect.brush_ix = rect.brush_sel[0];
		if (rect.tp == ui_rect::type::button) rect.brush_sel.emplace_back("red");
		rect.active = false;
	}
	m_Rect[m_MapGroup["fps"][0]].active = true;
}
//
template <typename T_app>
bool ui_simple<T_app>::define_apply_ix_if(int &index)
{
	// menu
	if (index == m_MapID["menu_exit"]) {
		group_active("menu", false);
		group_active("exit", true);
		return true;
	}
	if (index == m_MapID["menu_help"]) {
		group_active("menu", false);
		group_active("help", true);
		return true;
	}
	if (index == m_MapID["menu_credit"]) {
		group_active("menu", false);
		group_active("credit", true);
		return true;
	}
	if (index == m_MapID["menu_about"]) {
		group_active("menu", false);
		group_active("about", true);
		return true;
	}
	// exit
	if (index == m_MapID["exit_yes"]) {
		PostQuitMessage(0);
		return true;
	}
	if (index == m_MapID["exit_no"]) {
		group_active("exit", false);
		return true;
	}
	// help
	if (index == m_MapID["help_close"]) {
		group_active("help", false);
		return true;
	}
	// about
	if (index == m_MapID["credit_close"]) {
		group_active("credit", false);
		return true;
	}
	// about
	if (index == m_MapID["about_close"]) {
		group_active("about", false);
		return true;
	}
	return false;	
}
//
template <typename T_app>
void ui_simple<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	m_IsPadUsing = false;
	if (w_param == KEY_UI_ESC) {
		if (is_ui_appear()) define_deactivate_all_default();
		else group_active("menu", true);
		return;
	}
	if (w_param == KEY_UI_FPS1 && GetKeyState(KEY_UI_FPS2)) {
		group_active_switch("fps");
		return;
	}
}
//
template <typename T_app>
void ui_simple<T_app>::define_on_pad_keydown(const WORD &vkey, const float &dt)
{
	DUMMY(dt);
	m_IsPadUsing = true;
	if (vkey == PAD_UI_MENU) {
		group_active_switch("menu");
		return;
	}
	if (vkey == PAD_UI_DWON1 || vkey == PAD_UI_DWON2) {
		pad_loop_button(true);
		if (vkey == PAD_UI_DWON1) pad_roll_text_layout(true);
		return;
	}
	if (vkey == PAD_UI_UP1 || vkey == PAD_UI_UP2) {
		pad_loop_button(false);
		if (vkey == PAD_UI_UP1) pad_roll_text_layout(false);
		return;
	}
	if (vkey == PAD_UI_DEACTIVATE) {
		define_deactivate_all_default();
		return;
	}
	//
	if (m_ClickableActived != "none") {
		if (vkey == PAD_UI_APPLY) apply_ix(m_ClickIxPad);
		return;
	}
}
//
template <typename T_app>
void ui_simple<T_app>::define_update()
{
	m_Rect[m_MapGroup["fps"][0]].text = m_App->m_Fps;
}
//
template <typename T_app>
void ui_simple<T_app>::define_deactivate_all_default()
{
	bool fps = m_Rect[m_MapGroup["fps"][0]].active;
	deactivate_all();
	if (fps) group_active_switch("fps");
}
//
