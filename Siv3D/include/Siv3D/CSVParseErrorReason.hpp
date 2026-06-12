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
# include "CSVParseErrorCode.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	CSVParseErrorReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief CSV のパースエラーの詳細 | Details of a CSV parse error
	struct CSVParseErrorReason
	{
		/// @brief エラーの種類 | Error code
		CSVParseErrorCode code = CSVParseErrorCode::InvalidReader;

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

		/// @brief レコード番号（0 始まり） | Record index, starting at 0
		size_t row = 0;

		/// @brief カラム番号（0 始まり） | Column index, starting at 0
		size_t column = 0;

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
