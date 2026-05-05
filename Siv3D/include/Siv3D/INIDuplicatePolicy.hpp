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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INIDuplicateSectionPolicy
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI 読み込み時に重複するセクションを検出した場合の扱い | Policy for duplicate sections while reading INI
	enum class INIDuplicateSectionPolicy : uint8
	{
		/// @brief 同じ名前のセクションを 1 つのセクションとして結合します。 | Merges sections with the same name.
		Merge,

		/// @brief エラーにします。 | Treats duplicate sections as an error.
		Error,
	};

	////////////////////////////////////////////////////////////////
	//
	//	INIDuplicateKeyPolicy
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI 読み込み時に重複するキーを検出した場合の扱い | Policy for duplicate keys while reading INI
	enum class INIDuplicateKeyPolicy : uint8
	{
		/// @brief 後から出現した値で上書きします。 | Overwrites with the later value.
		Overwrite,

		/// @brief 最初に出現した値を保持します。 | Keeps the first value.
		KeepFirst,

		/// @brief エラーにします。 | Treats duplicate keys as an error.
		Error,
	};
}
