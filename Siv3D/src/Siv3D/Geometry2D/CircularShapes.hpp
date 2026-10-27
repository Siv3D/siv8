//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <cmath>
# include <Siv3D/Circle.hpp>
# include <Siv3D/Optional.hpp>

namespace s3d::detail::CircularShapes
{
	struct Crescent
	{
		Circle circle;
		Vec2 axis;
		double halfWidth = 0, halfChord = 0;
		bool circleOnly = false;
	};

	struct Teardrop
	{
		Circle circle;
		Vec2 axis, tip;
		double tangentCos = 0, tangentSin = 0, tangentAngle = 0;
		bool circleOnly = false;
	};

	// Inputs and intermediate geometry must be finite and representable.
	[[nodiscard]]
	inline Optional<Crescent> PlaceCrescent(const Circle& circle, const double openingAngle, const double width)
	{
		const double r = Abs(circle.r);
		if ((r == 0) || (width <= 0)) return none;
		Crescent result{ Circle{ circle.center, r }, {}, width * 0.5, 0, (2 * r <= width) };
		if (result.circleOnly) return result;
		result.axis = { std::sin(openingAngle), -std::cos(openingAngle) };
		const double ratio = (result.halfWidth / r);
		result.halfChord = (r * std::sqrt((1 - ratio) * (1 + ratio)));
		return result;
	}

	[[nodiscard]]
	inline Optional<Teardrop> PlaceTeardrop(const Circle& circle, const Vec2& tip)
	{
		const double r = Abs(circle.r);
		if (r == 0) return none;
		const Vec2 delta = (tip - circle.center);
		const double distance = delta.length();
		Teardrop result{ Circle{ circle.center, r }, {}, tip, 0, 0, 0, (distance <= r) };
		if (result.circleOnly) return result;
		result.axis = (delta / distance);
		result.tangentCos = (r / distance);
		result.tangentSin = std::sqrt((1 - result.tangentCos) * (1 + result.tangentCos));
		result.tangentAngle = std::acos(result.tangentCos);
		return result;
	}

	[[nodiscard]]
	inline uint16 CrescentIntervals(const Crescent& shape, const uint16 fullQuality)
	{
		// Uniform rays turn the outer circle by at most (1 + halfWidth / r)
		// times the ray angle. Match the circle's angular density within 252 steps.
		return static_cast<uint16>(std::ceil(fullQuality * 0.5 * (1 + shape.halfWidth / shape.circle.r)));
	}

	[[nodiscard]]
	inline uint16 TeardropIntervals(const Teardrop& shape, const uint16 fullQuality)
	{
		return static_cast<uint16>(std::ceil(fullQuality * (1 - shape.tangentAngle / Math::Pi)));
	}

	[[nodiscard]]
	inline uint16 VertexCount(const Crescent& shape, const uint16 fullQuality)
	{
		return (shape.circleOnly ? fullQuality : (CrescentIntervals(shape, fullQuality) * 2));
	}

	[[nodiscard]]
	inline uint16 VertexCount(const Teardrop& shape, const uint16 fullQuality)
	{
		return (shape.circleOnly ? fullQuality : (TeardropIntervals(shape, fullQuality) + 2));
	}

	// All emitted vertices form one clockwise boundary, with no interior vertex.
	// The same writers target Shape2D arrays and write-only renderer cursors.
	template <class Vertex, class Triangle>
	void WriteCircle(const Circle& circle, const uint16 count, Vertex&& vertex, Triangle&& triangle)
	{
		for (uint16 i = 0; i < count; ++i)
		{
			const double angle = (Math::TwoPi * i / count);
			vertex(i, Float2{ circle.center + Vec2{ std::sin(angle), -std::cos(angle) } * circle.r });
		}
		for (uint16 i = 1; i + 1 < count; ++i) triangle(0, i, (i + 1));
	}

	template <class Vertex, class Triangle>
	void Write(const Crescent& shape, const uint16 fullQuality, Vertex&& vertex, Triangle&& triangle)
	{
		if (shape.circleOnly)
		{
			WriteCircle(shape.circle, fullQuality, vertex, triangle);
			return;
		}
		const uint16 n = CrescentIntervals(shape, fullQuality);
		const uint16 count = (n * 2);
		const Vec2 perpendicular{ -shape.axis.y, shape.axis.x };
		const Vec2 origin = (shape.circle.center + shape.axis * shape.halfWidth);
		const double h2 = (shape.halfChord * shape.halfChord);
		vertex(0, Float2{ origin + perpendicular * shape.halfChord });
		vertex(n, Float2{ origin - perpendicular * shape.halfChord });
		for (uint16 i = 1; i < n; ++i)
		{
			const double angle = (Math::Pi * i / n);
			const double s = std::sin(angle), c = std::cos(angle);
			const double along = (shape.halfWidth * s);
			const double outer = (std::sqrt(h2 + along * along) + along);
			// Rationalization preserves the inner arc as the crescent approaches a circle.
			const double inner = (h2 / outer);
			const Vec2 ray = (shape.axis * -s + perpendicular * c);
			vertex(i, Float2{ origin + ray * outer });
			vertex((count - i), Float2{ origin + ray * inner });
		}
		triangle(0, 1, (count - 1));
		for (uint16 i = 1; i + 1 < n; ++i)
		{
			triangle(i, (i + 1), (count - i));
			triangle((count - i), (i + 1), (count - i - 1));
		}
		triangle((n - 1), n, (n + 1));
	}

	template <class Vertex, class Triangle>
	void Write(const Teardrop& shape, const uint16 fullQuality, Vertex&& vertex, Triangle&& triangle)
	{
		if (shape.circleOnly)
		{
			WriteCircle(shape.circle, fullQuality, vertex, triangle);
			return;
		}
		const uint16 n = TeardropIntervals(shape, fullQuality);
		const Vec2 perpendicular{ -shape.axis.y, shape.axis.x };
		const Vec2 along = (shape.axis * (shape.circle.r * shape.tangentCos));
		const Vec2 across = (perpendicular * (shape.circle.r * shape.tangentSin));
		vertex(0, Float2{ shape.tip });
		vertex(1, Float2{ shape.circle.center + along + across });
		vertex((n + 1), Float2{ shape.circle.center + along - across });
		const double sweep = (Math::TwoPi - 2 * shape.tangentAngle);
		for (uint16 i = 1; i < n; ++i)
		{
			const double angle = (shape.tangentAngle + sweep * i / n);
			vertex((i + 1), Float2{ shape.circle.center
				+ (shape.axis * std::cos(angle) + perpendicular * std::sin(angle)) * shape.circle.r });
		}
		for (uint16 i = 1; i <= n; ++i) triangle(0, i, (i + 1));
	}
}
