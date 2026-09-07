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
		using namespace Mesh3DDetail;

		Err<Mesh3DError> Fail(const Mesh3DErrorCode code, const StringView message)
		{
			LOG_FAIL(message);
			return Err{ Mesh3DError{ code, String{ message } } };
		}

		struct Frame
		{
			Vec3 origin, x, z, normal;

			bool position(const Float2 p, Float3& output) const noexcept
			{
				const Vec3 v = (origin + x * p.x - z * p.y);
				if (not (IsFloatRepresentable(v.x) && IsFloatRepresentable(v.y) && IsFloatRepresentable(v.z)))
				{
					return false;
				}
				output = v;
				return true;
			}
		};

		struct Cap
		{
			Polygon polygon;
			Array<Float3> points;
			Float2 min{}, max{};
			Frame frame;
			size_t triangles = 0;
		};

		struct PreparedLoft
		{
			size_t rings = 0, width = 0, vertices = 0, triangles = 0;
			Array<Float3> points;
			Array<Float3> normals; // Two sides of each contour edge at each section.
			Array<Float3> tangents; // Only allocated when contour smoothing is enabled.
			Array<double> distances, u;
			std::array<Cap, 2> caps;
		};

		bool Unit(const Vec3 v, Vec3& result) noexcept
		{
			const double lengthSq = v.lengthSq();
			if (not (lengthSq > 0.0))
			{
				return false;
			}
			result = (v / std::sqrt(lengthSq));
			return true;
		}

		Result<void, Mesh3DError> Prepare(const std::span<const LoftSection> sections,
			const LoftOptions& options, PreparedLoft& data)
		{
			if ((sections.size() < 2) || (sections.front().points.size() < 3)
				|| (options.smoothingAngle < 0) || (Math::Pi < options.smoothingAngle)
				|| (static_cast<uint8>(options.endCaps) > static_cast<uint8>(Mesh3DEndCaps::Both)))
			{
				return Fail(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Loft(): Invalid section dimensions or options");
			}
			if (not (std::isfinite(options.smoothingAngle)
				&& IsFloatRepresentable(options.uvScale.x) && IsFloatRepresentable(options.uvScale.y)
				&& IsFloatRepresentable(options.uvOffset.x) && IsFloatRepresentable(options.uvOffset.y)
				&& IsFloatRepresentable(options.uvOffset.x + options.uvScale.x)
				&& IsFloatRepresentable(options.uvOffset.y + options.uvScale.y)))
			{
				return Fail(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Non-finite options or unrepresentable UV coordinates");
			}
			data.rings = sections.size();
			data.width = sections.front().points.size();
			size_t pointCount, quadCount;
			if (not (CheckedMultiply(data.rings, data.width, pointCount)
				&& CheckedMultiply(pointCount, 2, data.vertices) && (data.vertices <= Mesh3D::MaxVertexCount)
				&& CheckedMultiply(data.rings - 1, data.width, quadCount)
				&& CheckedMultiply(quadCount, 2, data.triangles)))
			{
				return Fail(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Loft(): Section count exceeds mesh limits");
			}
			data.points.resize(pointCount);
			data.normals.resize(data.vertices);
			data.distances.resize(data.rings);
			data.u.resize(data.width + 1);
			Array<Float2> local(data.width);
			Frame previous;
			for (size_t i = 0; i < data.rings; ++i)
			{
				if (sections[i].points.size() != data.width)
				{
					return Fail(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Loft(): Sections must have equal vertex counts");
				}
				const Mat4x4& m = sections[i].frame.getTransform();
				if (not m.isFinite())
				{
					return Fail(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Non-finite section frame");
				}
				if (not m.isAffine(0))
				{
					return Fail(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Loft(): Section frame must be affine");
				}
				Frame frame{ .origin = Vec3{ m.transformPoint(Float3::Zero()) },
					.x = Vec3{ m.transformVector(Float3::UnitX()) }, .z = Vec3{ m.transformVector(Float3::UnitZ()) } };
				const Vec3 y{ m.transformVector(Float3::UnitY()) };
				if (not (frame.x.dot(y.cross(frame.z)) > 0))
				{
					return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Section frame must preserve orientation and be nonsingular");
				}
				(void)Unit(frame.z.cross(frame.x), frame.normal);
				if (i > 0)
				{
					const Vec3 delta = (frame.origin - previous.origin);
					if (not ((delta.dot(previous.normal) > 0) && (delta.dot(frame.normal) > 0)))
					{
						return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Section origins must advance along both section normals");
					}
					data.distances[i] = (data.distances[i - 1] + delta.length());
				}
				for (size_t j = 0; j < data.width; ++j)
				{
					const Vec2 p = sections[i].points[j];
					if (not (IsFloatRepresentable(p.x) && IsFloatRepresentable(p.y)))
					{
						return Fail(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Unrepresentable contour point");
					}
					local[j] = p;
					if (not frame.position(local[j], data.points[i * data.width + j]))
					{
						return Fail(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Unrepresentable transformed point");
					}
				}
				double perimeter;
				if (ValidateRing(std::span<const Float2>{ local }, true, perimeter) != RingValidationResult::Valid)
				{
					return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Contour must have positive area and nonzero edges");
				}
				for (size_t j = 0; j < data.width; ++j)
				{
					const auto a = data.points[i * data.width + j];
					const auto b = data.points[i * data.width + (j + 1) % data.width];
					if (a == b)
					{
						return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Section edge collapsed after placement");
					}
					if (i == 0)
					{
						data.u[j + 1] = (data.u[j] + (Vec3{ b } - Vec3{ a }).length());
					}
				}
				const bool start = (i == 0 && (options.endCaps == Mesh3DEndCaps::Start || options.endCaps == Mesh3DEndCaps::Both));
				const bool end = (i + 1 == data.rings && (options.endCaps == Mesh3DEndCaps::End || options.endCaps == Mesh3DEndCaps::Both));
				if (start || end)
				{
					Cap& cap = data.caps[start ? 0 : 1];
					cap.frame = frame;
					cap.polygon = Polygon{ sections[i].points };
					if (cap.polygon.isEmpty() || ValidateCapTriangles<false>(cap.polygon.vertices(), cap.polygon.indices(), cap.triangles) != CapValidationResult::Valid)
					{
						return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Cannot triangulate requested cap");
					}
					cap.min = cap.max = cap.polygon.vertices().front();
					cap.points.resize(cap.polygon.vertices().size());
					for (size_t j = 0; j < cap.points.size(); ++j)
					{
						const Float2 p = cap.polygon.vertices()[j];
						cap.min.x = std::min(cap.min.x, p.x); cap.max.x = std::max(cap.max.x, p.x);
						cap.min.y = std::min(cap.min.y, p.y); cap.max.y = std::max(cap.max.y, p.y);
						if (not frame.position(p, cap.points[j]))
						{
							return Fail(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Unrepresentable cap point");
						}
					}
					bool valid = true;
					ForEachValidCapTriangle(cap.polygon.vertices(), cap.polygon.indices(), cap.triangles, [&](const TriangleIndex& t)
					{
						const Vec3 a{ cap.points[t.i0] }, b{ cap.points[t.i1] }, c{ cap.points[t.i2] };
						valid &= ((b - a).cross(c - a).dot(frame.normal) > 0);
					});
					if (not valid)
					{
						return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Cap collapsed after placement");
					}
					if (not (CheckedAdd(data.vertices, cap.points.size(), data.vertices) && data.vertices <= Mesh3D::MaxVertexCount
						&& CheckedAdd(data.triangles, cap.triangles, data.triangles)))
					{
						return Fail(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Loft(): Cap count exceeds mesh limits");
					}
				}
				previous = frame;
			}
			const double perimeter = data.u.back();
			for (double& u : data.u) { u /= perimeter; }
			if (not IsFloatRepresentable(options.uvOffset.y + options.uvScale.y * data.distances.back()))
			{
				return Fail(Mesh3DErrorCode::NumericRange, U"Mesh3D::Loft(): Side V exceeds float range");
			}

			// Preserve the existing edge-major layout, even for smoothed contours.
			for (size_t e = 0; e < data.width; ++e)
			{
				const size_t next = (e + 1) % data.width;
				for (size_t i = 0; i < data.rings; ++i)
				{
					const size_t before = (i == 0 ? i : i - 1), after = std::min(i + 1, data.rings - 1);
					const Vec3 edge = Vec3{ data.points[i * data.width + next] } - Vec3{ data.points[i * data.width + e] };
					for (size_t side = 0; side < 2; ++side)
					{
						const size_t j = side ? next : e;
						const Vec3 direction = Vec3{ data.points[after * data.width + j] } - Vec3{ data.points[before * data.width + j] };
						Vec3 normal;
						if (not Unit(edge.cross(direction), normal))
						{
							return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Undefined side normal");
						}
						data.normals[(e * data.rings + i) * 2 + side] = normal;
					}
				}
			}
			if (options.smoothingAngle > 0)
			{
				const double threshold = std::cos(options.smoothingAngle) - 1e-6;
				for (size_t e = 0; e < data.width; ++e)
				{
					for (size_t i = 0; i < data.rings; ++i)
					{
						auto& a = data.normals[(((e + data.width - 1) % data.width) * data.rings + i) * 2 + 1];
						auto& b = data.normals[(e * data.rings + i) * 2];
						if (a.dot(b) >= threshold)
						{
							Vec3 average;
							if (not Unit(Vec3{ a } + Vec3{ b }, average))
							{
								return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Opposing smoothing normals");
							}
							a = b = average;
						}
					}
				}
			}
			if (options.smoothingAngle > 0)
			{
				data.tangents.resize(data.normals.size());
				for (size_t e = 0; e < data.width; ++e)
				{
					for (size_t i = 0; i < data.rings; ++i)
					{
						const Vec3 edge = Vec3{ data.points[i * data.width + (e + 1) % data.width] } - Vec3{ data.points[i * data.width + e] };
						for (size_t side = 0; side < 2; ++side)
						{
							const size_t n = (e * data.rings + i) * 2 + side;
							const Vec3 normal{ data.normals[n] };
							Vec3 tangent;
							if (not Unit(edge - normal * (edge.dot(normal) / normal.lengthSq()), tangent))
							{
								return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Undefined smoothed tangent");
							}
							data.tangents[n] = tangent;
						}
					}
				}
			}
			for (size_t e = 0; e < data.width; ++e)
			{
				for (size_t i = 0; i + 1 < data.rings; ++i)
				{
					const size_t j = (e + 1) % data.width;
					const Vec3 a{ data.points[i * data.width + e] }, b{ data.points[i * data.width + j] };
					const Vec3 c{ data.points[(i + 1) * data.width + e] }, d{ data.points[(i + 1) * data.width + j] };
					const size_t n = (e * data.rings + i) * 2;
					const Vec3 n0 = Vec3{ data.normals[n] } + Vec3{ data.normals[n + 1] } + Vec3{ data.normals[n + 2] };
					const Vec3 n1 = Vec3{ data.normals[n + 2] } + Vec3{ data.normals[n + 1] } + Vec3{ data.normals[n + 3] };
					if (not (((b - a).cross(c - a).dot(n0) > 0) && ((b - c).cross(d - c).dot(n1) > 0)))
					{
						return Fail(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Loft(): Collapsed or locally folded side triangle");
					}
				}
			}
			return{};
		}

		void Write(const PreparedLoft& data, const LoftOptions& options, Mesh3D& mesh, const size_t vertexBase, const size_t triangleBase)
		{
			const float uSign = (options.uvScale.x < 0 ? -1.0f : 1.0f), vSign = (options.uvScale.y < 0 ? -1.0f : 1.0f);
			const auto uv = [&](const double u, const double v) { return Float2{ static_cast<float>(options.uvOffset.x + options.uvScale.x * u), static_cast<float>(options.uvOffset.y + options.uvScale.y * v) }; };
			size_t base = vertexBase;
			auto* triangle = mesh.indices.data() + triangleBase;
			for (size_t k = 0; k < 2; ++k)
			{
				const auto& cap = data.caps[k];
				if (cap.points.isEmpty()) { continue; }
				const Float3 normal = cap.frame.normal * (k == 0 ? -1 : 1);
				const Float3 tangent = cap.frame.x.normalized() * uSign;
				for (size_t j = 0; j < cap.points.size(); ++j)
				{
					const Float2 p = cap.polygon.vertices()[j];
					const double u = (static_cast<double>(p.x) - cap.min.x) / (static_cast<double>(cap.max.x) - cap.min.x);
					const double v = (static_cast<double>(p.y) - cap.min.y) / (static_cast<double>(cap.max.y) - cap.min.y);
					mesh.vertices[base + j] = Vertex3D{ .pos = cap.points[j], .normal = normal,
						.tex = uv(u, k == 0 ? 1 - v : v), .tangent = Float4{ tangent, uSign * vSign } };
				}
				ForEachValidCapTriangle(cap.polygon.vertices(), cap.polygon.indices(), cap.triangles, [&](const TriangleIndex& t)
				{
					*triangle++ = TriangleIndex32{ static_cast<uint32>(base + t.i0), static_cast<uint32>(base + (k == 0 ? t.i2 : t.i1)), static_cast<uint32>(base + (k == 0 ? t.i1 : t.i2)) };
				});
				base += cap.points.size();
			}
			for (size_t e = 0; e < data.width; ++e)
			{
				const size_t next = (e + 1) % data.width;
				for (size_t i = 0; i < data.rings; ++i)
				{
					const Float3 edge = (options.smoothingAngle == 0
						? Float3{ (Vec3{ data.points[i * data.width + next] } - Vec3{ data.points[i * data.width + e] }).normalized() }
						: Float3::Zero());
					for (size_t side = 0; side < 2; ++side)
					{
						const size_t n = (e * data.rings + i) * 2 + side;
						const Float3 tangent = (options.smoothingAngle == 0 ? edge : data.tangents[n]) * uSign;
						mesh.vertices[base + n] = Vertex3D{ .pos = data.points[i * data.width + (side ? next : e)],
							.normal = data.normals[n], .tex = uv(data.u[e + side], data.distances[i]), .tangent = Float4{ tangent, uSign * vSign } };
					}
					if (i + 1 < data.rings)
					{
						const uint32 v = static_cast<uint32>(base + (e * data.rings + i) * 2);
						*triangle++ = TriangleIndex32{ v, v + 1, v + 2 };
						*triangle++ = TriangleIndex32{ v + 2, v + 1, v + 3 };
					}
				}
			}
		}
	}

	Mesh3DAddResult Mesh3DDetail::AppendLoft(Mesh3D& mesh, const std::span<const LoftSection> sections, const LoftOptions& options)
	{
		PreparedLoft data;
		if (auto result = Prepare(sections, options, data); not result) { return Err{ std::move(result.error()) }; }
		const size_t vertexBase = mesh.vertexCount(), triangleBase = mesh.triangleCount();
		size_t vertices, triangles;
		if (not (CheckedAdd(vertexBase, data.vertices, vertices) && vertices <= Mesh3D::MaxVertexCount
			&& CheckedAdd(triangleBase, data.triangles, triangles)))
		{
			return Fail(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Loft(): Output exceeds mesh limits");
		}
		mesh.vertices.resize(vertices);
		mesh.indices.resize(triangles);
		Write(data, options, mesh, vertexBase, triangleBase);
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
