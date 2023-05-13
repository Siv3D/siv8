//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	/// @brief パースに失敗した理由 | The reason for the parsing failure
	enum class ParseErrorReason
	{
		/// @brief 入力が空または空白文字のみである | The input is empty or consists of only whitespace characters
		EmptyInput,

		/// @brief 不正な基数である | The input contains an invalid radix
		InvalidRadix,
		
		/// @brief 不正な文字やフォーマットである | The input contains invalid characters or format
		InvalidFormat,
		
		/// @brief 数値が範囲外である | The input is out of range
		NumericOverflow,

		/// @brief 型に対する不適切な値である（unsigned int に対する負の数値など） | The input is an inappropriate value for the type (e.g. a negative number for unsigned int)
		InvalidValueForType,
	};
}
