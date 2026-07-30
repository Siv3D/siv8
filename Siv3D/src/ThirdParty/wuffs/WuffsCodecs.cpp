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

# include <Siv3D/Platform.hpp>

# define WUFFS_IMPLEMENTATION

// Wuffs の全利用をこの translation unit 内に閉じ込める
# define WUFFS_CONFIG__STATIC_FUNCTIONS

// 必要なモジュールのみ
# define WUFFS_CONFIG__MODULES

# define WUFFS_CONFIG__MODULE__BASE
# define WUFFS_CONFIG__MODULE__ADLER32
# define WUFFS_CONFIG__MODULE__CRC32
# define WUFFS_CONFIG__MODULE__DEFLATE
# define WUFFS_CONFIG__MODULE__ZLIB
# define WUFFS_CONFIG__MODULE__GIF
# define WUFFS_CONFIG__MODULE__PNG

// GZIP は実装開始時に追加する
// # define WUFFS_CONFIG__MODULE__GZIP

// アニメーション合成用キャンバスは常に straight alpha の RGBA8
# define WUFFS_CONFIG__DST_PIXEL_FORMAT__ENABLE_ALLOWLIST
# define WUFFS_CONFIG__DST_PIXEL_FORMAT__ALLOW_RGBA_NONPREMUL

// Windows / cl.exe / x64。
// SSE4.2 + POPCNT + PCLMULQDQ の V2 パスを有効化。
// AVX2 の V3 パスは有効化しない。
# if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
#	define WUFFS_CONFIG__ENABLE_MSVC_CPU_ARCH__X86_64_V2
# endif

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4100)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4505)
#include "wuffs-v0.4.h"
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
