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
# include <stdexcept>
# include "String.hpp"

namespace s3d
{
	/// @brief エラー | Error
	class Error : public std::exception
	{
	public:

		[[nodiscard]]
		Error() = default;

		[[nodiscard]]
		explicit Error(const char32* message);

		[[nodiscard]]
		explicit Error(StringView message);

		explicit Error(std::nullptr_t) = delete;

		virtual ~Error() = default;

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return エラーの種類 | The type of the error
		[[nodiscard]]
		virtual String type() const noexcept;

		/// @brief エラーメッセージを返します。 | Returns the error message.
		/// @return エラーメッセージ | The error message
		[[nodiscard]]
		virtual String message() const noexcept;

		friend std::ostream& operator <<(std::ostream& os, const Error& value);

		friend std::wostream& operator <<(std::wostream& os, const Error& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const Error& value);

		friend void Formatter(FormatData& formatData, const Error& value);

	protected:

		[[nodiscard]]
		const char* what() const noexcept override;
	};

	/// @brief パースのエラー | Parse error
	class ParseError final : public Error
	{
	public:

		using Error::Error;

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return U"ParseError"
		[[nodiscard]]
		String type() const noexcept override;
	};

	/// @brief 実装されていない機能を呼び出したエラー | Not implemented error
	class NotImplementedError final : public Error
	{
	public:

		using Error::Error;

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return U"NotImplementedError"
		[[nodiscard]]
		String type() const noexcept override;
	};

	/// @brief エンジン内部のエラー | Engine error
	class EngineError final : public Error
	{
	public:

		using Error::Error;

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return U"EngineError"
		[[nodiscard]]
		String type() const noexcept override;
	};
}
