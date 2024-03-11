//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
		LibraryHandle LoadSystemLibraryNoThrow(const wchar_t* library)
		{
			LOG_DEBUG(fmt::format("DLL::LoadSystemLibraryNoThrow(\"{0}\")", Unicode::FromWstring(library)));

			return ::LoadLibraryExW(library, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		}

		LibraryHandle LoadSystemLibrary(const wchar_t* library)
		{
			LOG_DEBUG(fmt::format("DLL::LoadSystemLibrary(\"{0}\")", Unicode::FromWstring(library)));

			const LibraryHandle hModule = ::LoadLibraryExW(library, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

			if (not hModule)
			{
				throw InternalEngineError{ fmt::format("Failed to load `{}`", Unicode::FromWstring(library)) };
			}

			return hModule;
		}

		LibraryHandle Load(const StringView path)
		{
			LOG_DEBUG(fmt::format("DLL::Load(\"path = `{}`\")", path));

			return ::LoadLibraryW(path.toWstr().c_str());
		}

		void Unload(LibraryHandle& library)
		{
			if (not library)
			{
				return;
			}

			::FreeLibrary(library);

			library = nullptr;
		}

		GetFunctionNoThrow::GetFunctionNoThrow(LibraryHandle _module, const char* name)
			: p{ static_cast<const void*>(::GetProcAddress(_module, name)) }
		{
			LOG_DEBUG(fmt::format("DLL::GetFunctionNoThrow::GetFunctionNoThrow(name = \"{0}\") p = {1}", name, p));
		}

		GetFunction::GetFunction(LibraryHandle _module, const char* name)
			: p(static_cast<const void*>(::GetProcAddress(_module, name)))
		{
			LOG_DEBUG(fmt::format("DLL::GetFunction::GetFunction(name = \"{0}\") p = {1}", name, p));

			if (not p)
			{
				throw InternalEngineError{ fmt::format("Failed to get function: `{}`", name) };
			}
		}
	}
}
