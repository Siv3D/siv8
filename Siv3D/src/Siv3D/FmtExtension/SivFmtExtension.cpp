//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/FmtExtension.hpp>

namespace s3d
{
	namespace FmtHelper
	{
		ParseContext::iterator GetFormatTag(std::u32string& representation, ParseContext& parseContext)
		{
			auto it = parseContext.begin();
			const auto itEnd = parseContext.end();

			while ((it != itEnd) && (*it != '}'))
			{
				representation.push_back(*it++);
			}

			return it;
		}

		fmt::format_context::iterator FormatString(fmt::format_context& ctx, std::string&& tag, const s3d::StringView value)
		{
			const std::string utf8 = s3d::Unicode::ToUTF8(value);

			if (tag.empty())
			{
				return fmt::format_to(ctx.out(), "{}", utf8);
			}
			else
			{
				return fmt::vformat_to(ctx.out(), ("{:" + std::move(tag) + '}'), fmt::make_format_args(utf8));
			}
		}

		BufferContext::iterator FormatString(BufferContext& ctx, std::u32string&& tag, const s3d::StringView value)
		{
			return fmt::format_to(ctx.out(), (tag.empty() ? U"{}" : (U"{:" + std::move(tag) + U'}')),
				fmt::basic_string_view<s3d::char32>{ value.data(), value.size() });
		}
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::StringView, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::StringView, s3d::char32>::format(const s3d::StringView value, s3d::BufferContext& ctx)
{
	return s3d::FmtHelper::FormatString(ctx, std::move(tag), value);
}

s3d::ParseContext::iterator fmt::formatter<s3d::String, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::String, s3d::char32>::format(const s3d::String& value, s3d::BufferContext& ctx)
{
	return s3d::FmtHelper::FormatString(ctx, std::move(tag), value);
}
