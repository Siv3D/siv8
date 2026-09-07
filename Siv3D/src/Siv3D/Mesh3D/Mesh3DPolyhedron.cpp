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
# include <unordered_map>

namespace s3d::Mesh3DDetail
{
	namespace
	{
		constexpr float GoldenRatio = 1.6180339887498948482f;

		constexpr std::array<Float3, 12> IcosahedronBaseVertices =
		{{
			{ -1.0f, GoldenRatio, 0.0f },
			{ 1.0f, GoldenRatio, 0.0f },
			{ -1.0f, -GoldenRatio, 0.0f },
			{ 1.0f, -GoldenRatio, 0.0f },
			{ 0.0f, -1.0f, GoldenRatio },
			{ 0.0f, 1.0f, GoldenRatio },
			{ 0.0f, -1.0f, -GoldenRatio },
			{ 0.0f, 1.0f, -GoldenRatio },
			{ GoldenRatio, 0.0f, -1.0f },
			{ GoldenRatio, 0.0f, 1.0f },
			{ -GoldenRatio, 0.0f, -1.0f },
			{ -GoldenRatio, 0.0f, 1.0f },
		}};

		constexpr std::array<std::array<uint32, 3>, 20> IcosahedronFaces =
		{{
			{ 0, 11, 5 }, { 0, 5, 1 }, { 0, 1, 7 }, { 0, 7, 10 }, { 0, 10, 11 },
			{ 1, 5, 9 }, { 5, 11, 4 }, { 11, 10, 2 }, { 10, 7, 6 }, { 7, 1, 8 },
			{ 3, 9, 4 }, { 3, 4, 2 }, { 3, 2, 6 }, { 3, 6, 8 }, { 3, 8, 9 },
			{ 4, 9, 5 }, { 2, 4, 11 }, { 6, 2, 10 }, { 8, 6, 7 }, { 9, 8, 1 },
		}};

		constexpr uint32 IcoSphereMaxSubdivisions = 8;

		[[nodiscard]]
		static constexpr uint64 MakeEdgeKey(const uint32 a, const uint32 b) noexcept
		{
			const uint32 low = std::min(a, b);
			const uint32 high = std::max(a, b);
			return ((static_cast<uint64>(low) << 32) | high);
		}

		template <size_t VertexCount, size_t FaceCount>
		[[nodiscard]]
		static bool AddTriangleFacedPolyhedron(
			Mesh3D& mesh,
			const float radius,
			const std::array<Float3, VertexCount>& baseVertices,
			const float baseRadius,
			const std::array<std::array<uint32, 3>, FaceCount>& faces)
		{
			size_t vertexOffset;
			size_t triangleOffset;
			if (not ResizeForAddition(mesh, (FaceCount * 3), FaceCount, vertexOffset, triangleOffset))
			{
				return false;
			}

			const float scale = (radius / baseRadius);
			for (size_t faceIndex = 0; faceIndex < FaceCount; ++faceIndex)
			{
				Float3 p0 = baseVertices[faces[faceIndex][0]];
				Float3 p1 = baseVertices[faces[faceIndex][1]];
				Float3 p2 = baseVertices[faces[faceIndex][2]];
				Float3 normal = (p1 - p0).cross(p2 - p0).normalized();

				if (normal.dot(p0 + p1 + p2) < 0.0f)
				{
					std::swap(p1, p2);
					normal = -normal;
				}

				const Float4 faceTangent{ (p0 - p1).normalized(), 1.0f };
				const size_t vertexBase = (vertexOffset + (faceIndex * 3));
				mesh.vertices[vertexBase + 0] = Vertex3D{
					.pos = (p0 * scale),
					.normal = normal,
					.tex = Float2{ 1.0f, 1.0f },
					.tangent = faceTangent
				};
				mesh.vertices[vertexBase + 1] = Vertex3D{
					.pos = (p1 * scale),
					.normal = normal,
					.tex = Float2{ 0.0f, 1.0f },
					.tangent = faceTangent
				};
				mesh.vertices[vertexBase + 2] = Vertex3D{
					.pos = (p2 * scale),
					.normal = normal,
					.tex = Float2{ 0.5f, 0.0f },
					.tangent = faceTangent
				};

				const uint32 i0 = static_cast<uint32>(vertexBase);
				mesh.indices[triangleOffset + faceIndex] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			}

			return true;
		}

		struct DodecahedronData
		{
			std::array<Float3, 20> vertices;
			std::array<std::array<uint32, 5>, 12> faces;
		};

		[[nodiscard]]
		static const DodecahedronData& GetDodecahedronData()
		{
			static const DodecahedronData data = []
			{
				DodecahedronData result;

				for (size_t faceIndex = 0; faceIndex < IcosahedronFaces.size(); ++faceIndex)
				{
					const auto& face = IcosahedronFaces[faceIndex];
					result.vertices[faceIndex] = (IcosahedronBaseVertices[face[0]]
						+ IcosahedronBaseVertices[face[1]]
						+ IcosahedronBaseVertices[face[2]]).normalized();
				}

				for (uint32 vertexIndex = 0; vertexIndex < IcosahedronBaseVertices.size(); ++vertexIndex)
				{
					const Float3 axis = IcosahedronBaseVertices[vertexIndex].normalized();
					const Float3 helper = ((std::abs(axis.y) < 0.9f) ? Float3::UnitY() : Float3::UnitX());
					const Float3 u = helper.cross(axis).normalized();
					const Float3 v = axis.cross(u);
					std::array<std::pair<float, uint32>, 5> adjacentFaces;
					size_t adjacentCount = 0;

					for (uint32 faceIndex = 0; faceIndex < IcosahedronFaces.size(); ++faceIndex)
					{
						const auto& face = IcosahedronFaces[faceIndex];
						if ((face[0] == vertexIndex)
							|| (face[1] == vertexIndex)
							|| (face[2] == vertexIndex))
						{
							const Float3 center = result.vertices[faceIndex];
							adjacentFaces[adjacentCount++] = {
								std::atan2(center.dot(v), center.dot(u)), faceIndex
							};
						}
					}

					std::sort(adjacentFaces.begin(), adjacentFaces.end());
					for (size_t i = 0; i < adjacentFaces.size(); ++i)
					{
						result.faces[vertexIndex][i] = adjacentFaces[i].second;
					}
				}

				return result;
			}();

			return data;
		}
	}

	Mesh3DAddResult AppendTetrahedron(
		Mesh3D& mesh,
		const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Tetrahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Tetrahedron(): radius must be positive after conversion to float");
		}

		constexpr std::array<Float3, 4> Vertices =
		{{
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ -1.0f, -1.0f, 1.0f },
		}};
		constexpr std::array<std::array<uint32, 3>, 4> Faces =
		{{
			{ 0, 1, 2 }, { 0, 3, 1 }, { 0, 2, 3 }, { 1, 3, 2 },
		}};

		if (not AddTriangleFacedPolyhedron(
			mesh, radius, Vertices, 1.7320508075688772935f, Faces))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Tetrahedron(): The generated mesh exceeds the supported size");
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 12),
			(mesh.indices.size() - 4));
	}

	Mesh3DAddResult AppendOctahedron(
		Mesh3D& mesh,
		const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Octahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Octahedron(): radius must be positive after conversion to float");
		}

		constexpr std::array<Float3, 6> Vertices =
		{{
			{ 1.0f, 0.0f, 0.0f },
			{ -1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, -1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, -1.0f },
		}};
		constexpr std::array<std::array<uint32, 3>, 8> Faces =
		{{
			{ 2, 4, 0 }, { 2, 1, 4 }, { 2, 5, 1 }, { 2, 0, 5 },
			{ 3, 0, 4 }, { 3, 4, 1 }, { 3, 1, 5 }, { 3, 5, 0 },
		}};

		if (not AddTriangleFacedPolyhedron(mesh, radius, Vertices, 1.0f, Faces))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Octahedron(): The generated mesh exceeds the supported size");
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 24),
			(mesh.indices.size() - 8));
	}

	Mesh3DAddResult AppendIcosahedron(
		Mesh3D& mesh,
		const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Icosahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Icosahedron(): radius must be positive after conversion to float");
		}

		if (not AddTriangleFacedPolyhedron(
			mesh,
			radius,
			IcosahedronBaseVertices,
			1.9021130325903071442f,
			IcosahedronFaces))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Icosahedron(): The generated mesh exceeds the supported size");
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 60),
			(mesh.indices.size() - 20));
	}

	Mesh3DAddResult AppendIcoSphere(
		Mesh3D& mesh,
		const double _radius,
		const uint32 subdivisions)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange,
				U"Mesh3D::IcoSphere(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::IcoSphere(): radius must be positive after conversion to float");
		}

		if (IcoSphereMaxSubdivisions < subdivisions)
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit,
				U"Mesh3D::IcoSphere(): subdivisions must not exceed 8");
		}

		size_t subdivisionScale = 1;
		for (uint32 i = 0; i < subdivisions; ++i)
		{
			size_t nextScale;
			if (not CheckedMultiply(subdivisionScale, size_t{ 4 }, nextScale))
			{
				return OperationFailed(Mesh3DErrorCode::SizeLimit,
					U"Mesh3D::IcoSphere(): The generated mesh exceeds the supported size");
			}
			subdivisionScale = nextScale;
		}

		size_t vertexCount;
		size_t triangleCount;
		size_t scaledVertexCount;
		if ((not CheckedMultiply(size_t{ 10 }, subdivisionScale, scaledVertexCount))
			|| (not CheckedAdd(scaledVertexCount, size_t{ 2 }, vertexCount))
			|| (not CheckedMultiply(size_t{ 20 }, subdivisionScale, triangleCount)))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit,
				U"Mesh3D::IcoSphere(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit,
				U"Mesh3D::IcoSphere(): The generated mesh exceeds the supported size");
		}

		for (size_t i = 0; i < IcosahedronBaseVertices.size(); ++i)
		{
			mesh.vertices[vertexOffset + i].pos = IcosahedronBaseVertices[i].normalized();
		}

		for (size_t i = 0; i < IcosahedronFaces.size(); ++i)
		{
			const auto& face = IcosahedronFaces[i];
			mesh.indices[triangleOffset + i] = TriangleIndex32{
				face[0], face[1], face[2]
			};
		}

		uint32 nextVertexIndex = static_cast<uint32>(IcosahedronBaseVertices.size());
		size_t currentTriangleCount = IcosahedronFaces.size();
		std::unordered_map<uint64, uint32> midpointIndices;
		for (uint32 level = 0; level < subdivisions; ++level)
		{
			midpointIndices.clear();
			midpointIndices.reserve((currentTriangleCount * 3) / 2);

			const auto getMidpointIndex = [&](const uint32 a, const uint32 b)
			{
				const uint64 key = MakeEdgeKey(a, b);
				if (const auto it = midpointIndices.find(key); it != midpointIndices.end())
				{
					return it->second;
				}

				const uint32 midpointIndex = nextVertexIndex++;
				mesh.vertices[vertexOffset + midpointIndex].pos =
					(mesh.vertices[vertexOffset + a].pos
						+ mesh.vertices[vertexOffset + b].pos).normalized();
				midpointIndices.emplace(key, midpointIndex);
				return midpointIndex;
			};

			for (size_t faceIndex = currentTriangleCount; faceIndex-- > 0;)
			{
				const TriangleIndex32 face = mesh.indices[triangleOffset + faceIndex];
				const uint32 ab = getMidpointIndex(face.i0, face.i1);
				const uint32 bc = getMidpointIndex(face.i1, face.i2);
				const uint32 ca = getMidpointIndex(face.i2, face.i0);
				const size_t childBase = (triangleOffset + faceIndex * 4);
				mesh.indices[childBase + 0] = TriangleIndex32{ face.i0, ab, ca };
				mesh.indices[childBase + 1] = TriangleIndex32{ face.i1, bc, ab };
				mesh.indices[childBase + 2] = TriangleIndex32{ face.i2, ca, bc };
				mesh.indices[childBase + 3] = TriangleIndex32{ ab, bc, ca };
			}

			currentTriangleCount *= 4;
		}

		for (size_t i = 0; i < vertexCount; ++i)
		{
			Vertex3D& vertex = mesh.vertices[vertexOffset + i];
			const Float3 normal = vertex.pos;
			const Float3 helper = ((std::abs(normal.y) < 0.9f)
				? Float3::UnitY() : Float3::UnitX());
			const Float3 tangent = helper.cross(normal).normalized();
			vertex = Vertex3D{
				.pos = (normal * radius),
				.normal = normal,
				.tex = Float2{ 0.0f, 0.0f },
				.tangent = Float4{ tangent, 1.0f }
			};
		}

		for (size_t i = 0; i < triangleCount; ++i)
		{
			TriangleIndex32& face = mesh.indices[triangleOffset + i];
			face.i0 = static_cast<uint32>(vertexOffset + face.i0);
			face.i1 = static_cast<uint32>(vertexOffset + face.i1);
			face.i2 = static_cast<uint32>(vertexOffset + face.i2);
		}

		return AddedRange(mesh, vertexOffset, triangleOffset);
	}

	Mesh3DAddResult AppendDodecahedron(
		Mesh3D& mesh,
		const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Dodecahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Dodecahedron(): radius must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(mesh, 60, 36, vertexOffset, triangleOffset))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Dodecahedron(): The generated mesh exceeds the supported size");
		}

		const DodecahedronData& data = GetDodecahedronData();
		TriangleIndex32* pTriangle = (mesh.indices.data() + triangleOffset);

		for (size_t faceIndex = 0; faceIndex < data.faces.size(); ++faceIndex)
		{
			std::array<uint32, 5> face = data.faces[faceIndex];
			Float3 faceCenter = Float3::Zero();
			for (const uint32 vertexIndex : face)
			{
				faceCenter += data.vertices[vertexIndex];
			}
			faceCenter /= static_cast<float>(face.size());

			Float3 normal = (data.vertices[face[1]] - data.vertices[face[0]])
				.cross(data.vertices[face[2]] - data.vertices[face[0]]).normalized();
			if (normal.dot(faceCenter) < 0.0f)
			{
				std::reverse(face.begin(), face.end());
				normal = -normal;
			}

			const size_t vertexBase = (vertexOffset + (faceIndex * face.size()));
			std::array<Float3, 5> positions;
			for (size_t i = 0; i < positions.size(); ++i)
			{
				positions[i] = data.vertices[face[i]];
			}

			const Float3 tangent = (positions[0] - faceCenter).normalized();
			const Float3 bitangent = normal.cross(tangent);
			const float inverseFaceRadius = (1.0f / positions[0].distanceFrom(faceCenter));

			for (size_t i = 0; i < positions.size(); ++i)
			{
				const Float3 vertexOffsetFromCenter = (positions[i] - faceCenter);
				mesh.vertices[vertexBase + i] = Vertex3D{
					.pos = (positions[i] * radius),
					.normal = normal,
					.tex = Float2{
						std::clamp((0.5f + (0.5f * vertexOffsetFromCenter.dot(tangent) * inverseFaceRadius)), 0.0f, 1.0f),
						std::clamp((0.5f + (0.5f * vertexOffsetFromCenter.dot(bitangent) * inverseFaceRadius)), 0.0f, 1.0f)
					},
					.tangent = Float4{ tangent, 1.0f }
				};
			}

			const uint32 i0 = static_cast<uint32>(vertexBase);
			*pTriangle++ = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			*pTriangle++ = TriangleIndex32{ i0, (i0 + 2), (i0 + 3) };
			*pTriangle++ = TriangleIndex32{ i0, (i0 + 3), (i0 + 4) };
		}

		return AddedRange(
			mesh,
			(mesh.vertices.size() - 60),
			(mesh.indices.size() - 36));
	}
}
