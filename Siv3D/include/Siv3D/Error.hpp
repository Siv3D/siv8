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
# include <source_location>
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
		/// @param location エラーの発生箇所 | Error location
		[[nodiscard]]
		explicit Error(const char* message, const std::source_location& location = std::source_location::current());

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		/// @param location エラーの発生箇所 | Error location
		[[nodiscard]]
		explicit Error(std::string_view message, const std::source_location& location = std::source_location::current());

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		/// @param location エラーの発生箇所 | Error location
		[[nodiscard]]
		explicit Error(const std::string& message, const std::source_location& location = std::source_location::current());

		/// @brief メッセージ付きのエラーを作成します。 | Creates an error with a message.
		/// @param message エラーメッセージ | Error message
		/// @param location エラーの発生箇所 | Error location
		[[nodiscard]]
		explicit Error(StringView message, const std::source_location& location = std::source_location::current());

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
		//	location
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーの発生位置情報を返します。 | Returns the source location where the error occurred.
		/// @return エラーの発生位置情報 | Source location information
		[[nodiscard]]
		const std::source_location& location() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	locationString
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーの発生位置情報を文字列で返します。 | Returns the source location where the error occurred as a string.
		/// @return エラーの発生位置情報 | Source location information
		[[nodiscard]]
		virtual String locationString() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	locationStringUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーの発生位置情報を UTF-8 文字列で返します。 | Returns the source location where the error occurred as a UTF-8 string.
		/// @return エラーの発生位置情報 | Source location information
		[[nodiscard]]
		virtual std::string locationStringUTF8() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーを出力します。 | Outputs the error.
		/// @param output 出力先 | Output destination
		/// @param value エラー | Error
		/// @return 出力先 | Output destination
		friend std::ostream& operator <<(std::ostream& output, const Error& value);

		/// @brief エラーを出力します。 | Outputs the error.
		/// @param output 出力先 | Output destination
		/// @param value エラー | Error
		/// @return 出力先 | Output destination
		friend std::wostream& operator <<(std::wostream& output, const Error& value);

		/// @brief エラーを出力します。 | Outputs the error.
		/// @param output 出力先 | Output destination
		/// @param value エラー | Error
		/// @return 出力先 | Output destination
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

		/// @brief エラーメッセージを返します。 | Returns the error message.
		/// @return エラーメッセージ | The error message
		[[nodiscard]]
		const char* what() const noexcept override;

		std::source_location m_sourceLocation;
	};
}
