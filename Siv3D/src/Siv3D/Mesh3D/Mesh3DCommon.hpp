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

# pragma once
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/MathConstants.hpp>
# include <Siv3D/Mat4x4.hpp>
# include <Siv3D/Mesh3D.hpp>
# include <Siv3D/Polygon.hpp>
# include <cmath>
# include <limits>
# include <span>
# include <type_traits>

namespace s3d::Mesh3DDetail
{
	template <class Result = Mesh3D>
	[[nodiscard]]
	inline Result GenerationFailed(const char* const message)
	{
		LOG_FAIL(message);
		return{};
	}

	[[nodiscard]]
	inline Mesh3DAddResult AdditionFailed(
		const Mesh3DErrorCode code,
		const StringView message)
	{
		LOG_FAIL(message);
		return Err{ Mesh3DError{ code, String{ message } } };
	}

	[[nodiscard]]
	inline Mesh3DRange AddedRange(
		const Mesh3D& mesh,
		const size_t vertexOffset,
		const size_t triangleOffset) noexcept
	{
		return Mesh3DRange{
			.vertexOffset = vertexOffset,
			.vertexCount = (mesh.vertices.size() - vertexOffset),
			.triangleOffset = triangleOffset,
			.triangleCount = (mesh.indices.size() - triangleOffset),
		};
	}

	inline void TransformVertexRange(
		const std::span<Vertex3D> vertexRange,
		const Mat4x4& matrix) noexcept
	{
		if (vertexRange.empty())
		{
			return;
		}

		DirectX::XMVECTOR determinantVector;
		const DirectX::XMMATRIX inverseMatrix = DirectX::XMMatrixInverse(&determinantVector, matrix.value);
		const float determinant = DirectX::XMVectorGetX(determinantVector);

		if (determinant == 0.0f)
		{
			for (auto& vertex : vertexRange)
			{
				const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
					static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
				DirectX::XMStoreFloat3(
					static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
					DirectX::XMVector3Transform(position, matrix.value));
			}

			return;
		}

		const DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(inverseMatrix);
		const bool flipHandedness = (determinant < 0.0f);

		for (auto& vertex : vertexRange)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
				DirectX::XMVector3Transform(position, matrix.value));

			const DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.normal)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.normal)),
				DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(normal, normalMatrix)));

			const DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(
				DirectX::XMVector3TransformNormal(
					DirectX::XMLoadFloat4(
						static_cast<const DirectX::XMFLOAT4*>(static_cast<const void*>(&vertex.tangent))),
					matrix.value));
			const float tangentW = (flipHandedness ? -vertex.tangent.w : vertex.tangent.w);
			DirectX::XMStoreFloat4(
				static_cast<DirectX::XMFLOAT4*>(static_cast<void*>(&vertex.tangent)),
				DirectX::XMVectorSetW(tangent, tangentW));
		}
	}

	[[nodiscard]]
	inline bool IsFloatRepresentable(const double value) noexcept
	{
		constexpr double MaxFloat = std::numeric_limits<float>::max();
		return (std::isfinite(value)
			&& (-MaxFloat <= value)
			&& (value <= MaxFloat));
	}

	[[nodiscard]]
	inline bool IsFloatRepresentable(const Vec2 value) noexcept
	{
		return (IsFloatRepresentable(value.x)
			&& IsFloatRepresentable(value.y));
	}

	[[nodiscard]]
	inline bool IsFloatRepresentable(const Vec3 value) noexcept
	{
		return (IsFloatRepresentable(value.x)
			&& IsFloatRepresentable(value.y)
			&& IsFloatRepresentable(value.z));
	}

	[[nodiscard]]
	inline bool CheckedAdd(const size_t a, const size_t b, size_t& result) noexcept
	{
		if ((std::numeric_limits<size_t>::max() - a) < b)
		{
			return false;
		}

		result = (a + b);
		return true;
	}

	[[nodiscard]]
	inline bool CheckedMultiply(const size_t a, const size_t b, size_t& result) noexcept
	{
		if ((a != 0)
			&& ((std::numeric_limits<size_t>::max() / a) < b))
		{
			return false;
		}

		result = (a * b);
		return true;
	}

	template <class Float>
	struct CircleSample
	{
		Float sin;
		Float cos;
	};

	template <class Float>
	[[nodiscard]]
	Array<CircleSample<Float>> MakeCircleSamples(const uint32 segments)
	{
		Array<CircleSample<Float>> samples(static_cast<size_t>(segments) + 1);
		Float angleStep;
		if constexpr (std::is_same_v<Float, float>)
		{
			angleStep = (Math::TwoPiF / static_cast<float>(segments));
		}
		else
		{
			angleStep = (Math::TwoPi / static_cast<double>(segments));
		}

		for (uint32 i = 0; i < segments; ++i)
		{
			const Float angle = (angleStep * static_cast<Float>(i));
			samples[i] = CircleSample<Float>{ std::sin(angle), std::cos(angle) };
		}

		samples[segments] = samples[0];
		return samples;
	}

	enum class RingValidationResult
	{
		Valid,
		TooFewVertices,
		InvalidVertex,
		InvalidEdge,
		InvalidArea,
	};

	enum class CapValidationResult
	{
		Valid,
		InvalidGeometry,
		NumericRange,
	};

	template <class Point>
	[[nodiscard]]
	RingValidationResult ValidateRing(
		const std::span<const Point> ring,
		const bool expectPositiveArea,
		double& perimeter) noexcept
	{
		if (ring.size() < 3)
		{
			return RingValidationResult::TooFewVertices;
		}

		double twiceArea = 0.0;
		perimeter = 0.0;

		for (size_t i = 0; i < ring.size(); ++i)
		{
			const Point current = ring[i];
			const Point next = ring[(i + 1) % ring.size()];

			if constexpr (std::is_same_v<Point, Vec2>)
			{
				if ((not IsFloatRepresentable(current.x))
					|| (not IsFloatRepresentable(current.y))
					|| (not IsFloatRepresentable(next.x))
					|| (not IsFloatRepresentable(next.y)))
				{
					return RingValidationResult::InvalidVertex;
				}
			}

			const Float2 currentF = current;
			const Float2 nextF = next;
			const double dx = (static_cast<double>(nextF.x) - currentF.x);
			const double dy = (static_cast<double>(nextF.y) - currentF.y);
			const double edgeLength = std::hypot(dx, dy);
			if ((not std::isfinite(edgeLength))
				|| (edgeLength == 0.0))
			{
				return RingValidationResult::InvalidEdge;
			}

			perimeter += edgeLength;
			twiceArea += ((static_cast<double>(currentF.x) * nextF.y)
				- (static_cast<double>(nextF.x) * currentF.y));
		}

		if ((not std::isfinite(perimeter))
			|| (not std::isfinite(twiceArea))
			|| (expectPositiveArea ? (twiceArea <= 0.0) : (0.0 <= twiceArea)))
		{
			return RingValidationResult::InvalidArea;
		}

		return RingValidationResult::Valid;
	}

	template <bool CheckFiniteVertices>
	[[nodiscard]]
	CapValidationResult ValidateCapTriangles(
		const std::span<const Float2> vertices,
		const std::span<const TriangleIndex> indices,
		size_t& validTriangleCount) noexcept
	{
		validTriangleCount = 0;

		if ((vertices.size() < 3)
			|| indices.empty())
		{
			return CapValidationResult::InvalidGeometry;
		}

		if constexpr (CheckFiniteVertices)
		{
			for (const Float2 vertex : vertices)
			{
				if ((not std::isfinite(vertex.x))
					|| (not std::isfinite(vertex.y)))
				{
					return CapValidationResult::NumericRange;
				}
			}
		}

		for (const TriangleIndex& index : indices)
		{
			if ((vertices.size() <= index.i0)
				|| (vertices.size() <= index.i1)
				|| (vertices.size() <= index.i2))
			{
				return CapValidationResult::InvalidGeometry;
			}

			const Float2 p0 = vertices[index.i0];
			const Float2 p1 = vertices[index.i1];
			const Float2 p2 = vertices[index.i2];
			const double twiceArea = (
				((static_cast<double>(p1.x) - p0.x) * (static_cast<double>(p2.y) - p0.y))
				- ((static_cast<double>(p1.y) - p0.y) * (static_cast<double>(p2.x) - p0.x)));
			if ((not std::isfinite(twiceArea))
				|| (twiceArea < 0.0))
			{
				return CapValidationResult::InvalidGeometry;
			}

			validTriangleCount += (0.0 < twiceArea);
		}

		return (0 < validTriangleCount
			? CapValidationResult::Valid
			: CapValidationResult::InvalidGeometry);
	}

	template <class Callback>
	void ForEachValidCapTriangle(
		const std::span<const Float2> vertices,
		const std::span<const TriangleIndex> indices,
		const size_t validTriangleCount,
		Callback&& callback)
	{
		if (validTriangleCount == indices.size())
		{
			for (const TriangleIndex& index : indices)
			{
				callback(index);
			}

			return;
		}

		for (const TriangleIndex& index : indices)
		{
			const Float2 p0 = vertices[index.i0];
			const Float2 p1 = vertices[index.i1];
			const Float2 p2 = vertices[index.i2];
			const double twiceArea = (
				((static_cast<double>(p1.x) - p0.x) * (static_cast<double>(p2.y) - p0.y))
				- ((static_cast<double>(p1.y) - p0.y) * (static_cast<double>(p2.x) - p0.x)));
			if (0.0 < twiceArea)
			{
				callback(index);
			}
		}
	}

	[[nodiscard]]
	Mesh3DAddResult AppendExtrude(
		Mesh3D& mesh,
		const Polygon& polygon,
		double height,
		double smoothingAngle);

	[[nodiscard]]
	Mesh3DAddResult AppendRevolve(
		Mesh3D& mesh,
		std::span<const Vec2> profile,
		uint32 segments,
		double smoothingAngle);

	[[nodiscard]]
	Mesh3DAddResult AppendRevolve(
		Mesh3D& mesh,
		std::span<const Vec2> profile,
		double startAngle,
		double sweepAngle,
		uint32 segments,
		double smoothingAngle,
		CloseEnds closeEnds);

	[[nodiscard]]
	Mesh3DAddResult AppendTube(
		Mesh3D& mesh,
		std::span<const Vec3> path,
		double radius,
		uint32 sides,
		Vec2 uvScale,
		Vec2 uvOffset,
		CloseRing closeRing);

	[[nodiscard]]
	Mesh3DAddResult AppendTube(
		Mesh3D& mesh,
		std::span<const Vec3> path,
		std::span<const double> radii,
		uint32 sides,
		Vec2 uvScale,
		Vec2 uvOffset,
		CloseRing closeRing);

	[[nodiscard]]
	Mesh3DAddResult AppendSweep(
		Mesh3D& mesh,
		const Polygon& crossSection,
		std::span<const Vec3> path,
		const Vec3* initialXAxis,
		Vec2 uvScale,
		Vec2 uvOffset,
		CloseRing closeRing);

	[[nodiscard]]
	Mesh3DAddResult AppendHeightField(
		Mesh3D& mesh,
		const Grid<float>& heights,
		SizeF sizeXZ,
		Vec2 uvScale,
		Vec2 uvOffset);

	[[nodiscard]]
	Mesh3DAddResult AppendLoft(
		Mesh3D& mesh,
		std::span<const std::span<const Vec2>> sections,
		std::span<const double> heights,
		Vec2 uvScale,
		Vec2 uvOffset);
}
