//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "Siv3DTest.hpp"
# include <sstream>

namespace s3d::ImageDrawingTest
{
	inline Image Background(const Size size)
	{
		Image image{ size };
		size_t i = 0;
		for (auto& pixel : image)
		{
			pixel = Color{ uint8(i * 73), uint8(i * 31), uint8(i * 17), uint8(i * 23) };
			++i;
		}
		return image;
	}

	// Independent scalar straight-alpha oracle, including byte rounding.
	// Do not use ImagePixel or ImageDraw to calculate reference results.
	inline Color Composite(const Color src, const Color dst, const uint8 coverage, const bool paint)
	{
		if (!paint)
		{
			const auto channel = [&](uint32 s, uint32 d) { return (s * coverage + d * (255 - coverage) + 127) / 255; };
			return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), channel(src.a, dst.a) };
		}
		const uint32 alpha = (src.a * coverage + 127) / 255;
		if (alpha == 0) return dst;
		const uint32 denominator = alpha * 255 + dst.a * (255 - alpha);
		const auto channel = [&](uint32 s, uint32 d) { return (s * alpha * 255 + d * dst.a * (255 - alpha) + denominator / 2) / denominator; };
		return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), (denominator + 127) / 255 };
	}

	// maskOrigin is the mask pixel corresponding to destination (0, 0).
	// Only mask alpha is coverage; apply once per shape, or once per ordered member.
	inline void CompositeMask(Image& destination, const Image& mask, const Color ink,
		const bool paint, const Point maskOrigin = { 0, 0 })
	{
		REQUIRE((0 <= maskOrigin.x && 0 <= maskOrigin.y
			&& maskOrigin.x <= mask.width() && maskOrigin.y <= mask.height()
			&& destination.width() <= mask.width() - maskOrigin.x
			&& destination.height() <= mask.height() - maskOrigin.y));
		for (int32 y = 0; y < destination.height(); ++y)
			for (int32 x = 0; x < destination.width(); ++x)
				destination[y][x] = Composite(ink, destination[y][x], mask[y + maskOrigin.y][x + maskOrigin.x].a, paint);
	}

	// Geometry and boundary inclusion belong to the caller's independent predicate.
	template <class Contains>
	uint8 SampleCoverage(const Point pixel, const bool aa, const Contains& contains)
	{
		const int32 samples = aa ? 4 : 1;
		int32 covered = 0;
		for (int32 sy = 0; sy < samples; ++sy)
			for (int32 sx = 0; sx < samples; ++sx)
				covered += contains(Vec2{ pixel.x + (sx + 0.5) / samples, pixel.y + (sy + 0.5) / samples });
		return static_cast<uint8>((covered * 255 + samples * samples / 2) / (samples * samples));
	}

	template <class Contains>
	Image SampleMask(const Size size, const bool aa, const Contains& contains)
	{
		Image image{ size, Color{ 0, 0 } };
		for (int32 y = 0; y < size.y; ++y)
			for (int32 x = 0; x < size.x; ++x)
			{
				const uint8 coverage = SampleCoverage({ x, y }, aa, contains);
				image[y][x] = Color{ coverage, coverage };
			}
		return image;
	}

	// Use with CHECK_THAT / REQUIRE_THAT to retain the caller's assertion location.
	// The expected image must outlive the matcher. Comparison is byte-exact RGBA.
	class ImageEquals : public Catch::Matchers::MatcherBase<Image>
	{
	public:

		explicit ImageEquals(const Image& expected)
			: m_expected{ expected } {}

		bool match(const Image& actual) const override
		{
			m_difference.clear();
			if (actual == m_expected) return true;
			std::ostringstream message;
			if (actual.size() != m_expected.size())
			{
				message << "actual size " << actual.width() << 'x' << actual.height();
			}
			else
			{
				for (int32 y = 0; y < actual.height(); ++y)
					for (int32 x = 0; x < actual.width(); ++x)
						if (actual[y][x] != m_expected[y][x])
						{
							const auto rgba = [&](Color c)
							{
								message << '(' << uint32(c.r) << ", " << uint32(c.g) << ", " << uint32(c.b) << ", " << uint32(c.a) << ')';
							};
							message << "first difference at (" << x << ", " << y << "): actual RGBA ";
							rgba(actual[y][x]);
							message << ", expected RGBA ";
							rgba(m_expected[y][x]);
							m_difference = message.str();
							return false;
						}
			}
			m_difference = message.str();
			return false;
		}

		std::string describe() const override
		{
			std::ostringstream message;
			message << "equals expected image " << m_expected.width() << 'x' << m_expected.height();
			if (!m_difference.empty()) message << "; " << m_difference;
			return message.str();
		}

	private:

		const Image& m_expected;
		mutable std::string m_difference;
	};
}
