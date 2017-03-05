// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "BuildProgressDialog.h"
#include "resource.h"

#include <CommCtrl.h>

namespace sb
{

	namespace
	{
		INT_PTR CALLBACK DialogProc(
			_In_  HWND,
			_In_  UINT uMsg,
			_In_  WPARAM,
			_In_  LPARAM
			)
		{
			switch (uMsg)
			{
			case WM_CLOSE:
				PostQuitMessage(0);
				break;
			};
			return FALSE;
		}
	}



	BuildProgressDialog::BuildProgressDialog()
		: _cur_progress(0), _max_progress(0)
	{
		_dialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
		ShowWindow(_dialog, SW_SHOW);

		// Center window
		RECT rc;
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(_dialog, &rc);
		SetWindowPos(_dialog, 0, (w - rc.right) / 2, (h - rc.bottom) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);


		_progress_bar = GetDlgItem(_dialog, IDC_PROGRESS2);
		_text_box = GetDlgItem(_dialog, IDC_TEXTBOX);

		SendMessage(_progress_bar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		SendMessage(_progress_bar, PBM_SETPOS, 0, 0);
		SetWindowText(_text_box, "Loading...");
	}
	BuildProgressDialog::~BuildProgressDialog()
	{

	}

	void BuildProgressDialog::OnCompileBatch(AssetSource*, uint32_t num)
	{
		_cur_progress = 0;
		_max_progress = num;
		SetProgressRange(0, num);
		SetProgressPos(0);
		SetStatusText("Compiling...");
	}
	void BuildProgressDialog::OnCompile(const FilePath& source, const FilePath&)
	{
		SetStatusText(source.c_str());
	}
	void BuildProgressDialog::OnCompileSkip(const FilePath&, const FilePath&)
	{
	}
	void BuildProgressDialog::OnCompileSuccessful(const FilePath&, const FilePath&)
	{
		SetProgressPos(++_cur_progress);
	}
	void BuildProgressDialog::OnCompileFailed(const FilePath&, const FilePath&, const string&)
	{
		SetProgressPos(++_cur_progress);
	}

	void BuildProgressDialog::SetProgressRange(uint32_t min, uint32_t max)
	{
		SendMessage(_progress_bar, PBM_SETRANGE, 0, MAKELPARAM(min, max));
	}
	void BuildProgressDialog::SetProgressPos(uint32_t pos)
	{
		SendMessage(_progress_bar, PBM_SETPOS, pos, 0);
	}

	void BuildProgressDialog::SetStatusText(const char* txt)
	{
		SetWindowText(_text_box, txt);
	}


} // namespace sb
