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

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		PolygonDetail() = default;

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, SkipValidation skipValidation);

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation);

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outer
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Array<Vec2>& outer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inners
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Array<Array<Vec2>>& inners() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vertices
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Array<Float2>& vertices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	indices
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Array<TriangleIndex>& indices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const RectF& boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		void draw(const ColorF& color) const;

		void draw(const Vec2& offset, const ColorF& color) const;

		void draw(const PatternParameters& pattern) const;

		void draw(const Vec2& offset, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	Validate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static PolygonFailureType Validate(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes);

		////////////////////////////////////////////////////////////////
		//
		//	Correct
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Array<Polygon> Correct(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes);

	private:

		CwOpenPolygon m_polygon;

		Array<Array<Vec2>> m_holes;

		Array<Float2> m_vertices;

		Array<TriangleIndex> m_indices;

		RectF m_boundingRect = RectF::Empty();
	};
}
