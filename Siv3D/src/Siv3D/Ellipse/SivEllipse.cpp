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

# include <Siv3D/Ellipse.hpp>
# include <Siv3D/Utility.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool Ellipse::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Ellipse::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Ellipse::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Ellipse::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Ellipse::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Ellipse::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Ellipse::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::draw(const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(a)),
			Abs(static_cast<float>(b)),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(a)),
			Abs(static_cast<float>(b)),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(a)),
			Abs(static_cast<float>(b)),
			topColor->toFloat4(),
			bottomColor->toFloat4(),
			ColorFillDirection::TopBottom
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(a)),
			Abs(static_cast<float>(b)),
			leftColor->toFloat4(),
			rightColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(a)),
			Abs(static_cast<float>(b)),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawFrame(const double thickness, const ColorF& color) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), color);
	}

	const Ellipse& Ellipse::drawFrame(const double thickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), innerColor, outerColor);
	}

	const Ellipse& Ellipse::drawFrame(const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipseFrame(
			center,
			static_cast<float>(Abs(a) - innerThickness),
			static_cast<float>(Abs(b) - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			color0,
			color0
		);

		return *this;
	}

	const Ellipse& Ellipse::drawFrame(const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addEllipseFrame(
			center,
			static_cast<float>(Abs(a) - innerThickness),
			static_cast<float>(Abs(b) - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);

		return *this;
	}

	const Ellipse& Ellipse::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), pattern);
	}

	const Ellipse& Ellipse::drawFrame(const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addEllipseFrame(
			center,
			static_cast<float>(Abs(a) - innerThickness),
			static_cast<float>(Abs(b) - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Ellipse& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.a);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.b);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Ellipse>::format(const s3d::Ellipse& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", value.x, value.y, value.a, value.b);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.x, value.y, value.a, value.b));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Ellipse, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Ellipse, s3d::char32>::format(const s3d::Ellipse& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", value.x, value.y, value.a, value.b);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}), {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.x, value.y, value.a, value.b);
	}
}
