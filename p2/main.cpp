#include <windows.h>
#include <windowsx.h>

#include "res.h"

void on_create(HWND hw) 
{
	char s[128];
	::LoadString(0, IDS_B1, s, sizeof s);
	::CreateWindow("BUTTON", s, WS_CHILD | WS_VISIBLE, 50, 50, 80, 24, hw, HMENU(IDC_BUTTON1), 0, 0);
	::LoadString(0, IDS_B2, s, sizeof s);
	::CreateWindow("BUTTON", s, WS_CHILD | WS_VISIBLE, 50, 80, 80, 24, hw, HMENU(IDC_BUTTON2), 0, 0);
}

void on_command(HWND hw, int id, int code) 
{
	switch (id) 
	{
		case IDC_BUTTON1:
		{
			HRSRC hr = ::FindResource(0, MAKEINTRESOURCE(ID_BIN), "MYTYPE");
			HGLOBAL hg = ::LoadResource(0, hr);
			void* data = ::LockResource(hg);
			char* text = reinterpret_cast<char*>(data);
			::MessageBox(hw, text, "NWP 2", MB_OK | MB_ICONWARNING);
			::FreeResource(hg);
		}
		break;
		case IDC_BUTTON2:
			::MoveWindow(::GetDlgItem(hw, IDC_BUTTON2), 150, 80, 80, 24, true);
			break;
		case ID_FILE_EXIT:
			::DestroyWindow(hw);
			break;
		case ID_HELP_ABOUT:
			::MessageBox(hw, "About 1", "NWP 2", MB_OK | MB_ICONWARNING);
			break;
	}
}

void on_context_menu(HWND hw) 
{
	HMENU hm = ::CreatePopupMenu();
	::AppendMenu(hm, MF_STRING, ID_HELP_ABOUT, "&About");
	::AppendMenu(hm, MF_SEPARATOR, 0, 0);
	::AppendMenu(hm, MF_STRING, ID_FILE_EXIT, "E&xit");
	DWORD pos = ::GetMessagePos();
	int x = GET_X_LPARAM(pos), y = GET_Y_LPARAM(pos);

	// TrackPopupMenu(hPopup, 0, x, y, 0, hwnd, 0); 	// verzija 1: salje WM_COMMAND prozoru
	UINT id = ::TrackPopupMenu(hm, TPM_RETURNCMD, x, y, 0, hw, 0); 	// verzija 2: vraca odabrani id (TPM_RETURNCMD)
	if (id == ID_HELP_ABOUT)
		::MessageBox(hw, "About 2", "NWP 2", MB_OK | MB_ICONWARNING);
}

LRESULT CALLBACK window_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) 
{
	switch (msg) 
	{
		case WM_CREATE:
			on_create(hw);
			return 0;
		case WM_COMMAND:
			on_command(hw, LOWORD(wp), HIWORD(wp));
			return 0;
		case WM_CONTEXTMENU:
			on_context_menu(hw);
			return 0;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}
	return ::DefWindowProc(hw, msg, wp, lp);
}

int register_class(HINSTANCE hi, const char* name) 
{
	WNDCLASS wc
	{
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		window_proc,
		0,
		0,
		hi,
		::LoadIcon(hi, MAKEINTRESOURCE(IDI_P2)),
		::LoadCursor(hi, MAKEINTRESOURCE(IDC_P2)), // LoadCursor(0, IDC_ARROW)
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
	HMENU hm = ::LoadMenu(hi, MAKEINTRESOURCE(IDM_P2));
	::CreateWindow(class_name, "NWP 2", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, hm, hi, 0);
	MSG msg;
	while (::GetMessage(&msg, 0, 0, 0)) 
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return msg.wParam;
}
