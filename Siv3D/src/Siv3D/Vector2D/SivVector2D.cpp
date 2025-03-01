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

# include <Siv3D/PointVector.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/2DShapes.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	clamp, clamped
	//
	////////////////////////////////////////////////////////////////

	template <>
	Vector2D<float> Vector2D<float>::clamped(const RectF& rect) const noexcept
	{
		return{
			Clamp(x, static_cast<value_type>(rect.x), static_cast<value_type>(rect.x + rect.w)),
			Clamp(y, static_cast<value_type>(rect.y), static_cast<value_type>(rect.y + rect.h)) };
	}

	template <>
	Vector2D<double> Vector2D<double>::clamped(const RectF& rect) const noexcept
	{
		return{
			Clamp(x, rect.x, (rect.x + rect.w)),
			Clamp(y, rect.y, (rect.y + rect.h)) };
	}

	template <>
	Vector2D<float>& Vector2D<float>::clamp(const RectF& rect) noexcept
	{
		x = Clamp(x, static_cast<value_type>(rect.x), static_cast<value_type>(rect.x + rect.w));
		y = Clamp(y, static_cast<value_type>(rect.y), static_cast<value_type>(rect.y + rect.h));
		return *this;
	}

	template <>
	Vector2D<double>& Vector2D<double>::clamp(const RectF& rect) noexcept
	{
		x = Clamp(x, rect.x, (rect.x + rect.w));
		y = Clamp(y, rect.y, (rect.y + rect.h));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	asCircle
	//
	////////////////////////////////////////////////////////////////

	template <>
	Circle Vector2D<float>::asCircle(const double r) const noexcept
	{
		return{ *this, r };
	}

	template <>
	Circle Vector2D<double>::asCircle(const double r) const noexcept
	{
		return{ *this, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	_Formatter
	//
	////////////////////////////////////////////////////////////////

	template <>
	void Vector2D<double>::_Formatter(FormatData& formatData, const Vector2D<double>& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.push_back(U')');
	}

	template <>
	void Vector2D<float>::_Formatter(FormatData& formatData, const Vector2D<float>& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.push_back(U')');
	}

	template struct Vector2D<float>;
	template struct Vector2D<double>;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Float2>::format(const s3d::Float2& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {})", value.x, value.y);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.x, value.y));
	}
}

fmt::format_context::iterator fmt::formatter<s3d::Vec2>::format(const s3d::Vec2& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {})", value.x, value.y);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.x, value.y));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Float2, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Float2, s3d::char32>::format(const s3d::Float2& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {})", value.x, value.y);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.x, value.y);
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Vec2, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Vec2, s3d::char32>::format(const s3d::Vec2& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {})", value.x, value.y);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.x, value.y);
	}
}
