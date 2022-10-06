//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "StringView.hpp"
# include "Array.hpp"

namespace s3d
{
	/// @brief エラーを表現する型
	class Error
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

		[[nodiscard]]
		StringView what() const noexcept;

		[[nodiscard]]
		virtual StringView type() const noexcept;

		friend std::ostream& operator <<(std::ostream& os, const Error& value);

		friend std::wostream& operator <<(std::wostream& os, const Error& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const Error& value);

		friend void Formatter(FormatData& formatData, const Error& value);

	protected:

		struct ErrorData
		{
			Array<char32> message; // String を使うよりも小さいコードを生成
		};

		ErrorData m_data;
	};

	/// @brief パースエラーを表現する型
	class ParseError final : public Error
	{
	public:

		using Error::Error;

		[[nodiscard]]
		StringView type() const noexcept override;
	};

	/// @brief 未実装の機能を使用したエラーを表現する型
	class NotImplementedError final : public Error
	{
	public:

		using Error::Error;

		[[nodiscard]]
		StringView type() const noexcept override;
	};

	/// @brief エンジン内部のエラーを表現する型
	class EngineError final : public Error
	{
	public:

		using Error::Error;

		[[nodiscard]]
		StringView type() const noexcept override;
	};
}

# include "detail/Error.ipp"
