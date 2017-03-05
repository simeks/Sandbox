// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDSERVERDIALOG_H__
#define __BUILDSERVERDIALOG_H__

#include "BuildUICallback.h"
#include <shellapi.h>

namespace sb
{

	class BuildServerDialog : public BuildUICallback
	{
	public:
		BuildServerDialog(BuildServer* server);
		~BuildServerDialog();

		BOOL OnCommand(WPARAM wparam, LPARAM lparam);

		/// Called before dispatching a batch of sources to the compiler
		void OnCompileBatch(AssetSource* sources, uint32_t num);

		/// Called before compiler starts compile
		void OnCompile(const FilePath& source, const FilePath& target);

		/// Called when a asset is skipped in the compilation process
		void OnCompileSkip(const FilePath& source, const FilePath& target);

		/// Called when a resource was successfully compiled
		void OnCompileSuccessful(const FilePath& source, const FilePath& target);

		/// Called when a resource compiled
		void OnCompileFailed(const FilePath& source, const FilePath& target, const string& error);

		const vector<pair<string, bool>>& GetProfiles() const;

	private:
		void SetStatus(const char* msg);

		BuildServer* _server;
		vector<pair<string, bool>> _profiles;
		uint32_t _active_profile;

		HWND _dialog;
		HWND _status_text;

		NOTIFYICONDATA _nid;

		uint32_t _cur_progress;
		uint32_t _max_progress;

	};

} // namespace sb



#endif // __BUILDSERVERDIALOG_H__


