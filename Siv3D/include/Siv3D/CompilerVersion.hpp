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

// Visual Studio 2022 17.12 よりも古いバージョンへのエラーメッセージ
// Error message for older versions of Visual Studio 2022 17.12

# if (SIV3D_COMPILER(MSVC) && (_MSC_VER < 1942))

	# pragma message("Siv3D は Visual Studio 2022 version 17.12 以降が必要です。Visual Studio をアップデートしてください。")
	# error "Siv3D requires Visual Studio 2022 version 17.12 or later. Please update your Visual Studio installation."

# endif

// Visual Studio の C++ ビルドツールが古い場合のエラーメッセージ
// Error message for older versions of Visual Studio C++ build tools

# if (SIV3D_COMPILER(MSVC) && (_MSVC_STL_UPDATE < 202408L))

	# pragma message("Visual Studio の C++ 標準ライブラリが古いため、Siv3D のビルドができません。Visual Studio Installer から最新の C++ x64/x86 ビルドツールをインストールしてください。")
	# error "Siv3D cannot be built because the C++ Standard Library provided by Visual Studio is too old. Please install the latest C++ x64/x86 build tools from the Visual Studio Installer."

# endif
