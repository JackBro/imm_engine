////////////////
// ui_main_menu_text.h
////////////////
////////////////
template <typename T_app>
void ui_main_menu<T_app>::define_txt_str()
{
	// help
	m_DefineTxt["help"] = L"\n";
	m_DefineTxt["help"] += L"Help\n";
	m_DefineTxt["help"] += L"\n";
	m_DefineTxt["help"] += L"Keyboard and mouse:\n";
	m_DefineTxt["help"] += L"W, A, S, D             Camera horizontal/vertical\n";
	m_DefineTxt["help"] += L"Space                  Jump\n";
	m_DefineTxt["help"] += L"Wheel Up/Down          Camera forward/backward\n";
	m_DefineTxt["help"] += L"Middle Mouse Button    Camera pitch/yaw\n";
	m_DefineTxt["help"] += L"Right Click            Select an instance\n";
	m_DefineTxt["help"] += L"Left Click             Walk or Run\n";
	m_DefineTxt["help"] += L"Shift                  Switch between Walk and Run\n";
	m_DefineTxt["help"] += L"\n";
	m_DefineTxt["help"] += L"System hotkeys:\n";
	m_DefineTxt["help"] += L"Ctrl+R                 Prints FPS\n";
	m_DefineTxt["help"] += L"Alt+Delete             Input/Gameplay mode\n";
	m_DefineTxt["help"] += L"\n";
	m_DefineTxt["help"] += L"Gamepad:\n";
	m_DefineTxt["help"] += L"Left Stick             Walk\n";
	m_DefineTxt["help"] += L"RT+Left Stick          Run\n";
	m_DefineTxt["help"] += L"Buttom A               Jump\n";
	m_DefineTxt["help"] += L"D-pad                  Camera horizontal/vertical\n";
	m_DefineTxt["help"] += L"Right Stick            Camera pitch/yaw\n";
	m_DefineTxt["help"] += L"LB                     Camera forward\n";
	m_DefineTxt["help"] += L"RB                     Camera backward\n";
	m_DefineTxt["help"] += L"\n";
	m_DefineTxt["help"] += L"Note:\n";
	m_DefineTxt["help"] += L"The program automatically detects the gamepad status, ";
	m_DefineTxt["help"] += L"if the gamepad is plugin, the keyboard and mouse control will be disabled.\n";
	// credit
	m_DefineTxt["credit"] = L"\n";
	// about
	m_DefineTxt["about"] = L"\n";
}