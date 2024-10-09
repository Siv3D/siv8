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

# pragma once
# include <Siv3D/Polygon.hpp>
# include "GeometryCommon.hpp"

namespace s3d
{
	class Polygon::PolygonDetail
	{
	public:

		[[nodiscard]]
		PolygonDetail() = default;

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, SkipValidation skipValidation);

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation);

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation);

		[[nodiscard]]
		bool isEmpty() const noexcept;

		[[nodiscard]]
		const Array<Vec2>& outer() const noexcept;

		[[nodiscard]]
		const Array<Array<Vec2>>& inners() const noexcept;

		[[nodiscard]]
		const Array<Float2>& vertices() const noexcept;

		[[nodiscard]]
		const Array<TriangleIndex>& indices() const noexcept;

		[[nodiscard]]
		const RectF& boundingRect() const noexcept;



		void draw(const ColorF& color) const;

		void draw(const Vec2& offset, const ColorF& color) const;

		void draw(const PatternParameters& pattern) const;

		void draw(const Vec2& offset, const PatternParameters& pattern) const;

	private:

		CwOpenPolygon m_polygon;

		Array<Array<Vec2>> m_holes;

		Array<Float2> m_vertices;

		Array<TriangleIndex> m_indices;

		RectF m_boundingRect = RectF::Empty();
	};
}
