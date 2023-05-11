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
# include "Common.hpp"
# include "FormatLiteral.hpp"

namespace s3d
{
	struct Point
	{
		/// @brief ベクトルの要素の型
		using value_type = int32;

		/// @brief ベクトルの次元数
		static constexpr size_t Dimension = 2;

		/// @brief ベクトルの X 成分
		value_type x;

		/// @brief ベクトルの Y 成分
		value_type y;
	};

	using Size = Point;

	struct Rect
	{
		using position_type = Point;

		using size_type = Point;

		using value_type = size_type::value_type;

		SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 長方形の左上の点の位置
			position_type pos;

			struct
			{
				/// @brief 長方形の左上の点の X 座標
				value_type x;

				/// @brief 長方形の左上の点の Y 座標
				value_type y;
			};
		};

		union
		{
			/// @brief 長方形のサイズ
			size_type size;

			struct
			{
				/// @brief 長方形の幅
				/// @remark この値が負の時の挙動は未規定です。
				value_type w;

				/// @brief 長方形の高さ
				/// @remark この値が負の時の挙動は未規定です。
				value_type h;
			};
		};

		SIV3D_DISABLE_MSVC_WARNINGS_POP()

		static constexpr Rect Empty() noexcept
		{
			return{ 0, 0, 0, 0 };
		}
	};
}

template <>
struct SIV3D_HIDDEN fmt::formatter<s3d::Point, s3d::char32>
{
	std::u32string tag;

	auto parse(fmt::basic_format_parse_context<s3d::char32>& ctx)
	{
		return s3d::GetFormatTag(tag, ctx);
	}

	auto format(const s3d::Point& value, auto& ctx)
	{
		if (tag.empty())
		{
			return fmt::format_to(ctx.out(), U"({}, {})", value.x, value.y);
		}
		else
		{
			const std::u32string format = (U"({:" + tag + U"}, {:" + tag + U"})");
			return fmt::format_to(ctx.out(), format, value.x, value.y);
		}
	}
};

template <>
struct SIV3D_HIDDEN fmt::formatter<s3d::Rect, s3d::char32>
{
	std::u32string tag;

	auto parse(fmt::basic_format_parse_context<s3d::char32>& ctx)
	{
		return s3d::GetFormatTag(tag, ctx);
	}

	auto format(const s3d::Rect& value, auto& ctx)
	{
		if (tag.empty())
		{
			return fmt::format_to(ctx.out(), U"({}, {}, {}, {})", value.x, value.y, value.w, value.h);
		}
		else
		{
			const std::u32string format = (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
			return fmt::format_to(ctx.out(), format, value.x, value.y);
		}
	}
};
