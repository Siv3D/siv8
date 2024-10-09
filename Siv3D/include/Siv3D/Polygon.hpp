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
# include <memory>
# include "Array.hpp"
# include "Common.hpp"
# include "PointVector.hpp"
# include "2DShapes.hpp"
# include "TriangleIndex.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	class Polygon
	{
	private:

		class PolygonDetail;

	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Polygon();

		[[nodiscard]]
		Polygon(const Polygon& other);

		[[nodiscard]]
		Polygon(Polygon&& other) noexcept;

		[[nodiscard]]
		explicit Polygon(std::span<const Vec2> outer, Array<Array<Vec2>> holes = {}, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		explicit Polygon(std::initializer_list<Vec2> outer, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		Polygon(const Shape2D& shape);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~Polygon();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		Polygon& operator =(const Polygon& other);

		Polygon& operator =(Polygon&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		void swap(Polygon& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が空であるかを返します。
		/// @return  多角形が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が空ではないかを返します。
		/// @return 多角形が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasHoles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が穴を持っているかを返します。
		/// @return 穴を持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasHoles() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_holes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が持つ穴の個数を返します。
		/// @return 多角形が持つ穴の個数
		[[nodiscard]]
		size_t num_holes() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outer
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の外周の頂点配列を返します。
		/// @return 多角形の外周の頂点配列
		[[nodiscard]]
		const Array<Vec2>& outer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inners
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の穴を構成する頂点配列を返します。
		/// @return 多角形の穴を構成する頂点配列
		[[nodiscard]]
		const Array<Array<Vec2>>& inners() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vertices
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形のすべての頂点を返します。
		/// @return 多角形のすべての頂点
		[[nodiscard]]
		const Array<Float2>& vertices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	indices
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の三角形分割のすべての頂点インデックスを返します。
		/// @remark `vertices()` の戻り値に対応します。
		/// @return 多角形の三角形分割のすべての頂点インデックス
		[[nodiscard]]
		const Array<TriangleIndex>& indices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形のバウンディングボックスを返します。
		/// @remark バウンディングボックスは計算済みであるため、この関数のコストは小さいです。
		/// @return 多角形のバウンディングボックス
		[[nodiscard]]
		const RectF& boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_triangles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の三角形分割での三角形の個数を返します。
		/// @remark 三角形分割は計算済みであるため、この関数のコストは小さいです。
		/// @return 三角形の個数
		[[nodiscard]]
		size_t num_triangles() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	triangle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の三角形分割での、個々の三角形を返します。
		/// @param index 三角形のインデックス。0 以上 `(num_triangles() - 1)` 以下
		/// @return 三角形
		[[nodiscard]]
		Triangle triangleAtIndex(size_t index) const;




		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(Polygon& lhs, Polygon& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Polygon& value);

	private:

		std::unique_ptr<PolygonDetail> pImpl;
	};
}

# include "detail/Polygon.ipp"
