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
# include <stdexcept>
# include "StringView.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Error
	//
	////////////////////////////////////////////////////////////////

	/// @brief エラー | Error
	class Error : public std::runtime_error
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		[[nodiscard]]
		explicit Error(const char* message);

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		[[nodiscard]]
		explicit Error(std::string_view message);

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		[[nodiscard]]
		explicit Error(const std::string& message);

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		[[nodiscard]]
		explicit Error(const char32* message);

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		[[nodiscard]]
		explicit Error(StringView message);

		explicit Error(std::nullptr_t) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~Error() = default;

		////////////////////////////////////////////////////////////////
		//
		//	type
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return エラーの種類 | The type of the error
		[[nodiscard]]
		virtual String type() const;

		////////////////////////////////////////////////////////////////
		//
		//	message
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーメッセージを返します。 | Returns the error message.
		/// @return エラーメッセージ | The error message
		[[nodiscard]]
		virtual String message() const;

		////////////////////////////////////////////////////////////////
		//
		//	messageUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーメッセージを UTF-8 文字列で返します。 | Returns the error message as a UTF-8 string.
		/// @return エラーメッセージ | The error message
		[[nodiscard]]
		virtual std::string messageUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		friend std::ostream& operator <<(std::ostream& output, const Error& value);

		friend std::wostream& operator <<(std::wostream& output, const Error& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const Error& value);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーを文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value エラー
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Error& value);

	protected:

		////////////////////////////////////////////////////////////////
		//
		//	what
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const char* what() const noexcept override;
	};
}
