// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "BuildServerDialog.h"
#include "BuildServer.h"
#include "resource.h"

#include <CommCtrl.h>

#define WM_SHELL_MESSAGE (WM_USER + 1)

#define SHELL_MENU_OPEN		(WM_USER + 2)
#define SHELL_MENU_REBUILD	(WM_USER + 3)
#define SHELL_MENU_SCAN		(WM_USER + 4)
#define SHELL_MENU_EXIT		(WM_USER + 5)
#define SHELL_SET_PROFILE	(WM_USER + 6)

namespace sb
{

	namespace
	{

		INT_PTR CALLBACK DialogProc(
			_In_  HWND hwndDlg,
			_In_  UINT uMsg,
			_In_  WPARAM wParam,
			_In_  LPARAM lParam
			)
		{
			BuildServerDialog* dialog = (BuildServerDialog*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);

			switch (uMsg)
			{
			case WM_SHELL_MESSAGE:
				switch (LOWORD(lParam))
				{
				case WM_LBUTTONUP:
					if (IsWindowVisible(hwndDlg))
					{
						SetForegroundWindow(hwndDlg);
					}
					return TRUE;
				case WM_LBUTTONDBLCLK:
					if (!IsWindowVisible(hwndDlg))
					{
						SetForegroundWindow(hwndDlg);
						ShowWindow(hwndDlg, SW_NORMAL);
					}
					else
						ShowWindow(hwndDlg, SW_HIDE);
					return TRUE;
				case WM_RBUTTONUP:
				{
					POINT pt;
					GetCursorPos(&pt);

					HMENU profile_menu = CreatePopupMenu();
					const vector<pair<string, bool>>& profiles = dialog->GetProfiles();
					for (uint32_t i = 0; i < profiles.size(); ++i)
					{
						UINT flags = MF_BYPOSITION | MF_STRING;
						if (profiles[i].second)
							flags |= MF_CHECKED;

						InsertMenu(profile_menu, 0xFFFFFFFF, flags, MAKEWPARAM(SHELL_SET_PROFILE, i), profiles[i].first.c_str());
					}

					HMENU hMenu = CreatePopupMenu();
					InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SHELL_MENU_OPEN, "Open");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, "Separator");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP, (UINT_PTR)profile_menu, "Select profile");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, "Separator");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SHELL_MENU_REBUILD, "Full rebuild");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SHELL_MENU_SCAN, "Scan");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, "Separator");
					InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SHELL_MENU_EXIT, "Exit");

					SetForegroundWindow(hwndDlg);
					TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwndDlg, NULL);


				}
					return TRUE;
				};
				break;
			case WM_COMMAND:
				if (dialog)
					return dialog->OnCommand(wParam, lParam);
				break;
			case WM_CLOSE:
				PostQuitMessage(0);
				break;
			case WM_SYSCOMMAND:
				if ((wParam == SC_MINIMIZE))
				{
					ShowWindow(hwndDlg, SW_HIDE);
					return TRUE;
				}
				break;
			};
			return FALSE;
		}
	}


	BuildServerDialog::BuildServerDialog(BuildServer* server) : _server(server), _cur_progress(0), _max_progress(0)
	{
		Assert(_server);

		const string& active_profile = _server->GetActiveProfile();

		map<string, BuildSettings*>::const_iterator prof_it, prof_end;
		prof_it = _server->GetSettingProfiles().begin(); prof_end = _server->GetSettingProfiles().end();
		for (uint32_t profile_id = 0; prof_it != prof_end; ++prof_it, ++profile_id)
		{
			bool active = false;
			if (prof_it->first == active_profile)
			{
				_active_profile = profile_id;
				active = true;
			}

			_profiles.push_back(pair<string, bool>(prof_it->first, active));
		}

		_dialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), NULL, DialogProc);
		ShowWindow(_dialog, SW_SHOW);
		SetWindowLongPtr(_dialog, GWLP_USERDATA, (LONG_PTR)this);


		memset(&_nid, 0, sizeof(NOTIFYICONDATA));
		_nid.cbSize = sizeof(NOTIFYICONDATA);
		_nid.hWnd = _dialog;
		_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		_nid.uCallbackMessage = WM_SHELL_MESSAGE;
		_nid.uID = IDI_ICON1;
		_nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		// Tooltip
		strcpy(_nid.szTip, "Idle");

		Shell_NotifyIcon(NIM_ADD, &_nid);

		// Center window
		RECT rc;
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(_dialog, &rc);
		SetWindowPos(_dialog, 0, (w - rc.right) / 2, (h - rc.bottom) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		_status_text = GetDlgItem(_dialog, IDC_STATUS_TEXT);
		SetStatus("Idle");
	}
	BuildServerDialog::~BuildServerDialog()
	{
		Shell_NotifyIcon(NIM_DELETE, &_nid);
		DestroyWindow(_dialog);
	}

	BOOL BuildServerDialog::OnCommand(WPARAM wparam, LPARAM)
	{
		switch (LOWORD(wparam))
		{
		case SHELL_MENU_OPEN:
			ShowWindow(_dialog, SW_NORMAL);
			break;
		case IDC_FULL_REBUILD_BUTTON:
		case SHELL_MENU_REBUILD:
			_server->QueueFullRebuild();
			break;
		case IDC_SCAN_BUTTON2:
		case SHELL_MENU_SCAN:
			_server->QueueScan();
			break;
		case SHELL_MENU_EXIT:
			PostQuitMessage(0);
			break;
		case SHELL_SET_PROFILE:
		{
			uint16_t profile_id = HIWORD(wparam);
			_profiles[profile_id].second = true;
			_profiles[_active_profile].second = false;
			_server->SetActiveProfile(_profiles[profile_id].first);
			_active_profile = profile_id;
		}
		default:
			return FALSE;
		};
		return TRUE;
	}



	void BuildServerDialog::OnCompileBatch(AssetSource*, uint32_t num)
	{
		_cur_progress = 0;
		_max_progress = num;
	}
	void BuildServerDialog::OnCompile(const FilePath& source, const FilePath&)
	{
		string str;
		if (_max_progress != 0)
		{
			float progress = ((float)_cur_progress / (float)_max_progress);

			str = string_util::Format("%d%% - Compiling %s", int(progress * 100), source.c_str());

			if ((_cur_progress + 1) == _max_progress)
				_cur_progress = _max_progress = 0;
		}
		else
		{
			str = string_util::Format("Compiling %s", source.c_str());
		}

		SetStatus(str.c_str());
	}
	void BuildServerDialog::OnCompileSkip(const FilePath&, const FilePath&)
	{
		if (_max_progress != 0)
		{
			++_cur_progress;
			if (_cur_progress == _max_progress)
				_cur_progress = _max_progress = 0;

		}
		if (_max_progress == 0) // _max_progress == 0, means we have no batch builds in progress
			SetStatus("Idle");
	}
	void BuildServerDialog::OnCompileSuccessful(const FilePath&, const FilePath&)
	{
		if (_max_progress == 0) // _max_progress == 0, means we have no batch builds in progress
			SetStatus("Idle");
		else
			++_cur_progress;
	}
	void BuildServerDialog::OnCompileFailed(const FilePath&, const FilePath&, const string&)
	{
		if (_max_progress == 0) // _max_progress == 0, means we have no batch builds in progress
			SetStatus("Idle");
		else
			++_cur_progress;
	}

	void BuildServerDialog::SetStatus(const char* msg)
	{
		Assert(_status_text != INVALID_HANDLE_VALUE);
		SetWindowText(_status_text, msg);


		strcpy_s(_nid.szTip, msg);
		Shell_NotifyIcon(NIM_MODIFY, &_nid);
	}

	const vector<pair<string, bool>>& BuildServerDialog::GetProfiles() const
	{
		return _profiles;
	}


} // namespace sb
