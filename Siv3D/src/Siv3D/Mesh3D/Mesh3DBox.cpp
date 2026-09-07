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

# include "Mesh3DCommon.hpp"
# include <algorithm>
# include <array>
# include <cmath>
# include <limits>

namespace s3d::Mesh3DDetail
{
	namespace
	{
		struct BoxFaceData
		{
			Float3 center;
			Float3 u;
			Float3 v;
			Float3 normal;
		};

		[[nodiscard]]
		static bool IsFinite(const BoxUVMapping& uvMapping) noexcept
		{
			return (uvMapping.negativeZ.isFinite()
				&& uvMapping.positiveZ.isFinite()
				&& uvMapping.positiveX.isFinite()
				&& uvMapping.negativeX.isFinite()
				&& uvMapping.positiveY.isFinite()
				&& uvMapping.negativeY.isFinite());
		}

		[[nodiscard]]
		static Float2 MapProjectedUV(const FloatRect& uvRect, const Float2 uv) noexcept
		{
			return{
				(uvRect.left + ((uvRect.right - uvRect.left) * uv.x)),
				(uvRect.top + ((uvRect.bottom - uvRect.top) * uv.y))
			};
		}

		[[nodiscard]]
		static Float4 MakeProjectedTangent(const Float3 baseTangent, const FloatRect& uvRect) noexcept
		{
			const float uSign = ((uvRect.right < uvRect.left) ? -1.0f : 1.0f);
			const float vSign = ((uvRect.bottom < uvRect.top) ? -1.0f : 1.0f);
			return{ (baseTangent * uSign), (uSign * vSign) };
		}


		static void WriteProjectedTriangle(
			Mesh3D& mesh,
			size_t& vertexOffset,
			size_t& triangleOffset,
			const std::array<Float3, 3>& positions,
			const std::array<Float2, 3>& projectedUVs,
			const Float3 normal,
			const Float3 baseTangent,
			const FloatRect& uvRect)
		{
			const Float4 tangent = MakeProjectedTangent(baseTangent, uvRect);

			for (size_t i = 0; i < positions.size(); ++i)
			{
				mesh.vertices[vertexOffset + i] = Vertex3D{
					.pos = positions[i],
					.normal = normal,
					.tex = MapProjectedUV(uvRect, projectedUVs[i]),
					.tangent = tangent
				};
			}

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			mesh.indices[triangleOffset] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			vertexOffset += 3;
			++triangleOffset;
		}

		static void WriteProjectedQuad(
			Mesh3D& mesh,
			size_t& vertexOffset,
			size_t& triangleOffset,
			const std::array<Float3, 4>& positions,
			const std::array<Float2, 4>& projectedUVs,
			const Float3 normal,
			const Float3 baseTangent,
			const FloatRect& uvRect)
		{
			const Float4 tangent = MakeProjectedTangent(baseTangent, uvRect);

			for (size_t i = 0; i < positions.size(); ++i)
			{
				mesh.vertices[vertexOffset + i] = Vertex3D{
					.pos = positions[i],
					.normal = normal,
					.tex = MapProjectedUV(uvRect, projectedUVs[i]),
					.tangent = tangent
				};
			}

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
			vertexOffset += 4;
			triangleOffset += 2;
		}

		[[nodiscard]]
		static const FloatRect& GetBoxUVRect(
			const BoxUVMapping& uvMapping,
			const BoxFace face) noexcept
		{
			switch (face)
			{
			case BoxFace::NegativeX: return uvMapping.negativeX;
			case BoxFace::PositiveX: return uvMapping.positiveX;
			case BoxFace::NegativeY: return uvMapping.negativeY;
			case BoxFace::PositiveY: return uvMapping.positiveY;
			case BoxFace::NegativeZ: return uvMapping.negativeZ;
			default: return uvMapping.positiveZ;
			}
		}

		[[nodiscard]]
		static Float2 ProjectBoxUV(
			const Float3 position,
			const Float3 outerSize,
			const BoxFace projection) noexcept
		{
			switch (projection)
			{
			case BoxFace::NegativeX:
				return{ (0.5f - (position.z / outerSize.z)), (0.5f - (position.y / outerSize.y)) };
			case BoxFace::PositiveX:
				return{ (0.5f + (position.z / outerSize.z)), (0.5f - (position.y / outerSize.y)) };
			case BoxFace::NegativeY:
				return{ (0.5f + (position.x / outerSize.x)), (0.5f + (position.z / outerSize.z)) };
			case BoxFace::PositiveY:
				return{ (0.5f + (position.x / outerSize.x)), (0.5f - (position.z / outerSize.z)) };
			case BoxFace::PositiveZ:
				return{ (0.5f - (position.x / outerSize.x)), (0.5f - (position.y / outerSize.y)) };
			default:
				return{ (0.5f + (position.x / outerSize.x)), (0.5f - (position.y / outerSize.y)) };
			}
		}

		[[nodiscard]]
		static Float3 GetBoxFaceNormal(const BoxFace face) noexcept
		{
			switch (face)
			{
			case BoxFace::NegativeX: return -Float3::UnitX();
			case BoxFace::PositiveX: return Float3::UnitX();
			case BoxFace::NegativeY: return -Float3::UnitY();
			case BoxFace::PositiveY: return Float3::UnitY();
			case BoxFace::NegativeZ: return -Float3::UnitZ();
			default: return Float3::UnitZ();
			}
		}

		[[nodiscard]]
		static Float3 GetBoxFaceBaseTangent(const BoxFace face) noexcept
		{
			switch (face)
			{
			case BoxFace::NegativeX: return -Float3::UnitZ();
			case BoxFace::PositiveX: return Float3::UnitZ();
			case BoxFace::PositiveZ: return -Float3::UnitX();
			default: return Float3::UnitX();
			}
		}

		[[nodiscard]]
		static constexpr BoxFace GetOppositeBoxFace(const BoxFace face) noexcept
		{
			switch (face)
			{
			case BoxFace::NegativeX: return BoxFace::PositiveX;
			case BoxFace::PositiveX: return BoxFace::NegativeX;
			case BoxFace::NegativeY: return BoxFace::PositiveY;
			case BoxFace::PositiveY: return BoxFace::NegativeY;
			case BoxFace::NegativeZ: return BoxFace::PositiveZ;
			default: return BoxFace::NegativeZ;
			}
		}

		[[nodiscard]]
		static constexpr BoxFace GetBoxFaceForAxis(
			const size_t axis,
			const bool positive) noexcept
		{
			if (axis == 0) return (positive ? BoxFace::PositiveX : BoxFace::NegativeX);
			if (axis == 1) return (positive ? BoxFace::PositiveY : BoxFace::NegativeY);
			return (positive ? BoxFace::PositiveZ : BoxFace::NegativeZ);
		}

		static void WriteAxisAlignedBoxQuad(
			Mesh3D& mesh,
			size_t& vertexOffset,
			size_t& triangleOffset,
			const Float3 outerSize,
			const BoxUVMapping& uvMapping,
			const size_t axis,
			const float coordinate,
			const float p0,
			const float p1,
			const float q0,
			const float q1,
			const BoxFace normalFace)
		{
			const size_t pAxis = ((axis + 1) % 3);
			const size_t qAxis = ((axis + 2) % 3);
			const auto makePosition = [=](const float p, const float q) noexcept
			{
				Float3 result{ 0.0f, 0.0f, 0.0f };
				result.getPointer()[axis] = coordinate;
				result.getPointer()[pAxis] = p;
				result.getPointer()[qAxis] = q;
				return result;
			};

			std::array<Float3, 4> positions
			{{
				makePosition(p0, q0),
				makePosition(p1, q0),
				makePosition(p0, q1),
				makePosition(p1, q1),
			}};
			const Float3 normal = GetBoxFaceNormal(normalFace);
			if ((positions[1] - positions[0]).cross(positions[2] - positions[0]).dot(normal) < 0.0f)
			{
				std::swap(positions[0], positions[1]);
				std::swap(positions[2], positions[3]);
			}

			std::array<Float2, 4> projectedUVs;
			for (size_t i = 0; i < positions.size(); ++i)
			{
				projectedUVs[i] = ProjectBoxUV(positions[i], outerSize, normalFace);
			}

			WriteProjectedQuad(mesh, vertexOffset, triangleOffset,
				positions, projectedUVs, normal, GetBoxFaceBaseTangent(normalFace),
				GetBoxUVRect(uvMapping, normalFace));
		}
	}

	Mesh3DAddResult AppendBox(
		Mesh3D& mesh,
		const Vec3 size,
		const BoxFace faces)
	{
		return AppendBox(mesh, size, BoxUVMapping{}, faces);
	}

	Mesh3DAddResult AppendBox(
		Mesh3D& mesh,
		const Vec3 _size,
		const BoxUVMapping& uvMapping,
		const BoxFace selectedFaces)
	{
		if (not IsFloatRepresentable(_size))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange,
				U"Mesh3D::Box(): size must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::Box(): Every size component must be positive after conversion to float");
		}

		constexpr uint8 ValidFaceBits = static_cast<uint8>(BoxFace::All);
		const uint8 selectedFaceBits = static_cast<uint8>(selectedFaces);
		if (selectedFaceBits & ~ValidFaceBits)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::Box(): faces contains unsupported bits");
		}

		const Float3 halfSize = (size * 0.5f);
		const std::array<BoxFaceData, 6> faces =
		{{
			{ { 0.0f, 0.0f, -halfSize.z }, { size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ { 0.0f, 0.0f,  halfSize.z }, { -size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -size.y, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, -size.y, 0.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { 0.0f,  halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -1.0f, 0.0f } },
		}};
		const std::array<FloatRect, 6> uvRects =
		{{
			uvMapping.negativeZ,
			uvMapping.positiveZ,
			uvMapping.positiveX,
			uvMapping.negativeX,
			uvMapping.positiveY,
			uvMapping.negativeY,
		}};
		constexpr std::array<BoxFace, 6> faceMasks =
		{{
			BoxFace::NegativeZ,
			BoxFace::PositiveZ,
			BoxFace::PositiveX,
			BoxFace::NegativeX,
			BoxFace::PositiveY,
			BoxFace::NegativeY,
		}};

		size_t selectedFaceCount = 0;
		for (size_t faceIndex = 0; faceIndex < faceMasks.size(); ++faceIndex)
		{
			if (not static_cast<bool>(selectedFaces & faceMasks[faceIndex]))
			{
				continue;
			}

			if (not uvRects[faceIndex].isFinite())
			{
				return OperationFailed(Mesh3DErrorCode::NumericRange,
					U"Mesh3D::Box(): A selected UV rectangle is non-finite");
			}

			++selectedFaceCount;
		}

		const size_t addedVertexCount = (selectedFaceCount * 4);
		const size_t addedTriangleCount = (selectedFaceCount * 2);
		const size_t vertexBase = mesh.vertices.size();
		const size_t triangleBase = mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if (not CheckAdditionSize(mesh, addedVertexCount, addedTriangleCount, newVertexCount, newTriangleCount))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit,
				U"Mesh3D::Box(): The generated mesh exceeds the supported size");
		}

		mesh.vertices.resize(newVertexCount);
		mesh.indices.resize(newTriangleCount);

		size_t outputFaceIndex = 0;
		for (size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
		{
			if (not static_cast<bool>(selectedFaces & faceMasks[faceIndex]))
			{
				continue;
			}

			const BoxFaceData& face = faces[faceIndex];
			const Float3 halfU = (face.u * 0.5f);
			const Float3 halfV = (face.v * 0.5f);
			const FloatRect uvRect = uvRects[faceIndex];
			const float uSign = ((uvRect.right < uvRect.left) ? -1.0f : 1.0f);
			const float vSign = ((uvRect.bottom < uvRect.top) ? -1.0f : 1.0f);
			const Float3 tangent = (face.u.normalized() * uSign);
			const Float4 tangentFrame{ tangent, (uSign * vSign) };
			const size_t vertexOffset = (vertexBase + outputFaceIndex * 4);

			mesh.vertices[vertexOffset + 0] = Vertex3D{
				.pos = (face.center - halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.left, uvRect.top },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 1] = Vertex3D{
				.pos = (face.center + halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.right, uvRect.top },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 2] = Vertex3D{
				.pos = (face.center - halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.left, uvRect.bottom },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 3] = Vertex3D{
				.pos = (face.center + halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.right, uvRect.bottom },
				.tangent = tangentFrame
			};

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			const size_t triangleOffset = (triangleBase + outputFaceIndex * 2);
			mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
			++outputFaceIndex;
		}

		return AddedRange(mesh, vertexBase, triangleBase);
	}

	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 outerSize,
		const double thickness,
		const BoxFace openFaces)
	{
		return AppendBoxShell(mesh, outerSize, Vec3::All(thickness), BoxUVMapping{}, openFaces);
	}

	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 outerSize,
		const Vec3 thickness,
		const BoxFace openFaces)
	{
		return AppendBoxShell(mesh, outerSize, thickness, BoxUVMapping{}, openFaces);
	}

	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 outerSize,
		const double thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces)
	{
		return AppendBoxShell(mesh, outerSize, Vec3::All(thickness), uvMapping, openFaces);
	}

	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 _outerSize,
		const Vec3 _thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces)
	{
		if ((not IsFloatRepresentable(_outerSize))
			|| (not IsFloatRepresentable(_thickness)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::BoxShell(): outerSize and thickness must be finite and float-representable");
		}

		constexpr uint8 ValidFaceBits = static_cast<uint8>(BoxFace::All);
		const uint8 openFaceBits = static_cast<uint8>(openFaces);
		if (openFaceBits & ~ValidFaceBits)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::BoxShell(): openFaces contains unsupported bits");
		}

		const Float3 outerSize = _outerSize;
		const Float3 thickness = _thickness;
		const Float3 outerHalf = (outerSize * 0.5f);
		if ((outerSize.x <= 0.0f) || (outerSize.y <= 0.0f) || (outerSize.z <= 0.0f)
			|| (thickness.x <= 0.0f) || (thickness.y <= 0.0f) || (thickness.z <= 0.0f)
			|| (outerHalf.x <= thickness.x) || (outerHalf.y <= thickness.y) || (outerHalf.z <= thickness.z))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::BoxShell(): Every size and thickness component must remain positive, and thickness must be smaller than half the corresponding size after conversion to float");
		}

		struct ShellFace
		{
			BoxFace mask;
			size_t axis;
			float sign;
		};

		constexpr std::array<ShellFace, 6> Faces
		{{
			{ BoxFace::NegativeX, 0, -1.0f },
			{ BoxFace::PositiveX, 0, 1.0f },
			{ BoxFace::NegativeY, 1, -1.0f },
			{ BoxFace::PositiveY, 1, 1.0f },
			{ BoxFace::NegativeZ, 2, -1.0f },
			{ BoxFace::PositiveZ, 2, 1.0f },
		}};

		const auto isOpen = [openFaces](const BoxFace face) noexcept
		{
			return static_cast<bool>(openFaces & face);
		};

		size_t quadCount = 0;
		uint8 requiredUVFaceBits = 0;
		for (const ShellFace& face : Faces)
		{
			if (isOpen(face.mask))
			{
				bool hasRim = false;
				for (const ShellFace& adjacent : Faces)
				{
					if ((adjacent.axis != face.axis) && (not isOpen(adjacent.mask)))
					{
						++quadCount;
						hasRim = true;
					}
				}
				if (hasRim)
				{
					requiredUVFaceBits |= static_cast<uint8>(face.mask);
				}
			}
			else
			{
				quadCount += 2;
				requiredUVFaceBits |= static_cast<uint8>(face.mask);
				requiredUVFaceBits |= static_cast<uint8>(GetOppositeBoxFace(face.mask));
			}
		}

		for (const ShellFace& face : Faces)
		{
			if ((requiredUVFaceBits & static_cast<uint8>(face.mask))
				&& (not GetBoxUVRect(uvMapping, face.mask).isFinite()))
			{
				return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::BoxShell(): Every used UV rectangle must be finite");
			}
		}

		size_t addedVertexCount;
		size_t addedTriangleCount;
		if ((not CheckedMultiply(quadCount, 4, addedVertexCount))
			|| (not CheckedMultiply(quadCount, 2, addedTriangleCount)))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::BoxShell(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, addedVertexCount, addedTriangleCount, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::BoxShell(): The generated mesh exceeds the supported size");
		}

		const Float3 innerHalf = (outerHalf - thickness);

		for (const ShellFace& face : Faces)
		{
			const size_t pAxis = ((face.axis + 1) % 3);
			const size_t qAxis = ((face.axis + 2) % 3);
			const float pOuter = outerHalf.elem(pAxis);
			const float qOuter = outerHalf.elem(qAxis);
			const float pInner = innerHalf.elem(pAxis);
			const float qInner = innerHalf.elem(qAxis);

			if (not isOpen(face.mask))
			{
				WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, outerSize, uvMapping,
					face.axis, (face.sign * outerHalf.elem(face.axis)),
					-pOuter, pOuter, -qOuter, qOuter, face.mask);

				const float p0 = (isOpen(GetBoxFaceForAxis(pAxis, false)) ? -pOuter : -pInner);
				const float p1 = (isOpen(GetBoxFaceForAxis(pAxis, true)) ? pOuter : pInner);
				const float q0 = (isOpen(GetBoxFaceForAxis(qAxis, false)) ? -qOuter : -qInner);
				const float q1 = (isOpen(GetBoxFaceForAxis(qAxis, true)) ? qOuter : qInner);
				WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, outerSize, uvMapping,
					face.axis, (face.sign * innerHalf.elem(face.axis)),
					p0, p1, q0, q1, GetOppositeBoxFace(face.mask));
				continue;
			}

			const bool negativeQClosed = (not isOpen(GetBoxFaceForAxis(qAxis, false)));
			const bool positiveQClosed = (not isOpen(GetBoxFaceForAxis(qAxis, true)));
			if (negativeQClosed)
			{
				WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, outerSize, uvMapping,
					face.axis, (face.sign * outerHalf.elem(face.axis)),
					-pOuter, pOuter, -qOuter, -qInner, face.mask);
			}
			if (positiveQClosed)
			{
				WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, outerSize, uvMapping,
					face.axis, (face.sign * outerHalf.elem(face.axis)),
					-pOuter, pOuter, qInner, qOuter, face.mask);
			}

			const float q0 = (negativeQClosed ? -qInner : -qOuter);
			const float q1 = (positiveQClosed ? qInner : qOuter);
			if (not isOpen(GetBoxFaceForAxis(pAxis, false)))
			{
				WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, outerSize, uvMapping,
					face.axis, (face.sign * outerHalf.elem(face.axis)),
					-pOuter, -pInner, q0, q1, face.mask);
			}
			if (not isOpen(GetBoxFaceForAxis(pAxis, true)))
			{
				WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, outerSize, uvMapping,
					face.axis, (face.sign * outerHalf.elem(face.axis)),
					pInner, pOuter, q0, q1, face.mask);
			}
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - addedVertexCount),
			(mesh.indices.size() - addedTriangleCount));
	}

	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 size,
		const double thickness)
	{
		return AppendBoxFrame(mesh, size, Vec3::All(thickness), BoxUVMapping{});
	}

	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 size,
		const Vec3 beamSize)
	{
		return AppendBoxFrame(mesh, size, beamSize, BoxUVMapping{});
	}

	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 size,
		const double thickness,
		const BoxUVMapping& uvMapping)
	{
		return AppendBoxFrame(mesh, size, Vec3::All(thickness), uvMapping);
	}

	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 _size,
		const Vec3 _beamSize,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFloatRepresentable(_beamSize))
			|| (not IsFinite(uvMapping)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::BoxFrame(): size, beamSize, and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		const Float3 beamSize = _beamSize;
		const Float3 outerHalf = (size * 0.5f);
		if ((size.x <= 0.0f) || (size.y <= 0.0f) || (size.z <= 0.0f)
			|| (beamSize.x <= 0.0f) || (beamSize.y <= 0.0f) || (beamSize.z <= 0.0f)
			|| (outerHalf.x <= beamSize.x) || (outerHalf.y <= beamSize.y) || (outerHalf.z <= beamSize.z))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::BoxFrame(): Every size and beamSize component must remain positive, and beamSize must be smaller than half the corresponding size after conversion to float");
		}

		constexpr size_t QuadCount = 48;
		constexpr size_t VertexCount = (QuadCount * 4);
		constexpr size_t TriangleCount = (QuadCount * 2);
		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, VertexCount, TriangleCount, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::BoxFrame(): The generated mesh exceeds the supported size");
		}

		struct FrameFace
		{
			BoxFace mask;
			size_t axis;
			float sign;
		};

		constexpr std::array<FrameFace, 6> Faces
		{{
			{ BoxFace::NegativeX, 0, -1.0f },
			{ BoxFace::PositiveX, 0, 1.0f },
			{ BoxFace::NegativeY, 1, -1.0f },
			{ BoxFace::PositiveY, 1, 1.0f },
			{ BoxFace::NegativeZ, 2, -1.0f },
			{ BoxFace::PositiveZ, 2, 1.0f },
		}};
		const Float3 innerHalf = (outerHalf - beamSize);

		for (const FrameFace& face : Faces)
		{
			const size_t pAxis = ((face.axis + 1) % 3);
			const size_t qAxis = ((face.axis + 2) % 3);
			const float pOuter = outerHalf.elem(pAxis);
			const float qOuter = outerHalf.elem(qAxis);
			const float pInner = innerHalf.elem(pAxis);
			const float qInner = innerHalf.elem(qAxis);
			const float outerCoordinate = (face.sign * outerHalf.elem(face.axis));
			const float innerCoordinate = (face.sign * innerHalf.elem(face.axis));

			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, outerCoordinate, -pOuter, pOuter, -qOuter, -qInner, face.mask);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, outerCoordinate, -pOuter, pOuter, qInner, qOuter, face.mask);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, outerCoordinate, -pOuter, -pInner, -qInner, qInner, face.mask);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, outerCoordinate, pInner, pOuter, -qInner, qInner, face.mask);

			const BoxFace innerNormalFace = GetOppositeBoxFace(face.mask);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, innerCoordinate, -pInner, pInner, -qOuter, -qInner, innerNormalFace);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, innerCoordinate, -pInner, pInner, qInner, qOuter, innerNormalFace);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, innerCoordinate, -pOuter, -pInner, -qInner, qInner, innerNormalFace);
			WriteAxisAlignedBoxQuad(mesh, vertexOffset, triangleOffset, size, uvMapping,
				face.axis, innerCoordinate, pInner, pOuter, -qInner, qInner, innerNormalFace);
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - VertexCount),
			(mesh.indices.size() - TriangleCount));
	}

	Mesh3DAddResult AppendRoundedBox(
		Mesh3D& mesh,
		const Vec3 size,
		const double radius,
		const uint32 subdivisions)
	{
		return AppendRoundedBox(mesh, size, radius, subdivisions, BoxUVMapping{});
	}

	Mesh3DAddResult AppendRoundedBox(
		Mesh3D& mesh,
		const Vec3 _size,
		const double _radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFloatRepresentable(_radius))
			|| (not IsFinite(uvMapping))
			|| (_radius < 0.0)
			|| (subdivisions == 0))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::RoundedBox(): The size, radius, subdivisions, or UV mapping is invalid");
		}

		const Float3 size = _size;
		const float radius = static_cast<float>(_radius);
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::RoundedBox(): Every size component must be positive after conversion to float");
		}

		const double maxRadius = (_size.minComponent() * 0.5);
		if (maxRadius < _radius)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::RoundedBox(): radius must not exceed half of the smallest size component");
		}

		if (radius == 0.0f)
		{
			return AppendBox(mesh, size, uvMapping, BoxFace::All);
		}

		const Float3 halfSize = (size * 0.5f);
		const Float3 innerHalfSize{
			std::max((halfSize.x - radius), 0.0f),
			std::max((halfSize.y - radius), 0.0f),
			std::max((halfSize.z - radius), 0.0f)
		};
		size_t twiceSubdivisions;
		if (not CheckedMultiply(static_cast<size_t>(subdivisions), 2, twiceSubdivisions))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::RoundedBox(): subdivisions exceed the supported range");
		}

		const auto getAxisPointCount = [twiceSubdivisions](const float innerHalfExtent) -> size_t
		{
			return (twiceSubdivisions + ((innerHalfExtent == 0.0f) ? 1 : 2));
		};
		const size_t xPointCount = getAxisPointCount(innerHalfSize.x);
		const size_t yPointCount = getAxisPointCount(innerHalfSize.y);
		const size_t zPointCount = getAxisPointCount(innerHalfSize.z);
		size_t xyPointCount;
		size_t xzPointCount;
		size_t yzPointCount;
		size_t pointCountSum;
		size_t addedVertexCount;
		size_t xyCellCount;
		size_t xzCellCount;
		size_t yzCellCount;
		size_t cellCountSum;
		size_t addedTriangleCount;
		if ((not CheckedMultiply(xPointCount, yPointCount, xyPointCount))
			|| (not CheckedMultiply(xPointCount, zPointCount, xzPointCount))
			|| (not CheckedMultiply(yPointCount, zPointCount, yzPointCount))
			|| (not CheckedAdd(xyPointCount, xzPointCount, pointCountSum))
			|| (not CheckedAdd(pointCountSum, yzPointCount, pointCountSum))
			|| (not CheckedMultiply(pointCountSum, 2, addedVertexCount))
			|| (not CheckedMultiply((xPointCount - 1), (yPointCount - 1), xyCellCount))
			|| (not CheckedMultiply((xPointCount - 1), (zPointCount - 1), xzCellCount))
			|| (not CheckedMultiply((yPointCount - 1), (zPointCount - 1), yzCellCount))
			|| (not CheckedAdd(xyCellCount, xzCellCount, cellCountSum))
			|| (not CheckedAdd(cellCountSum, yzCellCount, cellCountSum))
			|| (not CheckedMultiply(cellCountSum, 4, addedTriangleCount)))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::RoundedBox(): The generated mesh exceeds the supported size");
		}

		const size_t vertexBase = mesh.vertices.size();
		const size_t triangleBase = mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if (not CheckAdditionSize(mesh, addedVertexCount, addedTriangleCount, newVertexCount, newTriangleCount))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::RoundedBox(): The generated mesh exceeds the supported size");
		}

		const size_t roundedSegmentCount = static_cast<size_t>(subdivisions);
		Array<float> roundedOffsets(roundedSegmentCount + 1);
		const float angleStep = (Math::QuarterPiF / static_cast<float>(subdivisions));
		roundedOffsets[0] = 0.0f;
		for (size_t i = 1; i < roundedSegmentCount; ++i)
		{
			roundedOffsets[i] = (radius * std::tan(angleStep * static_cast<float>(i)));
		}
		roundedOffsets[roundedSegmentCount] = radius;

		const auto makeAxisSamples = [&roundedOffsets, roundedSegmentCount](
			const float halfExtent,
			const float innerHalfExtent)
		{
			const size_t pointCount = ((roundedSegmentCount * 2)
				+ ((innerHalfExtent == 0.0f) ? 1 : 2));
			Array<float> samples(pointCount);

			for (size_t i = 0; i <= roundedSegmentCount; ++i)
			{
				samples[i] = (-innerHalfExtent - roundedOffsets[roundedSegmentCount - i]);
			}
			samples[0] = -halfExtent;

			if (innerHalfExtent == 0.0f)
			{
				for (size_t i = 1; i <= roundedSegmentCount; ++i)
				{
					const size_t index = (roundedSegmentCount + i);
					samples[index] = ((i == roundedSegmentCount)
						? halfExtent
						: roundedOffsets[i]);
				}
			}
			else
			{
				samples[roundedSegmentCount + 1] = innerHalfExtent;
				for (size_t i = 1; i <= roundedSegmentCount; ++i)
				{
					const size_t index = (roundedSegmentCount + 1 + i);
					samples[index] = ((i == roundedSegmentCount)
						? halfExtent
						: (innerHalfExtent + roundedOffsets[i]));
				}
			}

			return samples;
		};

		// Finish temporary sampling before committing the already checked destination size.
		const Array<float> xSamples = makeAxisSamples(halfSize.x, innerHalfSize.x);
		const Array<float> ySamples = makeAxisSamples(halfSize.y, innerHalfSize.y);
		const Array<float> zSamples = makeAxisSamples(halfSize.z, innerHalfSize.z);
		mesh.vertices.resize(newVertexCount);
		mesh.indices.resize(newTriangleCount);
		size_t vertexOffset = vertexBase;
		size_t triangleOffset = triangleBase;

		const auto writeFace = [
			&mesh, &vertexOffset, &triangleOffset, &innerHalfSize, radius](
			const Float3 center,
			const Float3 uAxis,
			const Float3 vAxis,
			const Array<float>& uSamples,
			const Array<float>& vSamples,
			const float uSize,
			const float vSize,
			const FloatRect& uvRect)
		{
			const size_t rowStride = uSamples.size();

			for (size_t v = 0; v < vSamples.size(); ++v)
			{
				for (size_t u = 0; u < uSamples.size(); ++u)
				{
					const Float3 cubePoint = (center
						+ (uAxis * uSamples[u])
						+ (vAxis * vSamples[v]));
					const Float3 innerPoint{
						std::clamp(cubePoint.x, -innerHalfSize.x, innerHalfSize.x),
						std::clamp(cubePoint.y, -innerHalfSize.y, innerHalfSize.y),
						std::clamp(cubePoint.z, -innerHalfSize.z, innerHalfSize.z)
					};
					const Float3 normal = (cubePoint - innerPoint).normalized();
					const Float3 position = (innerPoint + (normal * radius));
					const Float3 baseTangent = (uAxis - (normal * normal.dot(uAxis))).normalized();
					const Float2 projectedUV{
						(0.5f + (position.dot(uAxis) / uSize)),
						(0.5f + (position.dot(vAxis) / vSize))
					};
					mesh.vertices[vertexOffset + (v * rowStride) + u] = Vertex3D{
						.pos = position,
						.normal = normal,
						.tex = MapProjectedUV(uvRect, projectedUV),
						.tangent = MakeProjectedTangent(baseTangent, uvRect)
					};
				}
			}

			for (size_t v = 0; (v + 1) < vSamples.size(); ++v)
			{
				for (size_t u = 0; (u + 1) < uSamples.size(); ++u)
				{
					const uint32 i0 = static_cast<uint32>(vertexOffset + (v * rowStride) + u);
					const uint32 nextRow = (i0 + static_cast<uint32>(rowStride));
					mesh.indices[triangleOffset++] = TriangleIndex32{ i0, (i0 + 1), nextRow };
					mesh.indices[triangleOffset++] = TriangleIndex32{ nextRow, (i0 + 1), (nextRow + 1) };
				}
			}

			vertexOffset += (uSamples.size() * vSamples.size());
		};

		writeFace({ 0.0f, 0.0f, -halfSize.z },
			Float3::UnitX(), -Float3::UnitY(), xSamples, ySamples,
			size.x, size.y, uvMapping.negativeZ);
		writeFace({ 0.0f, 0.0f, halfSize.z },
			-Float3::UnitX(), -Float3::UnitY(), xSamples, ySamples,
			size.x, size.y, uvMapping.positiveZ);
		writeFace({ halfSize.x, 0.0f, 0.0f },
			Float3::UnitZ(), -Float3::UnitY(), zSamples, ySamples,
			size.z, size.y, uvMapping.positiveX);
		writeFace({ -halfSize.x, 0.0f, 0.0f },
			-Float3::UnitZ(), -Float3::UnitY(), zSamples, ySamples,
			size.z, size.y, uvMapping.negativeX);
		writeFace({ 0.0f, halfSize.y, 0.0f },
			Float3::UnitX(), -Float3::UnitZ(), xSamples, zSamples,
			size.x, size.z, uvMapping.positiveY);
		writeFace({ 0.0f, -halfSize.y, 0.0f },
			Float3::UnitX(), Float3::UnitZ(), xSamples, zSamples,
			size.x, size.z, uvMapping.negativeY);

		return AddedRange(mesh, vertexBase, triangleBase);
	}

	Mesh3DAddResult AppendChamferedBox(
		Mesh3D& mesh,
		const Vec3 size,
		const double chamfer)
	{
		return AppendChamferedBox(mesh, size, chamfer, BoxUVMapping{});
	}

	Mesh3DAddResult AppendChamferedBox(
		Mesh3D& mesh,
		const Vec3 _size,
		const double _chamfer,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFloatRepresentable(_chamfer))
			|| (not IsFinite(uvMapping))
			|| (_chamfer < 0.0))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::ChamferedBox(): The size, chamfer, or UV mapping is invalid");
		}

		const Float3 size = _size;
		const float chamfer = static_cast<float>(_chamfer);
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::ChamferedBox(): Every size component must be positive after conversion to float");
		}

		if (chamfer == 0.0f)
		{
			return AppendBox(mesh, size, uvMapping, BoxFace::All);
		}

		const float maxChamfer = (size.minComponent() * 0.5f);
		if (maxChamfer <= chamfer)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::ChamferedBox(): chamfer must be smaller than half of the smallest size component after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, 96, 44, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::ChamferedBox(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const Float3 innerHalfSize{
			(halfSize.x - chamfer),
			(halfSize.y - chamfer),
			(halfSize.z - chamfer)
		};
		constexpr float InvSqrt2 = Math::InvSqrt2_v<float>;
		constexpr float InvSqrt3 = Math::InvSqrt3_v<float>;

		const auto getUVRect = [&uvMapping](const BoxFace projection) -> const FloatRect&
		{
			switch (projection)
			{
			case BoxFace::NegativeX:
				return uvMapping.negativeX;
			case BoxFace::PositiveX:
				return uvMapping.positiveX;
			case BoxFace::NegativeY:
				return uvMapping.negativeY;
			case BoxFace::PositiveY:
				return uvMapping.positiveY;
			case BoxFace::PositiveZ:
				return uvMapping.positiveZ;
			default:
				return uvMapping.negativeZ;
			}
		};

		const auto projectUV = [size](const Float3 position, const BoxFace projection) noexcept
		{
			switch (projection)
			{
			case BoxFace::NegativeX:
				return Float2{ (0.5f - (position.z / size.z)), (0.5f - (position.y / size.y)) };
			case BoxFace::PositiveX:
				return Float2{ (0.5f + (position.z / size.z)), (0.5f - (position.y / size.y)) };
			case BoxFace::NegativeY:
				return Float2{ (0.5f + (position.x / size.x)), (0.5f + (position.z / size.z)) };
			case BoxFace::PositiveY:
				return Float2{ (0.5f + (position.x / size.x)), (0.5f - (position.z / size.z)) };
			case BoxFace::PositiveZ:
				return Float2{ (0.5f - (position.x / size.x)), (0.5f - (position.y / size.y)) };
			default:
				return Float2{ (0.5f + (position.x / size.x)), (0.5f - (position.y / size.y)) };
			}
		};

		const auto getBaseTangent = [](const BoxFace projection) noexcept
		{
			switch (projection)
			{
			case BoxFace::NegativeX:
				return -Float3::UnitZ();
			case BoxFace::PositiveX:
				return Float3::UnitZ();
			case BoxFace::PositiveZ:
				return -Float3::UnitX();
			default:
				return Float3::UnitX();
			}
		};

		const auto makeTangent = [&getBaseTangent](const Float3 normal, const BoxFace projection) noexcept
		{
			const Float3 baseTangent = getBaseTangent(projection);
			return (baseTangent - (normal * baseTangent.dot(normal))).normalized();
		};

		const auto writeQuad = [&](std::array<Float3, 4> positions, const Float3 normal, const BoxFace projection)
		{
			if ((positions[1] - positions[0]).cross(positions[2] - positions[0]).dot(normal) < 0.0f)
			{
				std::swap(positions[0], positions[1]);
				std::swap(positions[2], positions[3]);
			}

			std::array<Float2, 4> projectedUVs;
			for (size_t i = 0; i < positions.size(); ++i)
			{
				projectedUVs[i] = projectUV(positions[i], projection);
			}

			WriteProjectedQuad(
				mesh, vertexOffset, triangleOffset,
				positions, projectedUVs, normal, makeTangent(normal, projection), getUVRect(projection));
		};

		const auto writeTriangle = [&](std::array<Float3, 3> positions, const Float3 normal, const BoxFace projection)
		{
			if ((positions[1] - positions[0]).cross(positions[2] - positions[0]).dot(normal) < 0.0f)
			{
				std::swap(positions[1], positions[2]);
			}

			std::array<Float2, 3> projectedUVs;
			for (size_t i = 0; i < positions.size(); ++i)
			{
				projectedUVs[i] = projectUV(positions[i], projection);
			}

			WriteProjectedTriangle(
				mesh, vertexOffset, triangleOffset,
				positions, projectedUVs, normal, makeTangent(normal, projection), getUVRect(projection));
		};

		const float hx = halfSize.x;
		const float hy = halfSize.y;
		const float hz = halfSize.z;
		const float ix = innerHalfSize.x;
		const float iy = innerHalfSize.y;
		const float iz = innerHalfSize.z;

		writeQuad({{
			{ -ix, iy, -hz }, { ix, iy, -hz }, { -ix, -iy, -hz }, { ix, -iy, -hz }
		}}, -Float3::UnitZ(), BoxFace::NegativeZ);
		writeQuad({{
			{ ix, iy, hz }, { -ix, iy, hz }, { ix, -iy, hz }, { -ix, -iy, hz }
		}}, Float3::UnitZ(), BoxFace::PositiveZ);
		writeQuad({{
			{ hx, iy, -iz }, { hx, iy, iz }, { hx, -iy, -iz }, { hx, -iy, iz }
		}}, Float3::UnitX(), BoxFace::PositiveX);
		writeQuad({{
			{ -hx, iy, iz }, { -hx, iy, -iz }, { -hx, -iy, iz }, { -hx, -iy, -iz }
		}}, -Float3::UnitX(), BoxFace::NegativeX);
		writeQuad({{
			{ -ix, hy, iz }, { ix, hy, iz }, { -ix, hy, -iz }, { ix, hy, -iz }
		}}, Float3::UnitY(), BoxFace::PositiveY);
		writeQuad({{
			{ -ix, -hy, -iz }, { ix, -hy, -iz }, { -ix, -hy, iz }, { ix, -hy, iz }
		}}, -Float3::UnitY(), BoxFace::NegativeY);

		constexpr std::array<float, 2> Signs{ -1.0f, 1.0f };
		for (const float sy : Signs)
		{
			for (const float sz : Signs)
			{
				const Float3 normal{ 0.0f, (sy * InvSqrt2), (sz * InvSqrt2) };
				const BoxFace projection = ((0.0f < sy) ? BoxFace::PositiveY : BoxFace::NegativeY);
				writeQuad({{
					{ -ix, (sy * hy), (sz * iz) }, { ix, (sy * hy), (sz * iz) },
					{ -ix, (sy * iy), (sz * hz) }, { ix, (sy * iy), (sz * hz) }
				}}, normal, projection);
			}
		}

		for (const float sx : Signs)
		{
			for (const float sz : Signs)
			{
				const Float3 normal{ (sx * InvSqrt2), 0.0f, (sz * InvSqrt2) };
				const BoxFace projection = ((0.0f < sx) ? BoxFace::PositiveX : BoxFace::NegativeX);
				writeQuad({{
					{ (sx * hx), -iy, (sz * iz) }, { (sx * hx), iy, (sz * iz) },
					{ (sx * ix), -iy, (sz * hz) }, { (sx * ix), iy, (sz * hz) }
				}}, normal, projection);
			}
		}

		for (const float sx : Signs)
		{
			for (const float sy : Signs)
			{
				const Float3 normal{ (sx * InvSqrt2), (sy * InvSqrt2), 0.0f };
				const BoxFace projection = ((0.0f < sx) ? BoxFace::PositiveX : BoxFace::NegativeX);
				writeQuad({{
					{ (sx * hx), (sy * iy), -iz }, { (sx * hx), (sy * iy), iz },
					{ (sx * ix), (sy * hy), -iz }, { (sx * ix), (sy * hy), iz }
				}}, normal, projection);
			}
		}

		for (const float sx : Signs)
		{
			for (const float sy : Signs)
			{
				for (const float sz : Signs)
				{
					const Float3 normal{
						(sx * InvSqrt3), (sy * InvSqrt3), (sz * InvSqrt3)
					};
					const BoxFace projection = ((0.0f < sx) ? BoxFace::PositiveX : BoxFace::NegativeX);
					writeTriangle({{
						{ (sx * hx), (sy * iy), (sz * iz) },
						{ (sx * ix), (sy * hy), (sz * iz) },
						{ (sx * ix), (sy * iy), (sz * hz) }
					}}, normal, projection);
				}
			}
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 96),
			(mesh.indices.size() - 44));
	}

	Mesh3DAddResult AppendWedge(
		Mesh3D& mesh,
		const Vec3 size)
	{
		return AppendWedge(mesh, size, BoxUVMapping{});
	}

	Mesh3DAddResult AppendWedge(
		Mesh3D& mesh,
		const Vec3 _size,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFinite(uvMapping)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Wedge(): size and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Wedge(): Every size component must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, 18, 8, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Wedge(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const float left = -halfSize.x;
		const float right = halfSize.x;
		const float bottom = -halfSize.y;
		const float top = halfSize.y;
		const float front = -halfSize.z;
		const float back = halfSize.z;
		const double inverseSlopeLength = (1.0 / std::hypot(_size.y, _size.z));
		const Float3 slopeNormal{
			0.0f,
			static_cast<float>(_size.z * inverseSlopeLength),
			static_cast<float>(-_size.y * inverseSlopeLength)
		};

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ right, top, back }, { left, top, back },
				{ right, bottom, back }, { left, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			Float3::UnitZ(), -Float3::UnitX(), uvMapping.positiveZ);

		WriteProjectedTriangle(
			mesh, vertexOffset, triangleOffset,
			{{
				{ right, bottom, front },
				{ right, top, back },
				{ right, bottom, back }
			}},
			{{ { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }},
			Float3::UnitX(), Float3::UnitZ(), uvMapping.positiveX);

		WriteProjectedTriangle(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front },
				{ left, bottom, back },
				{ left, top, back }
			}},
			{{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f } }},
			-Float3::UnitX(), -Float3::UnitZ(), uvMapping.negativeX);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, top, back }, { right, top, back },
				{ left, bottom, front }, { right, bottom, front }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			slopeNormal, Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front }, { right, bottom, front },
				{ left, bottom, back }, { right, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 18),
			(mesh.indices.size() - 8));
	}

	Mesh3DAddResult AppendTriangularPrism(
		Mesh3D& mesh,
		const Vec3 size)
	{
		return AppendTriangularPrism(mesh, size, BoxUVMapping{});
	}

	Mesh3DAddResult AppendTriangularPrism(
		Mesh3D& mesh,
		const Vec3 _size,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFinite(uvMapping)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::TriangularPrism(): size and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::TriangularPrism(): Every size component must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, 18, 8, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::TriangularPrism(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const float left = -halfSize.x;
		const float right = halfSize.x;
		const float bottom = -halfSize.y;
		const float top = halfSize.y;
		const float front = -halfSize.z;
		const float back = halfSize.z;
		const float inverseSlopeLength = static_cast<float>(1.0 / std::hypot(_size.y, (_size.z * 0.5)));
		const Float3 frontSlopeNormal{ 0.0f, (halfSize.z * inverseSlopeLength), (-size.y * inverseSlopeLength) };
		const Float3 backSlopeNormal{ 0.0f, (halfSize.z * inverseSlopeLength), (size.y * inverseSlopeLength) };

		WriteProjectedTriangle(
			mesh, vertexOffset, triangleOffset,
			{{
				{ right, bottom, back },
				{ right, bottom, front },
				{ right, top, 0.0f }
			}},
			{{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.5f, 0.0f } }},
			Float3::UnitX(), Float3::UnitZ(), uvMapping.positiveX);

		WriteProjectedTriangle(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front },
				{ left, bottom, back },
				{ left, top, 0.0f }
			}},
			{{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.5f, 0.0f } }},
			-Float3::UnitX(), -Float3::UnitZ(), uvMapping.negativeX);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, top, 0.0f }, { right, top, 0.0f },
				{ left, bottom, front }, { right, bottom, front }
			}},
			{{ { 0.0f, 0.5f }, { 1.0f, 0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			frontSlopeNormal, Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ right, top, 0.0f }, { left, top, 0.0f },
				{ right, bottom, back }, { left, bottom, back }
			}},
			{{ { 1.0f, 0.5f }, { 0.0f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f } }},
			backSlopeNormal, Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front }, { right, bottom, front },
				{ left, bottom, back }, { right, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 18),
			(mesh.indices.size() - 8));
	}

	Mesh3DAddResult AppendStairs(
		Mesh3D& mesh,
		const Vec3 size,
		const uint32 steps)
	{
		return AppendStairs(mesh, size, steps, BoxUVMapping{});
	}

	Mesh3DAddResult AppendStairs(
		Mesh3D& mesh,
		const Vec3 _size,
		const uint32 steps,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFinite(uvMapping)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Stairs(): size and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f)
			|| (steps == 0))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Stairs(): Every size component and the step count must be positive");
		}

		size_t addedVertexCount;
		size_t addedTriangleCount;
		if ((not CheckedMultiply(static_cast<size_t>(steps), 16, addedVertexCount))
			|| (not CheckedAdd(addedVertexCount, 8, addedVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(steps), 8, addedTriangleCount))
			|| (not CheckedAdd(addedTriangleCount, 4, addedTriangleCount)))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Stairs(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			mesh, addedVertexCount, addedTriangleCount, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Stairs(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const float left = -halfSize.x;
		const float right = halfSize.x;
		const float bottom = -halfSize.y;
		const float top = halfSize.y;
		const float front = -halfSize.z;
		const float back = halfSize.z;
		const float inverseSteps = (1.0f / static_cast<float>(steps));

		for (uint32 i = 0; i < steps; ++i)
		{
			const float t0 = (i * inverseSteps);
			const float t1 = ((i + 1) * inverseSteps);
			const float y0 = (bottom + (size.y * t0));
			const float y1 = ((i + 1 == steps) ? top : (bottom + (size.y * t1)));
			const float z0 = (front + (size.z * t0));
			const float z1 = ((i + 1 == steps) ? back : (front + (size.z * t1)));

			WriteProjectedQuad(
				mesh, vertexOffset, triangleOffset,
				{{
					{ left, y1, z1 }, { right, y1, z1 },
					{ left, y1, z0 }, { right, y1, z0 }
				}},
				{{ { 0.0f, (1.0f - t1) }, { 1.0f, (1.0f - t1) }, { 0.0f, (1.0f - t0) }, { 1.0f, (1.0f - t0) } }},
				Float3::UnitY(), Float3::UnitX(), uvMapping.positiveY);

			WriteProjectedQuad(
				mesh, vertexOffset, triangleOffset,
				{{
					{ left, y1, z0 }, { right, y1, z0 },
					{ left, y0, z0 }, { right, y0, z0 }
				}},
				{{ { 0.0f, (1.0f - t1) }, { 1.0f, (1.0f - t1) }, { 0.0f, (1.0f - t0) }, { 1.0f, (1.0f - t0) } }},
				-Float3::UnitZ(), Float3::UnitX(), uvMapping.negativeZ);
		}

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ right, top, back }, { left, top, back },
				{ right, bottom, back }, { left, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			Float3::UnitZ(), -Float3::UnitX(), uvMapping.positiveZ);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front }, { right, bottom, front },
				{ left, bottom, back }, { right, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		for (uint32 i = 0; i < steps; ++i)
		{
			const float t0 = (i * inverseSteps);
			const float t1 = ((i + 1) * inverseSteps);
			const float y1 = ((i + 1 == steps) ? top : (bottom + (size.y * t1)));
			const float z0 = (front + (size.z * t0));
			const float z1 = ((i + 1 == steps) ? back : (front + (size.z * t1)));

			WriteProjectedQuad(
				mesh, vertexOffset, triangleOffset,
				{{
					{ right, y1, z0 }, { right, y1, z1 },
					{ right, bottom, z0 }, { right, bottom, z1 }
				}},
				{{ { t0, (1.0f - t1) }, { t1, (1.0f - t1) }, { t0, 1.0f }, { t1, 1.0f } }},
				Float3::UnitX(), Float3::UnitZ(), uvMapping.positiveX);

			WriteProjectedQuad(
				mesh, vertexOffset, triangleOffset,
				{{
					{ left, y1, z1 }, { left, y1, z0 },
					{ left, bottom, z1 }, { left, bottom, z0 }
				}},
				{{ { (1.0f - t1), (1.0f - t1) }, { (1.0f - t0), (1.0f - t1) }, { (1.0f - t1), 1.0f }, { (1.0f - t0), 1.0f } }},
				-Float3::UnitX(), -Float3::UnitZ(), uvMapping.negativeX);
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - addedVertexCount),
			(mesh.indices.size() - addedTriangleCount));
	}

	Mesh3DAddResult AppendPyramid(
		Mesh3D& mesh,
		const double baseSize,
		const double height)
	{
		return AppendPyramid(mesh, SizeF{ baseSize, baseSize }, height);
	}

	Mesh3DAddResult AppendPyramid(
		Mesh3D& mesh,
		const SizeF _baseSizeXZ,
		const double _height)
	{
		if ((not IsFloatRepresentable(_baseSizeXZ))
			|| (not IsFloatRepresentable(_height)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Pyramid(): baseSizeXZ and height must be finite and float-representable");
		}

		const Float2 baseSizeXZ = _baseSizeXZ;
		const float height = static_cast<float>(_height);
		if ((baseSizeXZ.x <= 0.0f)
			|| (baseSizeXZ.y <= 0.0f)
			|| (height <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Pyramid(): Every base size component and height must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, 16, 6, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Pyramid(): The generated mesh exceeds the supported size");
		}

		const float halfX = (baseSizeXZ.x * 0.5f);
		const float halfZ = (baseSizeXZ.y * 0.5f);
		const float halfHeight = (height * 0.5f);
		const Float3 apex{ 0.0f, halfHeight, 0.0f };
		const float inverseZSideLength = (1.0f / std::hypot(height, halfZ));
		const float inverseXSideLength = (1.0f / std::hypot(height, halfX));

		struct Side
		{
			Float3 firstBase;
			Float3 secondBase;
			Float3 normal;
			Float3 tangent;
		};

		const std::array<Side, 4> sides =
		{{
			{
				{ halfX, -halfHeight, -halfZ },
				{ -halfX, -halfHeight, -halfZ },
				(Float3{ 0.0f, halfZ, -height } * inverseZSideLength),
				Float3::UnitX()
			},
			{
				{ -halfX, -halfHeight, halfZ },
				{ halfX, -halfHeight, halfZ },
				(Float3{ 0.0f, halfZ, height } * inverseZSideLength),
				-Float3::UnitX()
			},
			{
				{ halfX, -halfHeight, halfZ },
				{ halfX, -halfHeight, -halfZ },
				(Float3{ height, halfX, 0.0f } * inverseXSideLength),
				Float3::UnitZ()
			},
			{
				{ -halfX, -halfHeight, -halfZ },
				{ -halfX, -halfHeight, halfZ },
				(Float3{ -height, halfX, 0.0f } * inverseXSideLength),
				-Float3::UnitZ()
			},
		}};

		for (size_t faceIndex = 0; faceIndex < sides.size(); ++faceIndex)
		{
			const Side& side = sides[faceIndex];
			const Float4 tangent{ side.tangent, 1.0f };
			const size_t vertexBase = (vertexOffset + (faceIndex * 3));
			mesh.vertices[vertexBase + 0] = Vertex3D{
				.pos = side.firstBase,
				.normal = side.normal,
				.tex = Float2{ 1.0f, 1.0f },
				.tangent = tangent
			};
			mesh.vertices[vertexBase + 1] = Vertex3D{
				.pos = side.secondBase,
				.normal = side.normal,
				.tex = Float2{ 0.0f, 1.0f },
				.tangent = tangent
			};
			mesh.vertices[vertexBase + 2] = Vertex3D{
				.pos = apex,
				.normal = side.normal,
				.tex = Float2{ 0.5f, 0.0f },
				.tangent = tangent
			};

			const uint32 i0 = static_cast<uint32>(vertexBase);
			mesh.indices[triangleOffset + faceIndex] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
		}

		const size_t bottomVertexBase = (vertexOffset + 12);
		const Float3 bottomNormal = -Float3::UnitY();
		const Float4 bottomTangent{ 1.0f, 0.0f, 0.0f, 1.0f };
		mesh.vertices[bottomVertexBase + 0] = Vertex3D{
			.pos = Float3{ -halfX, -halfHeight, -halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 0.0f, 0.0f },
			.tangent = bottomTangent
		};
		mesh.vertices[bottomVertexBase + 1] = Vertex3D{
			.pos = Float3{ halfX, -halfHeight, -halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 1.0f, 0.0f },
			.tangent = bottomTangent
		};
		mesh.vertices[bottomVertexBase + 2] = Vertex3D{
			.pos = Float3{ -halfX, -halfHeight, halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 0.0f, 1.0f },
			.tangent = bottomTangent
		};
		mesh.vertices[bottomVertexBase + 3] = Vertex3D{
			.pos = Float3{ halfX, -halfHeight, halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 1.0f, 1.0f },
			.tangent = bottomTangent
		};

		const uint32 bottomIndexBase = static_cast<uint32>(bottomVertexBase);
		mesh.indices[triangleOffset + 4] = TriangleIndex32{
			bottomIndexBase, (bottomIndexBase + 1), (bottomIndexBase + 2) };
		mesh.indices[triangleOffset + 5] = TriangleIndex32{
			(bottomIndexBase + 2), (bottomIndexBase + 1), (bottomIndexBase + 3) };
		return AddedRange(
			mesh,
			(mesh.vertices.size() - 16),
			(mesh.indices.size() - 6));
	}

	Mesh3DAddResult AppendRectangularFrustum(
		Mesh3D& mesh,
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height)
	{
		return AppendRectangularFrustum(mesh, bottomSizeXZ, topSizeXZ, height, BoxUVMapping{});
	}

	Mesh3DAddResult AppendRectangularFrustum(
		Mesh3D& mesh,
		const SizeF _bottomSizeXZ,
		const SizeF _topSizeXZ,
		const double _height,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_bottomSizeXZ))
			|| (not IsFloatRepresentable(_topSizeXZ))
			|| (not IsFloatRepresentable(_height))
			|| (not IsFinite(uvMapping)))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::RectangularFrustum(): The sizes, height, and UV mapping must be finite and float-representable");
		}

		const Float2 bottomSizeXZ = _bottomSizeXZ;
		const Float2 topSizeXZ = _topSizeXZ;
		const float height = static_cast<float>(_height);
		if ((bottomSizeXZ.x <= 0.0f)
			|| (bottomSizeXZ.y <= 0.0f)
			|| (topSizeXZ.x <= 0.0f)
			|| (topSizeXZ.y <= 0.0f)
			|| (height <= 0.0f))
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::RectangularFrustum(): Every size component and height must be positive after conversion to float");
		}

		if (bottomSizeXZ == topSizeXZ)
		{
			return AppendBox(mesh, Vec3{ bottomSizeXZ.x, height, bottomSizeXZ.y }, uvMapping, BoxFace::All);
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, 24, 12, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::RectangularFrustum(): The generated mesh exceeds the supported size");
		}

		const Float2 bottomHalf = (bottomSizeXZ * 0.5f);
		const Float2 topHalf = (topSizeXZ * 0.5f);
		const Float2 boundsHalf{
			std::max(bottomHalf.x, topHalf.x),
			std::max(bottomHalf.y, topHalf.y)
		};
		const float halfHeight = (height * 0.5f);
		const float xDelta = (bottomHalf.x - topHalf.x);
		const float zDelta = (bottomHalf.y - topHalf.y);
		const float inverseXSideLength = static_cast<float>(1.0 / std::hypot(_height, static_cast<double>(xDelta)));
		const float inverseZSideLength = static_cast<float>(1.0 / std::hypot(_height, static_cast<double>(zDelta)));
		const Float3 negativeZNormal{ 0.0f, (zDelta * inverseZSideLength), (-height * inverseZSideLength) };
		const Float3 positiveZNormal{ 0.0f, (zDelta * inverseZSideLength), (height * inverseZSideLength) };
		const Float3 positiveXNormal{ (height * inverseXSideLength), (xDelta * inverseXSideLength), 0.0f };
		const Float3 negativeXNormal{ (-height * inverseXSideLength), (xDelta * inverseXSideLength), 0.0f };
		const auto mapX = [boundsHalf](const float x) { return (0.5f + (x / (2.0f * boundsHalf.x))); };
		const auto mapZ = [boundsHalf](const float z) { return (0.5f + (z / (2.0f * boundsHalf.y))); };

		const float bottomLeft = -bottomHalf.x;
		const float bottomRight = bottomHalf.x;
		const float bottomFront = -bottomHalf.y;
		const float bottomBack = bottomHalf.y;
		const float topLeft = -topHalf.x;
		const float topRight = topHalf.x;
		const float topFront = -topHalf.y;
		const float topBack = topHalf.y;

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ topLeft, halfHeight, topFront }, { topRight, halfHeight, topFront },
				{ bottomLeft, -halfHeight, bottomFront }, { bottomRight, -halfHeight, bottomFront }
			}},
			{{
				{ mapX(topLeft), 0.0f }, { mapX(topRight), 0.0f },
				{ mapX(bottomLeft), 1.0f }, { mapX(bottomRight), 1.0f }
			}},
			negativeZNormal, Float3::UnitX(), uvMapping.negativeZ);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ topRight, halfHeight, topBack }, { topLeft, halfHeight, topBack },
				{ bottomRight, -halfHeight, bottomBack }, { bottomLeft, -halfHeight, bottomBack }
			}},
			{{
				{ (1.0f - mapX(topRight)), 0.0f }, { (1.0f - mapX(topLeft)), 0.0f },
				{ (1.0f - mapX(bottomRight)), 1.0f }, { (1.0f - mapX(bottomLeft)), 1.0f }
			}},
			positiveZNormal, -Float3::UnitX(), uvMapping.positiveZ);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ topRight, halfHeight, topFront }, { topRight, halfHeight, topBack },
				{ bottomRight, -halfHeight, bottomFront }, { bottomRight, -halfHeight, bottomBack }
			}},
			{{
				{ mapZ(topFront), 0.0f }, { mapZ(topBack), 0.0f },
				{ mapZ(bottomFront), 1.0f }, { mapZ(bottomBack), 1.0f }
			}},
			positiveXNormal, Float3::UnitZ(), uvMapping.positiveX);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ topLeft, halfHeight, topBack }, { topLeft, halfHeight, topFront },
				{ bottomLeft, -halfHeight, bottomBack }, { bottomLeft, -halfHeight, bottomFront }
			}},
			{{
				{ (1.0f - mapZ(topBack)), 0.0f }, { (1.0f - mapZ(topFront)), 0.0f },
				{ (1.0f - mapZ(bottomBack)), 1.0f }, { (1.0f - mapZ(bottomFront)), 1.0f }
			}},
			negativeXNormal, -Float3::UnitZ(), uvMapping.negativeX);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ topLeft, halfHeight, topBack }, { topRight, halfHeight, topBack },
				{ topLeft, halfHeight, topFront }, { topRight, halfHeight, topFront }
			}},
			{{
				{ mapX(topLeft), (1.0f - mapZ(topBack)) }, { mapX(topRight), (1.0f - mapZ(topBack)) },
				{ mapX(topLeft), (1.0f - mapZ(topFront)) }, { mapX(topRight), (1.0f - mapZ(topFront)) }
			}},
			Float3::UnitY(), Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			mesh, vertexOffset, triangleOffset,
			{{
				{ bottomLeft, -halfHeight, bottomFront }, { bottomRight, -halfHeight, bottomFront },
				{ bottomLeft, -halfHeight, bottomBack }, { bottomRight, -halfHeight, bottomBack }
			}},
			{{
				{ mapX(bottomLeft), mapZ(bottomFront) }, { mapX(bottomRight), mapZ(bottomFront) },
				{ mapX(bottomLeft), mapZ(bottomBack) }, { mapX(bottomRight), mapZ(bottomBack) }
			}},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 24),
			(mesh.indices.size() - 12));
	}
}
