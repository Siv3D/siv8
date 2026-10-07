//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Triangulate.hpp"
# include "GeometryCommon.hpp"
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
		template <class InnerRing>
		struct PolygonRingsView
		{
			std::span<const Vec2> outer;
			std::span<const InnerRing> holes;

			[[nodiscard]]
			size_t size() const noexcept
			{
				return (1 + holes.size());
			}

			[[nodiscard]]
			bool empty() const noexcept
			{
				return false; // The first ring always represents the outer boundary.
			}

			[[nodiscard]]
			std::span<const Vec2> operator [](const size_t index) const noexcept
			{
				return ((index == 0) ? outer : std::span<const Vec2>{ holes[index - 1] });
			}
		};

		template <class InnerRing>
		[[nodiscard]]
		std::vector<Vertex2D::IndexType> MakeTriangleIndices(const std::span<const Vec2> outer, const Array<InnerRing>& holes)
		{
			const PolygonRingsView<InnerRing> polygon{ outer, holes };
			auto indices = mapbox::earcut<Vertex2D::IndexType>(polygon);

			if (indices.size() % 3 != 0)
			{
				indices.clear();
			}

			return indices;
		}

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
		const auto indices = MakeTriangleIndices(outer, holes);

		if (indices.empty())
		{
			return false;
		}

		const size_t triangleCount = (indices.size() / 3);

		dstIndices.resize(triangleCount);

		std::memcpy(dstIndices.data(), indices.data(), dstIndices.size_bytes());

		return true;
	}

	namespace
	{
		template <class Fill>
		void DrawTrianglesImpl(const std::span<const Vec2> outer, const InnersType& holes, const Fill& fill)
		{
			const auto indices = MakeTriangleIndices(outer, holes);

			if (indices.empty())
			{
				return;
			}

			SIV3D_ENGINE(Renderer2D)->addPolygon(MakeVertices(outer, holes), indices, fill);
		}
	}

	void DrawTriangles(const std::span<const Vec2> outer, const InnersType& holes, const ColorF& color)
	{
		DrawTrianglesImpl(outer, holes, color.toFloat4());
	}

	void DrawTriangles(const std::span<const Vec2> outer, const InnersType& holes, const PatternParameters& pattern)
	{
		DrawTrianglesImpl(outer, holes, pattern);
	}
}
