// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDUICALLBACK_H__
#define __BUILDUICALLBACK_H__


#include "CompilerSystem.h"
#include <Foundation/Filesystem/FilePath.h>

namespace sb
{

	/// Interface for callbacks from the build server
	/// @remark All calls will be made from the thread that the build server runs on.
	class BuildUICallback
	{
	public:
		virtual ~BuildUICallback() {}

		/// Called before dispatching a batch of sources to the compiler
		virtual void OnCompileBatch(AssetSource* sources, uint32_t num) = 0;

		/// Called before compiler starts compile
		virtual void OnCompile(const FilePath& source, const FilePath& target) = 0;

		/// Called when a asset is skipped in the compilation process
		virtual void OnCompileSkip(const FilePath& source, const FilePath& target) = 0;

		/// Called when a resource was successfully compiled
		virtual void OnCompileSuccessful(const FilePath& source, const FilePath& target) = 0;

		/// Called when a resource compiled
		virtual void OnCompileFailed(const FilePath& source, const FilePath& target, const string& error) = 0;


	};

} // namespace sb


#endif // __BUILDERCALLBACK_H__

