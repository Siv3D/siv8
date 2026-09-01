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

# include <Siv3D/Mesh3D.hpp>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsFloatRepresentable(const double value) noexcept
		{
			constexpr double MaxFloat = std::numeric_limits<float>::max();
			return (std::isfinite(value)
				&& (-MaxFloat <= value)
				&& (value <= MaxFloat));
		}

		[[nodiscard]]
		static bool CheckedMultiply(const size_t a, const size_t b, size_t& result) noexcept
		{
			if ((a != 0)
				&& ((std::numeric_limits<size_t>::max() / a) < b))
			{
				return false;
			}

			result = (a * b);
			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	HeightField
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::HeightField(
		const s3d::Grid<float>& heights,
		const SizeF _sizeXZ,
		const Vec2 _uvScale,
		const Vec2 _uvOffset)
	{
		if ((heights.width() < 2)
			|| (heights.height() < 2)
			|| (not IsFloatRepresentable(_sizeXZ.x))
			|| (not IsFloatRepresentable(_sizeXZ.y))
			|| (not IsFloatRepresentable(_uvScale.x))
			|| (not IsFloatRepresentable(_uvScale.y))
			|| (not IsFloatRepresentable(_uvOffset.x))
			|| (not IsFloatRepresentable(_uvOffset.y))
			|| (not IsFloatRepresentable(_uvOffset.x + _uvScale.x))
			|| (not IsFloatRepresentable(_uvOffset.y + _uvScale.y)))
		{
			return{};
		}

		const Float2 sizeXZ = _sizeXZ;
		if ((sizeXZ.x <= 0.0f)
			|| (sizeXZ.y <= 0.0f))
		{
			return{};
		}

		for (const float height : heights)
		{
			if (not std::isfinite(height))
			{
				return{};
			}
		}

		const size_t columnCount = static_cast<size_t>(heights.width());
		const size_t rowCount = static_cast<size_t>(heights.height());
		const size_t segmentsX = (columnCount - 1);
		const size_t segmentsZ = (rowCount - 1);
		size_t vertexCount;
		size_t cellCount;
		size_t triangleCount;
		if ((not CheckedMultiply(columnCount, rowCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(segmentsX, segmentsZ, cellCount))
			|| (not CheckedMultiply(cellCount, 2, triangleCount)))
		{
			return{};
		}

		Array<float> xPositions(columnCount);
		Array<float> zPositions(rowCount);
		const double halfSizeX = (_sizeXZ.x * 0.5);
		const double halfSizeZ = (_sizeXZ.y * 0.5);
		for (size_t x = 0; x < columnCount; ++x)
		{
			const double u = (static_cast<double>(x) / segmentsX);
			xPositions[x] = static_cast<float>(-halfSizeX + (_sizeXZ.x * u));
			if ((0 < x) && (not (xPositions[x - 1] < xPositions[x])))
			{
				return{};
			}
		}

		for (size_t z = 0; z < rowCount; ++z)
		{
			const double v = (static_cast<double>(z) / segmentsZ);
			zPositions[z] = static_cast<float>(halfSizeZ - (_sizeXZ.y * v));
			if ((0 < z) && (not (zPositions[z] < zPositions[z - 1])))
			{
				return{};
			}
		}

		const Float2 uvScale = _uvScale;
		const Float2 uvOffset = _uvOffset;
		Mesh3D mesh{ vertexCount, triangleCount };
		for (size_t z = 0; z < rowCount; ++z)
		{
			const size_t upperZ = ((z == 0) ? 0 : (z - 1));
			const size_t lowerZ = (((z + 1) == rowCount) ? z : (z + 1));
			const float v = static_cast<float>(static_cast<double>(z) / segmentsZ);

			for (size_t x = 0; x < columnCount; ++x)
			{
				const size_t leftX = ((x == 0) ? 0 : (x - 1));
				const size_t rightX = (((x + 1) == columnCount) ? x : (x + 1));
				const Vec3 xDerivative{
					(static_cast<double>(xPositions[rightX]) - xPositions[leftX]),
					(static_cast<double>(heights[z][rightX]) - heights[z][leftX]),
					0.0
				};
				const Vec3 vDerivative{
					0.0,
					(static_cast<double>(heights[lowerZ][x]) - heights[upperZ][x]),
					(static_cast<double>(zPositions[lowerZ]) - zPositions[upperZ])
				};
				const Vec3 normal = xDerivative.cross(vDerivative).normalized();
				const Vec3 tangent = xDerivative.normalized();
				const float u = static_cast<float>(static_cast<double>(x) / segmentsX);
				const size_t vertexIndex = (z * columnCount + x);

				mesh.vertices[vertexIndex] = Vertex3D{
					.pos = Float3{ xPositions[x], heights[z][x], zPositions[z] },
					.normal = normal,
					.tex = Float2{
						(uvOffset.x + (uvScale.x * u)),
						(uvOffset.y + (uvScale.y * v))
					},
					.tangent = Float4{
						static_cast<float>(tangent.x),
						static_cast<float>(tangent.y),
						static_cast<float>(tangent.z),
						1.0f
					}
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (size_t z = 0; z < segmentsZ; ++z)
		{
			const size_t rowOffset = (z * columnCount);
			for (size_t x = 0; x < segmentsX; ++x)
			{
				const uint32 i0 = static_cast<uint32>(rowOffset + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(rowOffset + columnCount + x);
				const uint32 i3 = (i2 + 1);
				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		return mesh;
	}
}
