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
	namespace FmtHelper
	{		
		////////////////////////////////////////////////////////////////
		//
		//	GetFormatTag
		//
		////////////////////////////////////////////////////////////////

		constexpr fmt::format_parse_context::iterator GetFormatTag(std::string& representation, fmt::format_parse_context& parseContext)
		{
			auto it = parseContext.begin();
			const auto itEnd = parseContext.end();

			while ((it != itEnd) && (*it != '}'))
			{
				representation.push_back(*it++);
			}

			return it;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FormatSequence
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		auto FormatSequence(const std::u32string& tag, const std::span<Type> sequence, auto& ctx)
		{
			if (sequence.empty())
			{
				return fmt::format_to(ctx.out(), U"[]");
			}

			const std::u32string elementTag = (tag.empty() ? U"{}" : (U"{:" + tag + U"}"));

			auto it = fmt::format_to(ctx.out(), U"[");

			bool isFirst = true;

			for (const auto& elem : sequence)
			{
				if (isFirst)
				{
					isFirst = false;
				}
				else
				{
					it = fmt::format_to(it, U", ");
				}

				it = fmt::format_to(it, elementTag, elem);
			}

			return fmt::format_to(it, U"]");
		}
	}
}