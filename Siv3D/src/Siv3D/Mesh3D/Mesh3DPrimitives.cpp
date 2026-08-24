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
# include <Siv3D/MathConstants.hpp>
# include <array>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsFinite(const Float2 value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y));
		}

		[[nodiscard]]
		static bool IsFinite(const Float3 value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y)
				&& std::isfinite(value.z));
		}

		[[nodiscard]]
		static bool CheckedAdd(const size_t a, const size_t b, size_t& result) noexcept
		{
			if ((std::numeric_limits<size_t>::max() - a) < b)
			{
				return false;
			}

			result = (a + b);
			return true;
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

		struct BoxFace
		{
			Float3 center;
			Float3 u;
			Float3 v;
			Float3 normal;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Box
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Box(const Float3 size)
	{
		if ((not IsFinite(size))
			|| (size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return{};
		}

		const Float3 halfSize = (size * 0.5f);
		const std::array<BoxFace, 6> faces =
		{{
			{ { 0.0f, 0.0f, -halfSize.z }, { size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ { 0.0f, 0.0f,  halfSize.z }, { -size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -size.y, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, -size.y, 0.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { 0.0f,  halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -1.0f, 0.0f } },
		}};

		Mesh3D mesh{ 24, 12 };

		for (size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
		{
			const BoxFace& face = faces[faceIndex];
			const Float3 halfU = (face.u * 0.5f);
			const Float3 halfV = (face.v * 0.5f);
			const Float3 tangent = face.u.normalized();
			const Float4 tangentFrame{ tangent, 1.0f };
			const size_t vertexOffset = (faceIndex * 4);

			mesh.vertices[vertexOffset + 0] = Vertex3D{
				.pos = (face.center - halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ 0.0f, 0.0f },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 1] = Vertex3D{
				.pos = (face.center + halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ 1.0f, 0.0f },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 2] = Vertex3D{
				.pos = (face.center - halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ 0.0f, 1.0f },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 3] = Vertex3D{
				.pos = (face.center + halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ 1.0f, 1.0f },
				.tangent = tangentFrame
			};

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			const size_t triangleOffset = (faceIndex * 2);
			mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Grid
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Grid(
		const Float2 sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Float2 uvScale,
		const Float2 uvOffset)
	{
		if ((not IsFinite(sizeXZ))
			|| (not IsFinite(uvScale))
			|| (not IsFinite(uvOffset))
			|| (sizeXZ.x <= 0.0f)
			|| (sizeXZ.y <= 0.0f)
			|| (segmentsX == 0)
			|| (segmentsZ == 0))
		{
			return{};
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
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Float2 halfSize = (sizeXZ * 0.5f);
		const float invSegmentsX = (1.0f / static_cast<float>(segmentsX));
		const float invSegmentsZ = (1.0f / static_cast<float>(segmentsZ));
		const Float3 normal = Float3::UnitY();
		const Float4 tangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		for (uint32 z = 0; z <= segmentsZ; ++z)
		{
			const float v = (z * invSegmentsZ);
			const float positionZ = (halfSize.y - (sizeXZ.y * v));

			for (uint32 x = 0; x <= segmentsX; ++x)
			{
				const float u = (x * invSegmentsX);
				const size_t vertexIndex = (static_cast<size_t>(z) * columnCount + x);

				mesh.vertices[vertexIndex] = Vertex3D{
					.pos = Float3{ (-halfSize.x + (sizeXZ.x * u)), 0.0f, positionZ },
					.normal = normal,
					.tex = Float2{ (uvOffset.x + (uvScale.x * u)), (uvOffset.y + (uvScale.y * v)) },
					.tangent = tangent
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (uint32 z = 0; z < segmentsZ; ++z)
		{
			const size_t rowOffset = (static_cast<size_t>(z) * columnCount);

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

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	UVSphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::UVSphere(const float radius, const uint32 slices, const uint32 stacks)
	{
		if ((not std::isfinite(radius))
			|| (radius <= 0.0f)
			|| (slices < 3)
			|| (stacks < 2))
		{
			return{};
		}

		const size_t interiorRingCount = (static_cast<size_t>(stacks) - 1);
		size_t ringStride;
		size_t ringVertexCount;
		size_t poleVertexCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(interiorRingCount, ringStride, ringVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(slices), 2, poleVertexCount))
			|| (not CheckedAdd(ringVertexCount, poleVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(slices), interiorRingCount, triangleCount))
			|| (not CheckedMultiply(triangleCount, 2, triangleCount)))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const size_t firstRingBase = slices;
		const size_t bottomPoleBase = (firstRingBase + ringVertexCount);
		const float longitudeStep = (Math::TwoPiF / static_cast<float>(slices));
		const float latitudeStep = (Math::PiF / static_cast<float>(stacks));
		Array<Float2> longitudeSinCos(ringStride);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float longitude = (longitudeStep * x);
			longitudeSinCos[x] = Float2{ std::sin(longitude), std::cos(longitude) };

			const float middleLongitude = (longitude + (longitudeStep * 0.5f));
			const float middleSin = std::sin(middleLongitude);
			const float middleCos = std::cos(middleLongitude);
			const float u = ((x + 0.5f) / static_cast<float>(slices));
			const Float4 tangent{ -middleSin, 0.0f, middleCos, 1.0f };

			mesh.vertices[x] = Vertex3D{
				.pos = Float3{ 0.0f, radius, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ u, 0.0f },
				.tangent = tangent
			};
			mesh.vertices[bottomPoleBase + x] = Vertex3D{
				.pos = Float3{ 0.0f, -radius, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ u, 1.0f },
				.tangent = tangent
			};
		}

		longitudeSinCos[slices] = longitudeSinCos[0];

		for (uint32 y = 1; y < stacks; ++y)
		{
			const float v = (static_cast<float>(y) / static_cast<float>(stacks));
			const float latitude = (latitudeStep * y);
			const float latitudeSin = std::sin(latitude);
			const float latitudeCos = std::cos(latitude);
			const size_t ringBase = (firstRingBase + (static_cast<size_t>(y - 1) * ringStride));

			for (uint32 x = 0; x <= slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].x;
				const float longitudeCos = longitudeSinCos[x].y;
				const Float3 normal{
					(latitudeSin * longitudeCos),
					latitudeCos,
					(latitudeSin * longitudeSin)
				};

				mesh.vertices[ringBase + x] = Vertex3D{
					.pos = (normal * radius),
					.normal = normal,
					.tex = Float2{ (static_cast<float>(x) / static_cast<float>(slices)), v },
					.tangent = Float4{ -longitudeSin, 0.0f, longitudeCos, 1.0f }
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();

		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 pole = x;
			const uint32 ringLeft = static_cast<uint32>(firstRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			*pTriangle++ = TriangleIndex32{ pole, ringRight, ringLeft };
		}

		for (size_t ring = 0; (ring + 1) < interiorRingCount; ++ring)
		{
			const size_t upperRingBase = (firstRingBase + (ring * ringStride));
			const size_t lowerRingBase = (upperRingBase + ringStride);

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 i0 = static_cast<uint32>(upperRingBase + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(lowerRingBase + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		const size_t lastRingBase = (firstRingBase + ((interiorRingCount - 1) * ringStride));
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 ringLeft = static_cast<uint32>(lastRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			const uint32 pole = static_cast<uint32>(bottomPoleBase + x);
			*pTriangle++ = TriangleIndex32{ ringLeft, ringRight, pole };
		}

		return mesh;
	}
}
