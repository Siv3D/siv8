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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ParseContext
	//
	////////////////////////////////////////////////////////////////

	using ParseContext = fmt::basic_format_parse_context<s3d::char32>;

	////////////////////////////////////////////////////////////////
	//
	//	BufferContext
	//
	////////////////////////////////////////////////////////////////

	using BufferContext = fmt::buffer_context<s3d::char32>;

	namespace FmtHelper
	{
		constexpr fmt::format_parse_context::iterator GetFormatTag(std::string& representation, fmt::format_parse_context& parseContext);

		ParseContext::iterator GetFormatTag(std::u32string& representation, ParseContext& parseContext);

		fmt::format_context::iterator FormatString(fmt::format_context& ctx, std::string&& tag, const s3d::StringView value);

		BufferContext::iterator FormatString(BufferContext& ctx, std::u32string&& tag, const s3d::StringView value);

		template <class Type>
		auto FormatSequence(const std::u32string& tag, const std::span<Type> sequence, auto& ctx);
	}
}
