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

# pragma once

//////////////////////////////////////////////////
//
//	プラットフォーム | Target platform
//
//	SIV3D_PLATFORM_NAME
//	SIV3D_PLATFORM(WINDOWS)
//	SIV3D_PLATFORM(MACOS)
//	SIV3D_PLATFORM(LINUX)	// ToDo
//	SIV3D_PLATFORM(IOS)		// ToDo
//	SIV3D_PLATFORM(ANDROID) // ToDo
//	SIV3D_PLATFORM(WEB)		// ToDo
// 
//////////////////////////////////////////////////

# define SIV3D_PLATFORM(X) SIV3D_PLATFORM_PRIVATE_DEFINITION_##X()
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WINDOWS()	0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_MACOS()		0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_LINUX()		0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_IOS()		0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_ANDROID()	0
# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WEB()		0

# if defined(_WIN32) // Windows

	# define SIV3D_PLATFORM_NAME	U"Windows Desktop"
	# undef	 SIV3D_PLATFORM_PRIVATE_DEFINITION_WINDOWS
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WINDOWS()	1

# elif defined(__APPLE__)

	#include <TargetConditionals.h>

	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR

	# define SIV3D_PLATFORM_NAME	U"iOS"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_IOS
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_IOS()		1

	# elif TARGET_OS_MAC
	
	# define SIV3D_PLATFORM_NAME	U"macOS"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_MACOS
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_MACOS()		1
	
	# else

	#error "Unknown Apple platform"

	# endif

# elif defined(__ANDROID__)

	# define SIV3D_PLATFORM_NAME	U"Android"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_ANDROID
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_ANDROID()	1

# elif defined(__linux__)

	# define SIV3D_PLATFORM_NAME	U"Linux"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_LINUX
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_LINUX()		1

# elif defined(__EMSCRIPTEN__)

	# define SIV3D_PLATFORM_NAME	U"Web"
	# undef  SIV3D_PLATFORM_PRIVATE_DEFINITION_WEB
	# define SIV3D_PLATFORM_PRIVATE_DEFINITION_WEB()		1

# else

	# error "Unknown or unsupported platform"

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

	# error "Unknown processor"

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

# else

	# error "Unknown intrinsic"

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

	# error "Unknown compiler"

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

# if SIV3D_PLATFORM(WINDOWS)

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

# if (SIV3D_COMPILER(APPLECLANG) || SIV3D_COMPILER(CLANG) || SIV3D_COMPILER(GCC)) // Clang, GCC

	# define SIV3D_LOCAL_SYMBOL	__attribute__((visibility("hidden")))

# else // MSVC

	# define SIV3D_LOCAL_SYMBOL

# endif

//////////////////////////////////////////////////
//
//	vtable の最適化 | vtable optimization
//
//	SIV3D_NOVTABLE
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC) // MSVC

	// https://learn.microsoft.com/en-us/cpp/cpp/novtable
	# define SIV3D_NOVTABLE __declspec(novtable)

# else // Clang, GCC

	# define SIV3D_NOVTABLE

# endif

//////////////////////////////////////////////////
//
//	アロケータへの修飾 | Allocator attributes
//
//	SIV3D_RESTRICT
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC) // MSVC

	# define SIV3D_RESTRICT __declspec(allocator) __declspec(restrict)

# else // Clang, GCC

	# define SIV3D_RESTRICT

# endif

//////////////////////////////////////////////////
//
//	[[msvc::intrinsic]]
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC)

	// https://learn.microsoft.com/en-us/cpp/cpp/attributes?#msvcintrinsic
	# define SIV3D_MSVC_INTRINSIC [[msvc::intrinsic]]

# else

	# define SIV3D_MSVC_INTRINSIC

# endif

//////////////////////////////////////////////////
//
//	lifetimebound
//
//////////////////////////////////////////////////

# if SIV3D_COMPILER(MSVC)

	# define SIV3D_LIFETIMEBOUND [[msvc::lifetimebound]]

	# include <CppCoreCheck/Warnings.h>
	# pragma warning(default: CPPCORECHECK_LIFETIME_WARNINGS)

# elif SIV3D_COMPILER(CLANG)

	# define SIV3D_LIFETIMEBOUND [[clang::lifetimebound]]

# elif SIV3D_COMPILER(GCC)

	# define SIV3D_LIFETIMEBOUND __attribute__((lifetimebound))

# else

	# define SIV3D_LIFETIMEBOUND

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

# if SIV3D_COMPILER(MSVC) // MSVC

    # define SIV3D_DISABLE_MSVC_WARNINGS_PUSH(warnings)	\
			 __pragma(warning(push))					\
			 __pragma(warning(disable: warnings))

	# define SIV3D_DISABLE_MSVC_WARNINGS_POP()			\
			 __pragma(warning(pop))

# else // Clang, GCC

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
