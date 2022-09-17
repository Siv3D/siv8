//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

//////////////////////////////////////////////////
//
//	プラットフォーム | Target platform
//
//	SIV3D_PLATFORM_NAME
//	SIV3D_PLATFORM(WINDOWS)
//	SIV3D_PLATFORM(MACOS)
//	SIV3D_PLATFORM(LINUX)
//	SIV3D_PLATFORM(WEB)
// 
//////////////////////////////////////////////////

# define SIV3D_PLATFORM(X) SIV3D_PLATFORM_PRIVATE_DEFINITION_##X()
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WINDOWS()	0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_MACOS()		0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_LINUX()		0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WEB()		0

# if defined(_WIN32) // Windows

	# define SIV3D_PLATFORM_NAME	U"Windows Desktop"
	# undef	 SIV3D_PLATFORM_PRIVATE_DEFINITION_WINDOWS
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WINDOWS()	1

# elif (defined(__APPLE__) && defined(__MACH__)) // macOS

	# define SIV3D_PLATFORM_NAME	U"macOS"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_MACOS
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_MACOS()		1

# elif defined(__linux__) // Linux

	# define SIV3D_PLATFORM_NAME	U"Linux"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_LINUX
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_LINUX()		1

# elif defined(__EMSCRIPTEN__) // Web

	# define SIV3D_PLATFORM_NAME	U"Web"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_WEB
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WEB()		1

# else

	# error Unknown platform

# endif

//////////////////////////////////////////////////
//
//	CPU アーキテクチャ | CPU architecture
//
//	SIV3D_CPU_ARCHITECTURE
//	SIV3D_CPU(X86_64)
//	SIV3D_CPU(ARM32)
//	SIV3D_CPU(ARM64)
// 
//////////////////////////////////////////////////

# define SIV3D_CPU(X) SIV3D_CPU_PRIVATE_DEFINITION_##X()
# define SIV3D_CPU_PRIVATE_DEFINITION_X86_64()		0
# define SIV3D_CPU_PRIVATE_DEFINITION_ARM32()		0
# define SIV3D_CPU_PRIVATE_DEFINITION_ARM64()		0

# if (defined(__x86_64__) || defined(_M_X64)) // x86-64

	# define SIV3D_CPU_ARCHITECTURE	U"x86-64"
	# undef	 SIV3D_CPU_PRIVATE_DEFINITION_X86_64
	# define SIV3D_CPU_PRIVATE_DEFINITION_X86_64()	1

# elif defined(__arm__) // ARM 32-bit

	# define SIV3D_CPU_ARCHITECTURE	U"ARM 32-bit"
	# undef	 SIV3D_CPU_PRIVATE_DEFINITION_ARM32
	# define SIV3D_CPU_PRIVATE_DEFINITION_ARM32()	1

# elif defined(__aarch64__) // ARM64

	# define SIV3D_CPU_ARCHITECTURE	U"ARM64"
	# undef	 SIV3D_CPU_PRIVATE_DEFINITION_ARM64
	# define SIV3D_CPU_PRIVATE_DEFINITION_ARM64()	1

# else

	# error Unknown processor

# endif

//////////////////////////////////////////////////
//
//	命令セット | Intrinsics
//
//	SIV3D_INTRINSIC_TYPE
//	SIV3D_INTRINSIC(SSE)
//	SIV3D_INTRINSIC(NEON)
// 
//////////////////////////////////////////////////

# define SIV3D_INTRINSIC(X) SIV3D_INTRINSIC_PRIVATE_DEFINITION_##X()
# define SIV3D_INTRINSIC_PRIVATE_DEFINITION_SSE()	0
# define SIV3D_INTRINSIC_PRIVATE_DEFINITION_NEON()	0

# if defined(_M_X64)
	# define __SSE__		1
	# define __SSE2__		1
	# define __SSE3__		1
	# define __SSSE3__		1
	# define __SSE4_1__		1
	# define __SSE4_2__		1
# endif

# if (defined(__SSE4_2__) && __SSE4_2__) // SSE4.2 を利用できる

	# define SIV3D_INTRINSIC_TYPE	U"SSE4.2"
	# undef  SIV3D_INTRINSIC_PRIVATE_DEFINITION_SSE
	# define SIV3D_INTRINSIC_PRIVATE_DEFINITION_SSE()	1

# elif (defined(__ARM_NEON__) || defined(__ARM_NEON)) // ARM NEON を利用できる

	# define SIV3D_INTRINSIC_TYPE	U"NEON"
	# undef  SIV3D_INTRINSIC_PRIVATE_DEFINITION_NEON
	# define SIV3D_INTRINSIC_PRIVATE_DEFINITION_NEON()	1

# endif

//////////////////////////////////////////////////
//
//	コンパイラ | Compilers
//
//	SIV3D_COMPILER_NAME
//	SIV3D_BUILD(MSVC)
//	SIV3D_BUILD(APPLECLANG)
//	SIV3D_BUILD(CLANG)
//	SIV3D_BUILD(GCC)
// 
//////////////////////////////////////////////////

# define SIV3D_COMPILER(X) SIV3D_COMPILER_PRIVATE_DEFINITION_##X()
# define SIV3D_COMPILER_PRIVATE_DEFINITION_MSVC()		0
# define SIV3D_COMPILER_PRIVATE_DEFINITION_APPLECLANG()	0
# define SIV3D_COMPILER_PRIVATE_DEFINITION_CLANG()		0
# define SIV3D_COMPILER_PRIVATE_DEFINITION_GCC()		0

# if defined(_MSC_VER) // MSVC

	# define SIV3D_COMPILER_NAME	U"Microsoft Visual C++"
	# undef	 SIV3D_COMPILER_PRIVATE_DEFINITION_MSVC
	# define SIV3D_COMPILER_PRIVATE_DEFINITION_MSVC()	1

# elif (defined(__clang__) && defined(__apple_build_version__)) // Apple Clang

	# define SIV3D_COMPILER_NAME	U"Apple Clang"
	# undef	 SIV3D_COMPILER_PRIVATE_DEFINITION_APPLECLANG
	# define SIV3D_COMPILER_PRIVATE_DEFINITION_APPLECLANG()	1

# elif defined(__clang__) // Clang

	# define SIV3D_COMPILER_NAME	U"Clang"
	# undef	 SIV3D_COMPILER_PRIVATE_DEFINITION_CLANG
	# define SIV3D_COMPILER_PRIVATE_DEFINITION_CLANG()	1

# elif defined(__GNUC__) // GCC

	# define SIV3D_COMPILER_NAME	U"GCC"
	# undef	 SIV3D_COMPILER_PRIVATE_DEFINITION_GCC
	# define SIV3D_COMPILER_PRIVATE_DEFINITION_GCC()	1

# else

	# error Unknown compiler

# endif

# if (SIV3D_COMPILER(MSVC) && (_MSC_VER < 1934))

	// Visual Studio のバージョンが古い場合、このエラーが発生します
	// This error occures when your Visual Studio version is not up to date.
	# error Please update the Visual Studio. Visual Studio 2022 17.4 is required to build this project.

# endif

//////////////////////////////////////////////////
//
//	ビルド設定 | Build type
//
//	SIV3D_BUILD_TYPE
//	SIV3D_BUILD(DEBUG)
//	SIV3D_BUILD(RELEASE)
// 
//////////////////////////////////////////////////

# define SIV3D_BUILD(X) SIV3D_BUILD_PRIVATE_DEFINITION_##X()
# define SIV3D_BUILD_PRIVATE_DEFINITION_DEBUG()		0
# define SIV3D_BUILD_PRIVATE_DEFINITION_RELEASE()	0

# if (defined(_DEBUG) || defined(DEBUG)) // Debug Build

	# define SIV3D_BUILD_TYPE	U"Debug"
	# undef  SIV3D_BUILD_PRIVATE_DEFINITION_DEBUG
	# define SIV3D_BUILD_PRIVATE_DEFINITION_DEBUG()		1

# else // Release Build

	# define SIV3D_BUILD_TYPE	U"Release"
	# undef  SIV3D_BUILD_PRIVATE_DEFINITION_RELEASE
	# define SIV3D_BUILD_PRIVATE_DEFINITION_RELEASE()	1

# endif

//////////////////////////////////////////////////
//
//	アロケータ | Default allocator
//
//	SIV3D_USE_MIMALLOC
//
//////////////////////////////////////////////////

# if (SIV3D_PLATFORM(WINDOWS))

	# define SIV3D_USE_MIMALLOC	1

# else

	# define SIV3D_USE_MIMALLOC	0

# endif

//////////////////////////////////////////////////
//
//	ローカルシンボル | Local symbol
//
//	SIV3D_LOCAL_SYMBOL
//
//////////////////////////////////////////////////

# if (SIV3D_COMPILER(APPLECLANG) || SIV3D_COMPILER(CLANG) || SIV3D_COMPILER(GCC))

	# define SIV3D_LOCAL_SYMBOL	__attribute__((visibility("hidden")))

# else

	# define SIV3D_LOCAL_SYMBOL

# endif

//////////////////////////////////////////////////
//
//	vtable の最適化 | vtable optimization
//
//	SIV3D_NOVTABLE
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC)

	# define SIV3D_NOVTABLE __declspec(novtable)

# else

	# define SIV3D_NOVTABLE

# endif

//////////////////////////////////////////////////
//
//	アロケータへの修飾 | Allocator attributes
//
//	SIV3D_RESTRICT
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC)

	# define SIV3D_RESTRICT __declspec(allocator) __declspec(restrict)

# else

	# define SIV3D_RESTRICT

# endif

//////////////////////////////////////////////////
//
//	警告の抑制 | Warning suppression
//
//	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(warnings)
//	SIV3D_DISABLE_MSVC_WARNINGS_POP()
//	SIV3D_DISABLE_CLANG_WARNINGS_POP()
//	SIV3D_DISABLE_GCC_WARNINGS_POP()
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC)

    # define SIV3D_DISABLE_MSVC_WARNINGS_PUSH(warnings)	\
			 __pragma(warning(push))					\
			 __pragma(warning(disable: warnings))

	# define SIV3D_DISABLE_MSVC_WARNINGS_POP()			\
			 __pragma(warning(pop))

# else

	# define SIV3D_DISABLE_MSVC_WARNINGS_PUSH(warnings)
	# define SIV3D_DISABLE_MSVC_WARNINGS_POP()

# endif


# if SIV3D_COMPILER(CLANG)

	# define SIV3D_DISABLE_CLANG_WARNINGS_POP()\
		_Pragma("clang diagnostic pop")

# else

	# define SIV3D_DISABLE_CLANG_WARNINGS_POP()

# endif


# if SIV3D_COMPILER(GCC) 

	# define SIV3D_DISABLE_GCC_WARNINGS_POP()\
		_Pragma("GCC diagnostic pop")

# else

	# define SIV3D_DISABLE_GCC_WARNINGS_POP()

# endif

//////////////////////////////////////////////////
//
//	実行ファイル埋め込みリソース | Embedded Resource
//
//	bool Platform::HasEmbeddedResource;
// 
//////////////////////////////////////////////////

namespace s3d::Platform
{
# if SIV3D_PLATFORM(WINDOWS)

	/// @brief ファイルを実行ファイルに埋め込み可能であるか
	inline constexpr bool HasEmbeddedResource = true;

# else

	/// @brief ファイルを実行ファイルに埋め込み可能であるか
	inline constexpr bool HasEmbeddedResource = false;

# endif
}
