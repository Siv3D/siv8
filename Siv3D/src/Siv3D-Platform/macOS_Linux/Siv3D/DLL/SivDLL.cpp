//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/DLL.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace DLL
	{
		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		LibraryHandle Load(const StringView path)
		{
			LOG_DEBUG(fmt::format("DLL::Load(\"path = `{}`\")", path));

			return ::dlopen(Unicode::ToUTF8(path).c_str(), RTLD_LOCAL | RTLD_LAZY);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Unload
		//
		////////////////////////////////////////////////////////////////

		void Unload(LibraryHandle& library)
		{
			LOG_DEBUG("DLL::Unload()");
			
			if (not library)
			{
				return;
			}
			
			::dlclose(library);
			library = nullptr;
		}

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		GetFunctionNoThrow::GetFunctionNoThrow(LibraryHandle module, const char* name)
			: p{ reinterpret_cast<void*>(::dlsym(module, name)) }
		{
			LOG_DEBUG(fmt::format("DLL::GetFunctionNoThrow::GetFunctionNoThrow(name = \"{0}\") p = {1}", name, p));
		}

		GetFunction::GetFunction(LibraryHandle module, const char* name)
			: p{ reinterpret_cast<void*>(::dlsym(module, name)) }
		{
			LOG_DEBUG(fmt::format("DLL::GetFunction::GetFunction(name = \"{0}\") p = {1}", name, p));

			if (not p)
			{
				throw InternalEngineError{ fmt::format("Failed to get function: `{}`", name) };
			}
		}
	}
}
