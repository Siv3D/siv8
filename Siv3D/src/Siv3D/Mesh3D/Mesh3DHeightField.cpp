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
# include "Mesh3DCommon.hpp"
# include <cmath>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::AddedRange;
		using Mesh3DDetail::OperationFailed;
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
			Mesh3DDetail::UVTangentTransform tangentTransform{ Vec2::One() };
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

			if ((not IsFloatRepresentable(sizeXZ))
				|| (not IsFloatRepresentable(options.uvScale))
				|| (not IsFloatRepresentable(options.uvOffset))
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
			layout.tangentTransform = Mesh3DDetail::UVTangentTransform{ options.uvScale };
			return none;
		}

		[[nodiscard]]
		static Mesh3DAddResult AppendPreparedHeightField(
			Mesh3D& mesh,
			const Grid<float>& heights,
			const HeightFieldLayout& layout)
		{
			size_t vertexBase;
			size_t triangleBase;
			if (not Mesh3DDetail::ResizeForAddition(
				mesh, layout.vertexCount, layout.triangleCount, vertexBase, triangleBase))
			{
				return OperationFailed(Mesh3DErrorCode::SizeLimit,
					U"Mesh3D::HeightField(): The generated mesh exceeds the supported size");
			}
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
						.tangent = layout.tangentTransform.apply(Float4{
							static_cast<float>(tangent.x),
							static_cast<float>(tangent.y),
							static_cast<float>(tangent.z),
							1.0f
						})
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
			return OperationFailed(error->code, error->message);
		}

		for (const float height : heights)
		{
			if (not std::isfinite(height))
			{
				return OperationFailed(Mesh3DErrorCode::NumericRange,
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
			return OperationFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::HeightField(): The grid dimensions are invalid");
		}

		HeightFieldLayout layout;
		if (const auto error = PrepareHeightFieldLayout(
			static_cast<size_t>(gridSize.x), static_cast<size_t>(gridSize.y),
			sizeXZ, options, layout))
		{
			return OperationFailed(error->code, error->message);
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
					return OperationFailed(Mesh3DErrorCode::NumericRange,
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendHeightField(mesh, heights, sizeXZ, options);
		return mesh;
	}

	Mesh3D Mesh3D::HeightField(
		const Size gridSize,
		const SizeF sizeXZ,
		const FunctionRef<double(Point)> heightFunction,
		const HeightFieldOptions& options)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendHeightField(mesh, gridSize, sizeXZ, heightFunction, options);
		return mesh;
	}

	Mesh3DAddResult Mesh3DDetail::AppendPlane(
		Mesh3D& mesh,
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return AppendGrid(mesh, sizeXZ, 1, 1, uvScale, uvOffset);
	}

	Mesh3DAddResult Mesh3DDetail::AppendGrid(
		Mesh3D& mesh,
		const SizeF _sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 _uvScale,
		const Vec2 _uvOffset)
	{
		if ((not IsFloatRepresentable(_sizeXZ))
			|| (not IsFloatRepresentable(_uvScale))
			|| (not IsFloatRepresentable(_uvOffset)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Grid()/Plane(): sizeXZ and the UV transform must be finite and float-representable");
		}

		const Float2 sizeXZ = _sizeXZ;
		const Float2 uvScale = _uvScale;
		const Float2 uvOffset = _uvOffset;
		if ((sizeXZ.x <= 0.0f)
			|| (sizeXZ.y <= 0.0f)
			|| (segmentsX == 0)
			|| (segmentsZ == 0))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Grid()/Plane(): Every size component and segment count must be positive");
		}

		size_t columnCount;
		size_t rowCount;
		size_t vertexCount;
		size_t cellCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(segmentsX), 1, columnCount))
			|| (not CheckedAdd(static_cast<size_t>(segmentsZ), 1, rowCount))
			|| (not CheckedMultiply(columnCount, rowCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(segmentsX), static_cast<size_t>(segmentsZ), cellCount))
			|| (not CheckedMultiply(cellCount, 2, triangleCount)))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Grid()/Plane(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Grid()/Plane(): The generated mesh exceeds the supported size");
		}

		const Float2 halfSize = (sizeXZ * 0.5f);
		const float invSegmentsX = (1.0f / static_cast<float>(segmentsX));
		const float invSegmentsZ = (1.0f / static_cast<float>(segmentsZ));
		const Float3 normal = Float3::UnitY();
		const Float4 tangent = UVTangentTransform{ _uvScale }.apply(Float4{ 1.0f, 0.0f, 0.0f, 1.0f });

		for (uint32 z = 0; z <= segmentsZ; ++z)
		{
			const float v = (z * invSegmentsZ);
			const float positionZ = (halfSize.y - (sizeXZ.y * v));

			for (uint32 x = 0; x <= segmentsX; ++x)
			{
				const float u = (x * invSegmentsX);
				const size_t vertexIndex = (vertexOffset
					+ (static_cast<size_t>(z) * columnCount) + x);

				mesh.vertices[vertexIndex] = Vertex3D{
					.pos = Float3{ (-halfSize.x + (sizeXZ.x * u)), 0.0f, positionZ },
					.normal = normal,
					.tex = Float2{ (uvOffset.x + (uvScale.x * u)), (uvOffset.y + (uvScale.y * v)) },
					.tangent = tangent
				};
			}
		}

		TriangleIndex32* pTriangle = (mesh.indices.data() + triangleOffset);
		for (uint32 z = 0; z < segmentsZ; ++z)
		{
			const size_t rowOffset = (vertexOffset + (static_cast<size_t>(z) * columnCount));

			for (uint32 x = 0; x < segmentsX; ++x)
			{
				const uint32 i0 = static_cast<uint32>(rowOffset + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(rowOffset + columnCount + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - vertexCount),
			(mesh.indices.size() - triangleCount));
	}

}
