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
# include "INIParseErrorCode.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INIParseErrorReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI のパースエラーの詳細 | Details of an INI parse error
	struct INIParseErrorReason
	{
		/// @brief エラーの種類 | Error code
		INIParseErrorCode code = INIParseErrorCode::InvalidReader;

		/// @brief エラーの概要 | Error title
		String title;

		/// @brief エラーの位置 | Error locations
		Array<String> locations;

		/// @brief エラー解決のヒント | Hint for resolving the error
		String hint;

		/// @brief 行番号（1 始まり） | Line number, starting at 1
		size_t line = 1;

		/// @brief 行内の文字位置（1 始まり） | Character position in the line, starting at 1
		size_t character = 1;

		/// @brief セクション名 | Section name
		String section;

		/// @brief キー | Key
		String key;

		/// @brief エラー情報を文字列に整形します。 | Formats the error details as a string.
		/// @return 整形された文字列 | Formatted string
		[[nodiscard]]
		String format() const
		{
			String result = title;

			for (const auto& location : locations)
			{
				if (result)
				{
					result.push_back(U'\n');
				}

				result.append(location);
			}

			if (hint)
			{
				if (result)
				{
					result.push_back(U'\n');
				}

				result.append(hint);
			}

			return result;
		}
	};
}
