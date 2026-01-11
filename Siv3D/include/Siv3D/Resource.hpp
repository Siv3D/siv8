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

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	EnumResourceFiles
	//
	////////////////////////////////////////////////////////////////

	/// @brief リソースファイルの一覧を取得します。
	/// @return リソースファイルの一覧
	[[nodiscard]]
	const Array<FilePath>& EnumResourceFiles() noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Resource
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルパスをリソースパスに変換します。
	/// @param path ファイルパス
	/// @return リソースパス	
	[[nodiscard]]
	FilePath Resource(FilePathView path);

# if SIV3D_PLATFORM(WINDOWS)

	namespace Platform::Windows
	{
		inline constexpr char32 ResourcePathPrefix = U':';

		/// @brief リソースパスをリソース名に変換します（内部関数）
		/// @param resourcePath リソースパス
		/// @return リソース名
		/// @remark この関数は Siv3D エンジンの内部で使用されます。
		[[nodiscard]]
		std::wstring ToResourceName(FilePathView resourcePath);
	}

# endif
}
