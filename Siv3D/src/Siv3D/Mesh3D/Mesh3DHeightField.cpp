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
# include <Siv3D/Mesh3DBuilder.hpp>
# include "Mesh3DCommon.hpp"
# include <cmath>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::AddedRange;
		using Mesh3DDetail::AdditionFailed;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::IsFloatRepresentable;

		struct HeightFieldLayout
		{
			size_t columnCount = 0;
			size_t rowCount = 0;
			size_t segmentsX = 0;
			size_t segmentsZ = 0;
			size_t vertexCount = 0;
			size_t triangleCount = 0;
			Float2 uvScale;
			Float2 uvOffset;
			Array<float> xPositions;
			Array<float> zPositions;
		};

		[[nodiscard]]
		static Optional<Mesh3DError> PrepareHeightFieldLayout(
			const size_t columnCount,
			const size_t rowCount,
			const SizeF sizeXZ,
			const HeightFieldOptions& options,
			HeightFieldLayout& layout)
		{
			if ((columnCount < 2)
				|| (rowCount < 2))
			{
				return Mesh3DError{
					Mesh3DErrorCode::InvalidArgument,
					U"Mesh3D::HeightField(): The grid dimensions are invalid"
				};
			}

			if ((not IsFloatRepresentable(sizeXZ.x))
				|| (not IsFloatRepresentable(sizeXZ.y))
				|| (not IsFloatRepresentable(options.uvScale.x))
				|| (not IsFloatRepresentable(options.uvScale.y))
				|| (not IsFloatRepresentable(options.uvOffset.x))
				|| (not IsFloatRepresentable(options.uvOffset.y))
				|| (not IsFloatRepresentable(options.uvOffset.x + options.uvScale.x))
				|| (not IsFloatRepresentable(options.uvOffset.y + options.uvScale.y)))
			{
				return Mesh3DError{
					Mesh3DErrorCode::NumericRange,
					U"Mesh3D::HeightField(): The size or UV transform is non-finite or outside the float range"
				};
			}

			const Float2 sizeXZFloat = sizeXZ;
			if ((sizeXZFloat.x <= 0.0f)
				|| (sizeXZFloat.y <= 0.0f))
			{
				return Mesh3DError{
					Mesh3DErrorCode::InvalidArgument,
					U"Mesh3D::HeightField(): sizeXZ must remain positive after conversion to float"
				};
			}

			layout.columnCount = columnCount;
			layout.rowCount = rowCount;
			layout.segmentsX = (layout.columnCount - 1);
			layout.segmentsZ = (layout.rowCount - 1);
			size_t cellCount;
			if ((not CheckedMultiply(layout.columnCount, layout.rowCount, layout.vertexCount))
				|| (Mesh3D::MaxVertexCount < layout.vertexCount)
				|| (not CheckedMultiply(layout.segmentsX, layout.segmentsZ, cellCount))
				|| (not CheckedMultiply(cellCount, 2, layout.triangleCount)))
			{
				return Mesh3DError{
					Mesh3DErrorCode::SizeLimit,
					U"Mesh3D::HeightField(): The generated mesh exceeds the supported size"
				};
			}

			layout.xPositions.resize(layout.columnCount);
			layout.zPositions.resize(layout.rowCount);
			const double halfSizeX = (sizeXZ.x * 0.5);
			const double halfSizeZ = (sizeXZ.y * 0.5);
			for (size_t x = 0; x < layout.columnCount; ++x)
			{
				const double u = (static_cast<double>(x) / layout.segmentsX);
				layout.xPositions[x] = static_cast<float>(-halfSizeX + (sizeXZ.x * u));
				if ((0 < x) && (not (layout.xPositions[x - 1] < layout.xPositions[x])))
				{
					return Mesh3DError{
						Mesh3DErrorCode::NumericRange,
						U"Mesh3D::HeightField(): Adjacent X coordinates collapse after conversion to float"
					};
				}
			}

			for (size_t z = 0; z < layout.rowCount; ++z)
			{
				const double v = (static_cast<double>(z) / layout.segmentsZ);
				layout.zPositions[z] = static_cast<float>(halfSizeZ - (sizeXZ.y * v));
				if ((0 < z) && (not (layout.zPositions[z] < layout.zPositions[z - 1])))
				{
					return Mesh3DError{
						Mesh3DErrorCode::NumericRange,
						U"Mesh3D::HeightField(): Adjacent Z coordinates collapse after conversion to float"
					};
				}
			}

			layout.uvScale = options.uvScale;
			layout.uvOffset = options.uvOffset;
			return none;
		}

		[[nodiscard]]
		static Mesh3DAddResult AppendPreparedHeightField(
			Mesh3D& mesh,
			const Grid<float>& heights,
			const HeightFieldLayout& layout)
		{
			const size_t vertexBase = mesh.vertices.size();
			const size_t triangleBase = mesh.indices.size();
			size_t newVertexCount;
			size_t newTriangleCount;
			if ((not Mesh3DDetail::CheckedAdd(vertexBase, layout.vertexCount, newVertexCount))
				|| (Mesh3D::MaxVertexCount < newVertexCount)
				|| (not Mesh3DDetail::CheckedAdd(triangleBase, layout.triangleCount, newTriangleCount)))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit,
					U"Mesh3D::HeightField(): The generated mesh exceeds the supported size");
			}

			mesh.vertices.resize(newVertexCount);
			mesh.indices.resize(newTriangleCount);
			for (size_t z = 0; z < layout.rowCount; ++z)
			{
				const size_t upperZ = ((z == 0) ? 0 : (z - 1));
				const size_t lowerZ = (((z + 1) == layout.rowCount) ? z : (z + 1));
				const float v = static_cast<float>(static_cast<double>(z) / layout.segmentsZ);

				for (size_t x = 0; x < layout.columnCount; ++x)
				{
					const size_t leftX = ((x == 0) ? 0 : (x - 1));
					const size_t rightX = (((x + 1) == layout.columnCount) ? x : (x + 1));
					const Vec3 xDerivative{
						(static_cast<double>(layout.xPositions[rightX]) - layout.xPositions[leftX]),
						(static_cast<double>(heights[z][rightX]) - heights[z][leftX]),
						0.0
					};
					const Vec3 vDerivative{
						0.0,
						(static_cast<double>(heights[lowerZ][x]) - heights[upperZ][x]),
						(static_cast<double>(layout.zPositions[lowerZ]) - layout.zPositions[upperZ])
					};
					const Vec3 normal = xDerivative.cross(vDerivative).normalized();
					const Vec3 tangent = xDerivative.normalized();
					const float u = static_cast<float>(static_cast<double>(x) / layout.segmentsX);
					const size_t vertexIndex = (vertexBase + z * layout.columnCount + x);

					mesh.vertices[vertexIndex] = Vertex3D{
						.pos = Float3{ layout.xPositions[x], heights[z][x], layout.zPositions[z] },
						.normal = normal,
						.tex = Float2{
							(layout.uvOffset.x + (layout.uvScale.x * u)),
							(layout.uvOffset.y + (layout.uvScale.y * v))
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

			TriangleIndex32* pTriangle = (mesh.indices.data() + triangleBase);
			for (size_t z = 0; z < layout.segmentsZ; ++z)
			{
				const size_t rowOffset = (z * layout.columnCount);
				for (size_t x = 0; x < layout.segmentsX; ++x)
				{
					const uint32 i0 = static_cast<uint32>(vertexBase + rowOffset + x);
					const uint32 i1 = (i0 + 1);
					const uint32 i2 = static_cast<uint32>(vertexBase + rowOffset + layout.columnCount + x);
					const uint32 i3 = (i2 + 1);
					*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
					*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
				}
			}

			return AddedRange(mesh, vertexBase, triangleBase);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	HeightField
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DDetail::AppendHeightField(
		Mesh3D& mesh,
		const Grid<float>& heights,
		const SizeF sizeXZ,
		const HeightFieldOptions& options)
	{
		HeightFieldLayout layout;
		if (const auto error = PrepareHeightFieldLayout(
			heights.width(), heights.height(), sizeXZ, options, layout))
		{
			return AdditionFailed(error->code, error->message);
		}

		for (const float height : heights)
		{
			if (not std::isfinite(height))
			{
				return AdditionFailed(Mesh3DErrorCode::NumericRange,
					U"Mesh3D::HeightField(): Every height must be finite");
			}
		}

		return AppendPreparedHeightField(mesh, heights, layout);
	}

	Mesh3DAddResult Mesh3DDetail::AppendHeightField(
		Mesh3D& mesh,
		const Size gridSize,
		const SizeF sizeXZ,
		const FunctionRef<double(Point)> heightFunction,
		const HeightFieldOptions& options)
	{
		if ((gridSize.x < 2)
			|| (gridSize.y < 2))
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::HeightField(): The grid dimensions are invalid");
		}

		HeightFieldLayout layout;
		if (const auto error = PrepareHeightFieldLayout(
			static_cast<size_t>(gridSize.x), static_cast<size_t>(gridSize.y),
			sizeXZ, options, layout))
		{
			return AdditionFailed(error->code, error->message);
		}

		Grid<float> heights{ gridSize };
		for (int32 y = 0; y < gridSize.y; ++y)
		{
			for (int32 x = 0; x < gridSize.x; ++x)
			{
				const Point point{ x, y };
				const double height = heightFunction(point);
				if (not IsFloatRepresentable(height))
				{
					return AdditionFailed(Mesh3DErrorCode::NumericRange,
						U"Mesh3D::HeightField(): Every generated height must be finite and representable as float");
				}

				heights[y][x] = static_cast<float>(height);
			}
		}

		return AppendPreparedHeightField(mesh, heights, layout);
	}

	Mesh3D Mesh3D::HeightField(
		const s3d::Grid<float>& heights,
		const SizeF sizeXZ,
		const HeightFieldOptions& options)
	{
		Mesh3DBuilder builder;
		(void)builder.addHeightField(heights, sizeXZ, options);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::HeightField(
		const Size gridSize,
		const SizeF sizeXZ,
		const FunctionRef<double(Point)> heightFunction,
		const HeightFieldOptions& options)
	{
		Mesh3DBuilder builder;
		(void)builder.addHeightField(gridSize, sizeXZ, heightFunction, options);
		return std::move(builder).build();
	}
}
