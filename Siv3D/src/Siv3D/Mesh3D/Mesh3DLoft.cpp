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
# include <Siv3D/Polygon.hpp>
# include "Mesh3DCommon.hpp"
# include <algorithm>
# include <cmath>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::AddedRange;
		using Mesh3DDetail::OperationFailed;
		using Mesh3DDetail::CapValidationResult;
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::ForEachValidCapTriangle;
		using Mesh3DDetail::IsFloatRepresentable;
		using Mesh3DDetail::ResizeForAddition;
		using Mesh3DDetail::RingValidationResult;
		using Mesh3DDetail::ValidateCapTriangles;
		using Mesh3DDetail::ValidateRing;

		struct LoftFrame
		{
			Vec3 origin;
			Vec3 xAxis;
			Vec3 zAxis;
			Vec3 normal;

			[[nodiscard]]
			bool transformPoint(const Float2 point, Float3& output) const noexcept
			{
				const Vec3 position = (origin + xAxis * point.x - zAxis * point.y);
				if (not IsFloatRepresentable(position))
				{
					return false;
				}
				output = position;
				return true;
			}
		};

		struct LoftCap
		{
			Polygon polygon;
			Array<Float3> positions;
			Float2 minPoint{};
			Float2 maxPoint{};
			LoftFrame frame;
			size_t triangleCount = 0;
		};

		struct PreparedLoft
		{
			size_t sectionCount = 0;
			size_t contourVertexCount = 0;
			size_t vertexCount = 0;
			size_t triangleCount = 0;
			Array<Float3> positions;
			Array<Float3> sideNormals; // Two sides of each contour edge at each section.
			Array<Float3> sideTangents; // Only allocated when contour smoothing is enabled.
			Array<double> sectionDistances;
			Array<double> contourU;
			std::array<LoftCap, 2> caps;
		};

		[[nodiscard]]
		static bool NormalizeDirection(const Vec3 direction, Vec3& result) noexcept
		{
			const double lengthSq = direction.lengthSq();
			if (not (lengthSq > 0.0))
			{
				return false;
			}
			result = (direction / std::sqrt(lengthSq));
			return true;
		}

		[[nodiscard]]
		static Result<void, Mesh3DError> PrepareLoftSections(
			const std::span<const LoftSection> sections,
			const LoftOptions& options,
			PreparedLoft& data)
		{
			if ((sections.size() < 2) || (sections.front().points.size() < 3)
				|| (options.smoothingAngle < 0) || (Math::Pi < options.smoothingAngle)
				|| (static_cast<uint8>(options.endCaps) > static_cast<uint8>(Mesh3DEndCaps::Both)))
			{
				return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Loft(): Invalid section dimensions or options");
			}
			if (not (std::isfinite(options.smoothingAngle)
				&& IsFloatRepresentable(options.uvScale.x) && IsFloatRepresentable(options.uvScale.y)
				&& IsFloatRepresentable(options.uvOffset.x) && IsFloatRepresentable(options.uvOffset.y)
				&& IsFloatRepresentable(options.uvOffset.x + options.uvScale.x)
				&& IsFloatRepresentable(options.uvOffset.y + options.uvScale.y)))
			{
				return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Non-finite options or unrepresentable UV coordinates");
			}
			data.sectionCount = sections.size();
			data.contourVertexCount = sections.front().points.size();
			size_t pointCount;
			size_t quadCount;
			if (not (CheckedMultiply(data.sectionCount, data.contourVertexCount, pointCount)
				&& CheckedMultiply(pointCount, 2, data.vertexCount)
				&& (data.vertexCount <= Mesh3D::MaxVertexCount)
				&& CheckedMultiply(data.sectionCount - 1, data.contourVertexCount, quadCount)
				&& CheckedMultiply(quadCount, 2, data.triangleCount)))
			{
				return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Loft(): Section count exceeds mesh limits");
			}
			data.positions.resize(pointCount);
			data.sideNormals.resize(data.vertexCount);
			data.sectionDistances.resize(data.sectionCount);
			data.contourU.resize(data.contourVertexCount + 1);
			Array<Float2> local(data.contourVertexCount);
			LoftFrame previous;
			for (size_t i = 0; i < data.sectionCount; ++i)
			{
				if (sections[i].points.size() != data.contourVertexCount)
				{
					return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Loft(): Sections must have equal vertex counts");
				}
				const Mat4x4& m = sections[i].frame.getTransform();
				if (not m.isFinite())
				{
					return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Non-finite section frame");
				}
				if (not m.isAffine(0))
				{
					return OperationFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Loft(): Section frame must be affine");
				}
				LoftFrame frame{
					.origin = Vec3{ m.transformPoint(Float3::Zero()) },
					.xAxis = Vec3{ m.transformVector(Float3::UnitX()) },
					.zAxis = Vec3{ m.transformVector(Float3::UnitZ()) }
				};
				const Vec3 y{ m.transformVector(Float3::UnitY()) };
				if (not (frame.xAxis.dot(y.cross(frame.zAxis)) > 0))
				{
					return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Section frame must preserve orientation and be nonsingular");
				}
				(void)NormalizeDirection(frame.zAxis.cross(frame.xAxis), frame.normal);
				if (i > 0)
				{
					const Vec3 delta = (frame.origin - previous.origin);
					if (not ((delta.dot(previous.normal) > 0) && (delta.dot(frame.normal) > 0)))
					{
						return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Section origins must advance along both section normals");
					}
					data.sectionDistances[i] = (data.sectionDistances[i - 1] + delta.length());
				}
				for (size_t j = 0; j < data.contourVertexCount; ++j)
				{
					const Vec2 p = sections[i].points[j];
					if (not IsFloatRepresentable(p))
					{
						return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Unrepresentable contour point");
					}
					local[j] = p;
					if (not frame.transformPoint(local[j], data.positions[i * data.contourVertexCount + j]))
					{
						return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Unrepresentable transformed point");
					}
				}
				double perimeter;
				if (ValidateRing(std::span<const Float2>{ local }, true, perimeter) != RingValidationResult::Valid)
				{
					return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Contour must have positive area and nonzero edges");
				}
				for (size_t j = 0; j < data.contourVertexCount; ++j)
				{
					const auto a = data.positions[i * data.contourVertexCount + j];
					const auto b = data.positions[i * data.contourVertexCount + (j + 1) % data.contourVertexCount];
					if (a == b)
					{
						return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Section edge collapsed after placement");
					}
					if (i == 0)
					{
						data.contourU[j + 1] = (data.contourU[j] + (Vec3{ b } - Vec3{ a }).length());
					}
				}
				const bool start = ((i == 0)
					&& ((options.endCaps == Mesh3DEndCaps::Start) || (options.endCaps == Mesh3DEndCaps::Both)));
				const bool end = ((i + 1 == data.sectionCount)
					&& ((options.endCaps == Mesh3DEndCaps::End) || (options.endCaps == Mesh3DEndCaps::Both)));
				if (start || end)
				{
					LoftCap& cap = data.caps[start ? 0 : 1];
					cap.frame = frame;
					cap.polygon = Polygon{ sections[i].points };
					if (cap.polygon.isEmpty()
						|| (ValidateCapTriangles<false>(cap.polygon.vertices(), cap.polygon.indices(), cap.triangleCount) != CapValidationResult::Valid))
					{
						return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Cannot triangulate requested cap");
					}
					cap.minPoint = cap.maxPoint = cap.polygon.vertices().front();
					cap.positions.resize(cap.polygon.vertices().size());
					for (size_t j = 0; j < cap.positions.size(); ++j)
					{
						const Float2 p = cap.polygon.vertices()[j];
						cap.minPoint.x = std::min(cap.minPoint.x, p.x);
						cap.maxPoint.x = std::max(cap.maxPoint.x, p.x);
						cap.minPoint.y = std::min(cap.minPoint.y, p.y);
						cap.maxPoint.y = std::max(cap.maxPoint.y, p.y);
						if (not frame.transformPoint(p, cap.positions[j]))
						{
							return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Unrepresentable cap point");
						}
					}
					bool valid = true;
					ForEachValidCapTriangle(cap.polygon.vertices(), cap.polygon.indices(), cap.triangleCount, [&](const TriangleIndex& t)
					{
						const Vec3 a{ cap.positions[t.i0] };
						const Vec3 b{ cap.positions[t.i1] };
						const Vec3 c{ cap.positions[t.i2] };
						valid &= ((b - a).cross(c - a).dot(frame.normal) > 0);
					});
					if (not valid)
					{
						return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Cap collapsed after placement");
					}
					if (not (CheckedAdd(data.vertexCount, cap.positions.size(), data.vertexCount)
						&& (data.vertexCount <= Mesh3D::MaxVertexCount)
						&& CheckedAdd(data.triangleCount, cap.triangleCount, data.triangleCount)))
					{
						return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Loft(): Cap count exceeds mesh limits");
					}
				}
				previous = frame;
			}
			const double perimeter = data.contourU.back();
			for (double& u : data.contourU)
			{
				u /= perimeter;
			}
			if (not IsFloatRepresentable(options.uvOffset.y + options.uvScale.y * data.sectionDistances.back()))
			{
				return OperationFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Side V exceeds float range");
			}

			return{};
		}

		[[nodiscard]]
		static Result<void, Mesh3DError> PrepareLoftSideFrames(
			const LoftOptions& options,
			PreparedLoft& data)
		{
			// Each edge owns two vertices at each section, including smoothed corners.
			for (size_t edgeIndex = 0; edgeIndex < data.contourVertexCount; ++edgeIndex)
			{
				const size_t next = (edgeIndex + 1) % data.contourVertexCount;
				for (size_t sectionIndex = 0; sectionIndex < data.sectionCount; ++sectionIndex)
				{
					const size_t before = (sectionIndex == 0 ? sectionIndex : sectionIndex - 1);
					const size_t after = std::min(sectionIndex + 1, data.sectionCount - 1);
					const Vec3 edge = Vec3{ data.positions[sectionIndex * data.contourVertexCount + next] } - Vec3{ data.positions[sectionIndex * data.contourVertexCount + edgeIndex] };
					for (size_t side = 0; side < 2; ++side)
					{
						const size_t j = side ? next : edgeIndex;
						const Vec3 direction = Vec3{ data.positions[after * data.contourVertexCount + j] } - Vec3{ data.positions[before * data.contourVertexCount + j] };
						Vec3 normal;
						if (not NormalizeDirection(edge.cross(direction), normal))
						{
							return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Undefined side normal");
						}
						data.sideNormals[(edgeIndex * data.sectionCount + sectionIndex) * 2 + side] = normal;
					}
				}
			}
			if (options.smoothingAngle > 0)
			{
				const double threshold = std::cos(options.smoothingAngle) - 1e-6;
				for (size_t edgeIndex = 0; edgeIndex < data.contourVertexCount; ++edgeIndex)
				{
					for (size_t sectionIndex = 0; sectionIndex < data.sectionCount; ++sectionIndex)
					{
						auto& a = data.sideNormals[(((edgeIndex + data.contourVertexCount - 1) % data.contourVertexCount) * data.sectionCount + sectionIndex) * 2 + 1];
						auto& b = data.sideNormals[(edgeIndex * data.sectionCount + sectionIndex) * 2];
						if (a.dot(b) >= threshold)
						{
							Vec3 average;
							if (not NormalizeDirection(Vec3{ a } + Vec3{ b }, average))
							{
								return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Opposing smoothing normals");
							}
							a = b = average;
						}
					}
				}
			}
			if (options.smoothingAngle > 0)
			{
				data.sideTangents.resize(data.sideNormals.size());
				for (size_t edgeIndex = 0; edgeIndex < data.contourVertexCount; ++edgeIndex)
				{
					for (size_t sectionIndex = 0; sectionIndex < data.sectionCount; ++sectionIndex)
					{
						const Vec3 edge = Vec3{ data.positions[sectionIndex * data.contourVertexCount + (edgeIndex + 1) % data.contourVertexCount] } - Vec3{ data.positions[sectionIndex * data.contourVertexCount + edgeIndex] };
						for (size_t side = 0; side < 2; ++side)
						{
							const size_t normalIndex = (edgeIndex * data.sectionCount + sectionIndex) * 2 + side;
							const Vec3 normal{ data.sideNormals[normalIndex] };
							Vec3 tangent;
							if (not NormalizeDirection(edge - normal * (edge.dot(normal) / normal.lengthSq()), tangent))
							{
								return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Undefined smoothed tangent");
							}
							data.sideTangents[normalIndex] = tangent;
						}
					}
				}
			}
			return{};
		}

		[[nodiscard]]
		static Result<void, Mesh3DError> ValidateLoftSideTriangles(const PreparedLoft& data)
		{
			for (size_t edgeIndex = 0; edgeIndex < data.contourVertexCount; ++edgeIndex)
			{
				for (size_t sectionIndex = 0; sectionIndex + 1 < data.sectionCount; ++sectionIndex)
				{
					const size_t j = (edgeIndex + 1) % data.contourVertexCount;
					const Vec3 a{ data.positions[sectionIndex * data.contourVertexCount + edgeIndex] };
					const Vec3 b{ data.positions[sectionIndex * data.contourVertexCount + j] };
					const Vec3 c{ data.positions[(sectionIndex + 1) * data.contourVertexCount + edgeIndex] };
					const Vec3 d{ data.positions[(sectionIndex + 1) * data.contourVertexCount + j] };
					const size_t normalIndex = (edgeIndex * data.sectionCount + sectionIndex) * 2;
					const Vec3 n0 = Vec3{ data.sideNormals[normalIndex] } + Vec3{ data.sideNormals[normalIndex + 1] } + Vec3{ data.sideNormals[normalIndex + 2] };
					const Vec3 n1 = Vec3{ data.sideNormals[normalIndex + 2] } + Vec3{ data.sideNormals[normalIndex + 1] } + Vec3{ data.sideNormals[normalIndex + 3] };
					if (not (((b - a).cross(c - a).dot(n0) > 0) && ((b - c).cross(d - c).dot(n1) > 0)))
					{
						return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Collapsed or locally folded side triangle");
					}
				}
			}
			return{};
		}

		static void WriteLoft(
			const PreparedLoft& data,
			const LoftOptions& options,
			Mesh3D& mesh,
			const size_t vertexBase,
			const size_t triangleBase)
		{
			const Mesh3DDetail::UVTangentTransform tangentTransform{ options.uvScale };
			const auto transformUV = [&](const double u, const double v)
			{
				return Float2{
					static_cast<float>(options.uvOffset.x + options.uvScale.x * u),
					static_cast<float>(options.uvOffset.y + options.uvScale.y * v)
				};
			};
			size_t base = vertexBase;
			auto* triangle = mesh.indices.data() + triangleBase;
			for (size_t k = 0; k < 2; ++k)
			{
				const auto& cap = data.caps[k];
				if (cap.positions.isEmpty())
				{
					continue;
				}
				const Float3 normal = cap.frame.normal * (k == 0 ? -1 : 1);
				const Float4 tangent = tangentTransform.apply(Float4{ Float3{ cap.frame.xAxis.normalized() }, 1.0f });
				for (size_t j = 0; j < cap.positions.size(); ++j)
				{
					const Float2 p = cap.polygon.vertices()[j];
					const double u = (static_cast<double>(p.x) - cap.minPoint.x) / (static_cast<double>(cap.maxPoint.x) - cap.minPoint.x);
					const double v = (static_cast<double>(p.y) - cap.minPoint.y) / (static_cast<double>(cap.maxPoint.y) - cap.minPoint.y);
					mesh.vertices[base + j] = Vertex3D{
						.pos = cap.positions[j],
						.normal = normal,
						.tex = transformUV(u, k == 0 ? 1 - v : v),
						.tangent = tangent
					};
				}
				ForEachValidCapTriangle(cap.polygon.vertices(), cap.polygon.indices(), cap.triangleCount, [&](const TriangleIndex& t)
				{
					*triangle++ = TriangleIndex32{
						static_cast<uint32>(base + t.i0),
						static_cast<uint32>(base + (k == 0 ? t.i2 : t.i1)),
						static_cast<uint32>(base + (k == 0 ? t.i1 : t.i2))
					};
				});
				base += cap.positions.size();
			}
			for (size_t edgeIndex = 0; edgeIndex < data.contourVertexCount; ++edgeIndex)
			{
				const size_t next = (edgeIndex + 1) % data.contourVertexCount;
				for (size_t sectionIndex = 0; sectionIndex < data.sectionCount; ++sectionIndex)
				{
					const Float3 edge = (options.smoothingAngle == 0
						? Float3{ (Vec3{ data.positions[sectionIndex * data.contourVertexCount + next] } - Vec3{ data.positions[sectionIndex * data.contourVertexCount + edgeIndex] }).normalized() }
						: Float3::Zero());
					for (size_t side = 0; side < 2; ++side)
					{
						const size_t normalIndex = (edgeIndex * data.sectionCount + sectionIndex) * 2 + side;
						const Float3 tangent = (options.smoothingAngle == 0 ? edge : data.sideTangents[normalIndex]);
						mesh.vertices[base + normalIndex] = Vertex3D{
							.pos = data.positions[sectionIndex * data.contourVertexCount + (side ? next : edgeIndex)],
							.normal = data.sideNormals[normalIndex],
							.tex = transformUV(data.contourU[edgeIndex + side], data.sectionDistances[sectionIndex]),
							.tangent = tangentTransform.apply(Float4{ tangent, 1.0f })
						};
					}
					if (sectionIndex + 1 < data.sectionCount)
					{
						const uint32 v = static_cast<uint32>(base + (edgeIndex * data.sectionCount + sectionIndex) * 2);
						*triangle++ = TriangleIndex32{ v, v + 1, v + 2 };
						*triangle++ = TriangleIndex32{ v + 2, v + 1, v + 3 };
					}
				}
			}
		}
	}

	Mesh3DAddResult Mesh3DDetail::AppendLoft(
		Mesh3D& mesh,
		const std::span<const LoftSection> sections,
		const LoftOptions& options)
	{
		PreparedLoft data;
		if (auto result = PrepareLoftSections(sections, options, data); not result)
		{
			return Err{ std::move(result.error()) };
		}
		if (auto result = PrepareLoftSideFrames(options, data); not result)
		{
			return Err{ std::move(result.error()) };
		}
		if (auto result = ValidateLoftSideTriangles(data); not result)
		{
			return Err{ std::move(result.error()) };
		}

		size_t vertexBase;
		size_t triangleBase;
		if (not ResizeForAddition(mesh, data.vertexCount, data.triangleCount, vertexBase, triangleBase))
		{
			return OperationFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Loft(): Output exceeds mesh limits");
		}
		WriteLoft(data, options, mesh, vertexBase, triangleBase);
		return AddedRange(mesh, vertexBase, triangleBase);
	}

	Mesh3D Mesh3D::Loft(const std::span<const LoftSection> sections, const LoftOptions& options)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendLoft(mesh, sections, options);
		return mesh;
	}

	Mesh3D Mesh3D::Loft(const std::initializer_list<LoftSection> sections, const LoftOptions& options)
	{
		return Loft(std::span<const LoftSection>{ sections.begin(), sections.size() }, options);
	}
}
