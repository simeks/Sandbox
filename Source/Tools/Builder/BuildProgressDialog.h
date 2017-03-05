// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDERWINDOW_H__
#define __BUILDERWINDOW_H__

#include "BuildUICallback.h"

namespace sb
{

	class BuildProgressDialog : public BuildUICallback
	{
	public:
		BuildProgressDialog();
		~BuildProgressDialog();

		/// Called before dispatching a batch of sources to the compiler
		void OnCompileBatch(AssetSource* sources, uint32_t num);

		/// Called before compiler starts compile
		void OnCompile(const FilePath& source, const FilePath& target);

		void OnCompileSkip(const FilePath& source, const FilePath& target);

		/// Called when a resource was successfully compiled
		void OnCompileSuccessful(const FilePath& source, const FilePath& target);

		/// Called when a resource compiled
		void OnCompileFailed(const FilePath& source, const FilePath& target, const string& error);



		void SetProgressRange(uint32_t min, uint32_t max);
		void SetProgressPos(uint32_t pos);

		void SetStatusText(const char* txt);


	private:
		HWND _dialog;
		HWND _progress_bar;
		HWND _text_box;

		uint32_t _cur_progress;
		uint32_t _max_progress;

	};

} // namespace sb


#endif // __BUILDERWINDOW_H__

