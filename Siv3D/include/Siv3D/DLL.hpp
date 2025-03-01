//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "StringView.hpp"

# if SIV3D_PLATFORM(WINDOWS)

# define NOGDI
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	using LibraryHandle = HMODULE;

	namespace DLL
	{
		////////////////////////////////////////////////////////////////
		//
		//	LoadSystemLibraryNoThrow
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LibraryHandle LoadSystemLibraryNoThrow(const wchar_t* library);

		////////////////////////////////////////////////////////////////
		//
		//	LoadSystemLibrary
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LibraryHandle LoadSystemLibrary(const wchar_t* library);

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LibraryHandle Load(StringView path);

		////////////////////////////////////////////////////////////////
		//
		//	Unload
		//
		////////////////////////////////////////////////////////////////

		void Unload(LibraryHandle& library);

		////////////////////////////////////////////////////////////////
		//
		//	GetFunctionNoThrow
		//
		////////////////////////////////////////////////////////////////

		class GetFunctionNoThrow
		{
		public:

			[[nodiscard]]
			GetFunctionNoThrow(LibraryHandle _module, const char* name);

			template <class Type>
			[[nodiscard]]
			operator Type() const noexcept
			{
				return static_cast<Type>(p);
			}

		private:

			const void* p = nullptr;
		};

		////////////////////////////////////////////////////////////////
		//
		//	GetFunction
		//
		////////////////////////////////////////////////////////////////

		class GetFunction
		{
		public:

			[[nodiscard]]
			GetFunction(LibraryHandle _module, const char* name);

			template <class Type>
			[[nodiscard]]
			operator Type() const noexcept
			{
				return static_cast<Type>(p);
			}

		private:

			const void* p = nullptr;
		};
	}
}

# else

# include <dlfcn.h>

namespace s3d
{
	using LibraryHandle = void*;

	namespace DLL
	{
		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LibraryHandle Load(StringView path);
	
		////////////////////////////////////////////////////////////////
		//
		//	Unload
		//
		////////////////////////////////////////////////////////////////

		void Unload(LibraryHandle& library);

		////////////////////////////////////////////////////////////////
		//
		//	GetFunctionNoThrow
		//
		////////////////////////////////////////////////////////////////

		class GetFunctionNoThrow
		{
		public:

			[[nodiscard]]
			GetFunctionNoThrow(LibraryHandle _module, const char* name);

			template <class Type>
			operator Type() const noexcept
			{
				return reinterpret_cast<Type>(p);
			}

		private:

			void* p;
		};

		////////////////////////////////////////////////////////////////
		//
		//	GetFunction
		//
		////////////////////////////////////////////////////////////////

		class GetFunction
		{
		public:

			[[nodiscard]]
			GetFunction(LibraryHandle _module, const char* name);

			template <class Type>
			operator Type() const noexcept
			{
				return reinterpret_cast<Type>(p);
			}

		private:

			void* p = nullptr;
		};
	}
}

# endif
