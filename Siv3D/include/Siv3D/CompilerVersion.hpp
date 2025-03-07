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
# include <version>
# include "Platform.hpp"

// Visual Studio 2022 バージョン要件チェック
// Visual Studio 2022 version requirement check
# if (SIV3D_COMPILER(MSVC) && (_MSC_VER < 1944))

	# pragma message("Siv3D のビルドには Visual Studio 2022 version 17.14 以降が必要です。Visual Studio をアップデートするか、最新版をダウンロードしてください。")
	# error "Siv3D requires Visual Studio 2022 version 17.14 or later. Please update your Visual Studio installation or download the latest version."

# endif

// C++ 標準ライブラリのバージョン要件チェック
// C++ Standard Library version requirement check
# if (SIV3D_COMPILER(MSVC) && (_MSVC_STL_UPDATE < 202501L))

	# pragma message("Siv3D のビルドには最新の C++ 標準ライブラリが必要です。Visual Studio Installer を開き、「C++ によるデスクトップ開発」ワークロードから最新の C++ x64/x86 ビルドツールをインストールしてください。")
	# error "Siv3D requires the latest C++ Standard Library. Please open Visual Studio Installer, select the 'Desktop development with C++' workload, and install the latest C++ x64/x86 build tools."

# endif
