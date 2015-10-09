#include <windows.h>
#include <string>
#include "resource.h"

//////////////////////////////////////////////////////////////////////////////////////////
// modeless dialog functions                                                               //
//////////////////////////////////////////////////////////////////////////////////////////

HWND hw_modeless;

BOOL CALLBACK modeless_dlg_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// modal dialog functions                                                               //
//////////////////////////////////////////////////////////////////////////////////////////

bool on_init_dialog(HWND hw)
{
	// edit
	::SetDlgItemText(hw, IDC_EDIT1, "abcd");
	::SetDlgItemInt(hw, IDC_EDIT2, 123, true);
	// check box
	::CheckDlgButton(hw, IDC_CHECK1, BST_CHECKED);
	// radio buttons
	::CheckRadioButton(hw, IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
	// combo box
	for(int i=0; i<10; ++i)
	{
		std::string s { "item " + std::to_string(i) };
		::SendDlgItemMessage(hw, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)s.c_str());
	}
	::SendDlgItemMessage(hw, IDC_COMBO1, CB_SETCURSEL, 1, 0);
	// list box
	for(int i=0; i<10; ++i)
	{
		std::string s { "item " + std::to_string(i) };
		::SendDlgItemMessage(hw, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)s.c_str());
	}
	::SendDlgItemMessage(hw, IDC_LIST1, LB_SETCURSEL, 2, 0);
	return true;
}


void on_ok(HWND hw)
{
	char s[64]; 
	::GetDlgItemText(hw, IDC_EDIT1, s, sizeof s);
	BOOL success; 
	int n = ::GetDlgItemInt(hw, IDC_EDIT2, &success, true);
	int combo_box_sel_index = ::SendDlgItemMessage(hw, IDC_COMBO1, CB_GETCURSEL, 0, 0);
	int list_box_sel_index  = ::SendDlgItemMessage(hw, IDC_LIST1, LB_GETCURSEL, 0, 0);
}

bool on_command(HWND hw, int id) {
	switch (id) {
		case IDC_CHECK1: 
			{
			bool checked = ::IsDlgButtonChecked(hw, IDC_CHECK1) == BST_CHECKED;
			::EnableWindow(::GetDlgItem(hw, IDC_EDIT1), checked);
			::EnableWindow(::GetDlgItem(hw, IDC_EDIT2), checked);
			}
			return true;
		case IDOK:
			on_ok(hw);
			[[fallthrough]];
		case IDCANCEL:
			::EndDialog(hw, id);
			return true;
	}
	return false;
}

BOOL CALLBACK modal_dlg_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			return on_init_dialog(hw);
		case WM_COMMAND:
			return on_command(hw, LOWORD(wp));
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// common dialogs                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////

void file_open(HWND parent)
{
	char path[MAX_PATH]; *path = 0;
	char filter[] = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	OPENFILENAME ofn{ sizeof OPENFILENAME };
	ofn.hwndOwner = parent;
	ofn.lpstrFile = path;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;
	ofn.Flags = OFN_HIDEREADONLY;
	if(::GetOpenFileName(&ofn))
		::MessageBox(0, ofn.lpstrFile, "NWP", MB_OK);
}

void file_save(HWND parent)
{
	char path[MAX_PATH]; *path = 0;
	char filter[] = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	OPENFILENAME ofn{ sizeof OPENFILENAME };
	ofn.hwndOwner = parent;
	ofn.lpstrFile = path;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if(::GetSaveFileName(&ofn))
		::MessageBox(0, ofn.lpstrFile, "NWP", MB_OK);
}


void choose_color(HWND parent)
{
	static COLORREF color = RGB(255,128,0);
	COLORREF custom_colors[16] {0};
	CHOOSECOLOR cc{sizeof(CHOOSECOLOR)};
	cc.Flags = CC_FULLOPEN|CC_RGBINIT;
	cc.hwndOwner = parent;
	cc.lpCustColors = custom_colors;
	cc.rgbResult = color;
	if(::ChooseColor(&cc))
		color = cc.rgbResult;
}

void choose_font(HWND parent)
{
	static LOGFONT lf{}; 
	if(0 == *lf.lfFaceName) strcpy_s(lf.lfFaceName, "Arial");
	CHOOSEFONT cf{ sizeof CHOOSEFONT };
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	cf.hwndOwner = parent;
	cf.lpLogFont = &lf;
	::ChooseFont(&cf);
}

//////////////////////////////////////////////////////////////////////////////////////////
// main window functions                                                                //
//////////////////////////////////////////////////////////////////////////////////////////

void on_command(HWND hw, int id, int code)
{
	switch(id)
	{
		case ID_DIALOG_COMMON_COLOR:
			choose_color(hw);
			break;
		case ID_DIALOG_COMMON_FONT:
			choose_font(hw);
			break;
		case ID_DIALOG_COMMON_OPEN:
			file_open(hw);
			break;
		case ID_DIALOG_COMMON_SAVE:
			file_save(hw);
			break;
		case ID_DIALOG_MODAL:
			if(IDOK == ::DialogBox(0, MAKEINTRESOURCE(IDD_DIALOG1), hw, modal_dlg_proc))
				::MessageBox(hw, "OK!", "NWP", MB_OK);
			break;
		case ID_DIALOG_MODELESS:
			{
			if(0 == ::hw_modeless)
				::hw_modeless = ::CreateDialog(0, MAKEINTRESOURCE(IDD_DIALOG2), hw, modeless_dlg_proc);
			bool visible = ::IsWindowVisible(::hw_modeless) != 0;
			::ShowWindow(::hw_modeless, visible ? SW_HIDE : SW_SHOW);
			::CheckMenuItem(::GetSubMenu(::GetMenu(hw), 0), ID_DIALOG_MODELESS, !visible ? MF_CHECKED : MF_UNCHECKED);
			}
			break;
		case ID_EXIT:
			::DestroyWindow(hw);
			break;
	}
}

LRESULT CALLBACK window_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)	
	{
		case WM_COMMAND:
			on_command(hw, LOWORD(wp), HIWORD(wp));
			return 0;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}
	return ::DefWindowProc(hw, msg, wp, lp);
}

//////////////////////////////////////////////////////////////////////////////////////////
// basic functions                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

int register_class(HINSTANCE hi, const char* name) 
{
	WNDCLASS wc
	{
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		window_proc,
		0,
		0,
		hi,
		::LoadIcon(hi, MAKEINTRESOURCE(IDI_P3)),
		::LoadCursor(0, IDC_ARROW),
		static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)),
		0,
		name
	};
	return ::RegisterClass(&wc);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int)
{
	const auto class_name = "NWPClass";
	if (!register_class(hi, class_name))
		return 0;
	HMENU hm = ::LoadMenu(hi, MAKEINTRESOURCE(IDM_P3));
	::CreateWindow(class_name, "NWP 3",  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, hm, hi, 0); 
	MSG msg;
	while(::GetMessage(&msg, 0, 0, 0))
	{
		if (::hw_modeless == 0 || !::IsDialogMessage (::hw_modeless, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

// enable themes support ("visual styles")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
