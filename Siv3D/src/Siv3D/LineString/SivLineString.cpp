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

# include <Siv3D/LineString.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/SequenceFormatter.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const LineString& LineString::draw(const ColorF& color) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, color);
	}

	const LineString& LineString::draw(const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, colorBegin, colorEnd);
	}

	const LineString& LineString::draw(const PatternParameters& pattern) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, pattern);
	}

	const LineString& LineString::draw(const double thickness, const ColorF& color) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, color);
	}

	const LineString& LineString::draw(const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, colorBegin, colorEnd);
	}

	const LineString& LineString::draw(const double thickness, const PatternParameters& pattern) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, pattern);
	}

	const LineString& LineString::draw(const LineCap lineCap, const double thickness, const ColorF& color) const
	{
		return draw(lineCap, lineCap, thickness, color);
	}

	const LineString& LineString::draw(const LineCap lineCap, const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		return draw(lineCap, lineCap, thickness, colorBegin, colorEnd);
	}

	const LineString& LineString::draw(const LineCap lineCap, const double thickness, const PatternParameters& pattern) const
	{
		return draw(lineCap, lineCap, thickness, pattern);
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			color.toFloat4());

		return *this;
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			colorBegin.toFloat4(),
			colorEnd.toFloat4());

		return *this;
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, const double thickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			pattern);

		return *this;
	}

	const LineString& LineString::draw(const std::span<const ColorF> colors) const
	{
		return draw(LineCap::Square, LineCap::Square, 1.0, colors);
	}

	const LineString& LineString::draw(const double thickness, const std::span<const ColorF> colors) const
	{
		return draw(LineCap::Square, LineCap::Square, thickness, colors);
	}

	const LineString& LineString::draw(const LineCap lineCap, double thickness, std::span<const ColorF> colors) const
	{
		return draw(lineCap, lineCap, thickness, colors);
	}

	const LineString& LineString::draw(const LineCap startCap, const LineCap endCap, double thickness, std::span<const ColorF> colors) const
	{
		SIV3D_ENGINE(Renderer2D)->addLineString(startCap, endCap,
			m_points, none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::No,
			colors);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const LineString& value)
	{
		Formatter(formatData, std::span<const Vec2>(value.m_points));
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::LineString, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::LineString, s3d::char32>::format(const s3d::LineString& value, s3d::BufferContext& ctx)
{
	return s3d::FmtHelper::FormatSequence(tag, std::span(value), ctx);
}
