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

# include "Triangulate.hpp"
# include <ThirdParty/Earcut/earcut.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

// Earcut s3d::Vec2 adapter
namespace mapbox::util
{
	template <>
	struct nth<0, s3d::Vec2>
	{
		static constexpr auto get(const s3d::Vec2& t)
		{
			return t.x;
		};
	};

	template <>
	struct nth<1, s3d::Vec2>
	{
		static constexpr auto get(const s3d::Vec2& t)
		{
			return t.y;
		};
	};
}

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static size_t GetVertexCount(const InnersType& holes) noexcept
		{
			size_t count = 0;

			for (const auto& hole : holes)
			{
				count += hole.size();
			}

			return count;
		}

		[[nodiscard]]
		static Array<Float2> MakeVertices(std::span<const Vec2> outerVertices, const InnersType& holes)
		{
			Array<Float2> vertices{ Arg::reserve = (outerVertices.size() + GetVertexCount(holes)) };

			vertices.append_range(outerVertices);

			for (const auto& hole : holes)
			{
				vertices.append_range(hole);
			}

			return vertices;
		}
	}

	bool Triangulate(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes, Array<TriangleIndex>& dstIndices)
	{
		Array<Array<Vec2>> polygon{ Arg::reserve = (1 + holes.size()) };
		{
			polygon.emplace_back(outer.begin(), outer.end());

			for (const auto& hole : holes)
			{
				polygon.emplace_back(hole.begin(), hole.end());
			}
		}

		const std::vector<Vertex2D::IndexType> indices = mapbox::earcut<Vertex2D::IndexType>(polygon);

		if (indices.size() % 3 != 0)
		{
			return false;
		}

		const size_t triangleCount = (indices.size() / 3);

		dstIndices.resize(triangleCount);

		std::memcpy(dstIndices.data(), indices.data(), dstIndices.size_bytes());

		return true;
	}

	void DrawTriangles(const std::span<const Vec2> outer, const InnersType& holes, const ColorF& color)
	{
		Array<Array<Vec2>> polygon{ Arg::reserve = (1 + holes.size()) };
		{
			polygon.emplace_back(outer.begin(), outer.end());

			for (const auto& hole : holes)
			{
				polygon.emplace_back(hole.begin(), hole.end());
			}
		}

		const std::vector<Vertex2D::IndexType> indices = mapbox::earcut<Vertex2D::IndexType>(polygon);

		if (indices.size() % 3 != 0)
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(MakeVertices(outer, holes), indices, color.toFloat4());
	}

	void DrawTriangles(std::span<const Vec2> outer, const InnersType& holes, const PatternParameters& pattern)
	{
		Array<Array<Vec2>> polygon{ Arg::reserve = (1 + holes.size()) };
		{
			polygon.emplace_back(outer.begin(), outer.end());

			for (const auto& hole : holes)
			{
				polygon.emplace_back(hole.begin(), hole.end());
			}
		}

		const std::vector<Vertex2D::IndexType> indices = mapbox::earcut<Vertex2D::IndexType>(polygon);

		if (indices.size() % 3 != 0)
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(MakeVertices(outer, holes), indices, pattern);
	}
}
