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

# include <Siv3D/Mat3x3.hpp>

# include <algorithm>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		using Matrix = std::array<std::array<double, 3>, 3>;
		using Points = std::array<Vec2, 4>;

		struct NormalizedQuad
		{
			Points points;
			Vec2 origin;
			double scale;
		};

		NormalizedQuad Normalize(const Quad& quad)
		{
			NormalizedQuad result{ { quad.p0, quad.p1, quad.p2, quad.p3 }, quad.p0, 0.0 };
			for (auto& point : result.points)
			{
				point -= result.origin;
				result.scale = std::max({ result.scale, std::abs(point.x), std::abs(point.y) });
			}
			for (auto& point : result.points)
			{
				point /= result.scale;
			}
			return result;
		}

		bool IsConvex(const NormalizedQuad& quad)
		{
			if ((not std::isfinite(quad.origin.x)) || (not std::isfinite(quad.origin.y))
				|| (not std::isfinite(quad.scale)) || (quad.scale <= 0.0))
			{
				return false;
			}
			double previous = 0.0;
			for (size_t i = 0; i < 4; ++i)
			{
				const Vec2 a = (quad.points[(i + 1) % 4] - quad.points[i]);
				const Vec2 b = (quad.points[(i + 2) % 4] - quad.points[(i + 1) % 4]);
				const double cross = (a.x * b.y - a.y * b.x);
				if ((not std::isfinite(cross)) || (std::abs(cross) <= (64 * std::numeric_limits<double>::epsilon()))
					|| ((i != 0) && ((cross > 0.0) != (previous > 0.0))))
				{
					return false;
				}
				previous = cross;
			}
			return true;
		}

		Matrix Multiply(const Matrix& a, const Matrix& b)
		{
			Matrix result{};
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					for (size_t k = 0; k < 3; ++k)
					{
						result[i][j] += (a[i][k] * b[k][j]);
					}
				}
			}
			return result;
		}

		Matrix Adjugate(const Matrix& a)
		{
			Matrix result{};
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					result[j][i] = (a[(i + 1) % 3][(j + 1) % 3] * a[(i + 2) % 3][(j + 2) % 3]
						- a[(i + 1) % 3][(j + 2) % 3] * a[(i + 2) % 3][(j + 1) % 3]);
				}
			}
			return result;
		}

		Matrix FromUnitSquare(const Points& p)
		{
			const Vec2 a = (p[1] - p[2]);
			const Vec2 b = (p[3] - p[2]);
			const Vec2 c = (p[0] - p[1] + p[2] - p[3]);
			const double determinant = (a.x * b.y - b.x * a.y);
			const double u = ((c.x * b.y - b.x * c.y) / determinant);
			const double v = ((a.x * c.y - c.x * a.y) / determinant);
			return {{
				{ p[1].x - p[0].x + u * p[1].x, p[1].y - p[0].y + u * p[1].y, u },
				{ p[3].x - p[0].x + v * p[3].x, p[3].y - p[0].y + v * p[3].y, v },
				{ p[0].x, p[0].y, 1.0 }
			}};
		}

		Mat3x3 MakeHomography(const NormalizedQuad& from, const NormalizedQuad& to)
		{
			// Row vectors: source normalization, inverse source quad, target quad, target denormalization.
			const Matrix source{{ { 1 / from.scale, 0, 0 }, { 0, 1 / from.scale, 0 },
				{ -from.origin.x / from.scale, -from.origin.y / from.scale, 1 } }};
			const Matrix target{{ { to.scale, 0, 0 }, { 0, to.scale, 0 }, { to.origin.x, to.origin.y, 1 } }};
			Matrix result = Multiply(Multiply(Multiply(source, Adjugate(FromUnitSquare(from.points))),
				FromUnitSquare(to.points)), target);
			double magnitude = 0.0;
			for (const auto& row : result)
			{
				for (const double value : row)
				{
					magnitude = std::max(magnitude, std::abs(value));
				}
			}
			// A projective matrix is defined up to a nonzero scalar. Do not assume _33 != 0.
			const double w = (from.origin.x * result[0][2] + from.origin.y * result[1][2] + result[2][2]);
			const double divisor = std::copysign(magnitude, w);
			for (auto& row : result)
			{
				for (double& value : row)
				{
					value /= divisor;
				}
			}
			return { static_cast<float>(result[0][0]), static_cast<float>(result[0][1]), static_cast<float>(result[0][2]),
				static_cast<float>(result[1][0]), static_cast<float>(result[1][1]), static_cast<float>(result[1][2]),
				static_cast<float>(result[2][0]), static_cast<float>(result[2][1]), static_cast<float>(result[2][2]) };
		}

		bool IsRepresentable(const Mat3x3& matrix, const Quad& from, const NormalizedQuad& to)
		{
			const Matrix m{{ { matrix._11, matrix._12, matrix._13 }, { matrix._21, matrix._22, matrix._23 },
				{ matrix._31, matrix._32, matrix._33 } }};
			for (const auto& row : m)
			{
				for (const double value : row)
				{
					if (not std::isfinite(value)) { return false; }
				}
			}
			const Matrix adj = Adjugate(m);
			if ((m[0][0] * adj[0][0] + m[0][1] * adj[1][0] + m[0][2] * adj[2][0]) == 0.0)
			{
				return false;
			}
			const Points source{ from.p0, from.p1, from.p2, from.p3 };
			for (size_t i = 0; i < 4; ++i)
			{
				const Vec2 p = source[i];
				const double w = (p.x * matrix._13 + p.y * matrix._23 + matrix._33);
				const Vec2 mapped = ((matrix.transformPoint(p) - to.origin) / to.scale);
				const Vec2 error = (mapped - to.points[i]);
				if ((w <= 0.0) || (not std::isfinite(error.x)) || (not std::isfinite(error.y))
					|| (std::max(std::abs(error.x), std::abs(error.y)) > 1e-4))
				{
					return false;
				}
			}
			return true;
		}
	}

	Mat3x3 Mat3x3::Homography(const Quad& to)
	{
		return Homography(RectF{ 0, 0, 1, 1 }.asQuad(), to);
	}

	Mat3x3 Mat3x3::Homography(const std::array<Float2, 4>& to)
	{
		return Homography(Quad{ to[0], to[1], to[2], to[3] });
	}

	Mat3x3 Mat3x3::Homography(const RectF& from, const RectF& to)
	{
		return Homography(from.asQuad(), to.asQuad());
	}

	Mat3x3 Mat3x3::Homography(const RectF& from, const Quad& to)
	{
		return Homography(from.asQuad(), to);
	}

	Mat3x3 Mat3x3::Homography(const Quad& from, const RectF& to)
	{
		return Homography(from, to.asQuad());
	}

	Mat3x3 Mat3x3::Homography(const Quad& from, const Quad& to)
	{
		return MakeHomography(Normalize(from), Normalize(to));
	}

	Optional<Mat3x3> Mat3x3::TryHomography(const Quad& to)
	{
		return TryHomography(RectF{ 0, 0, 1, 1 }.asQuad(), to);
	}

	Optional<Mat3x3> Mat3x3::TryHomography(const std::array<Float2, 4>& to)
	{
		return TryHomography(Quad{ to[0], to[1], to[2], to[3] });
	}

	Optional<Mat3x3> Mat3x3::TryHomography(const RectF& from, const RectF& to)
	{
		return TryHomography(from.asQuad(), to.asQuad());
	}

	Optional<Mat3x3> Mat3x3::TryHomography(const RectF& from, const Quad& to)
	{
		return TryHomography(from.asQuad(), to);
	}

	Optional<Mat3x3> Mat3x3::TryHomography(const Quad& from, const RectF& to)
	{
		return TryHomography(from, to.asQuad());
	}

	Optional<Mat3x3> Mat3x3::TryHomography(const Quad& from, const Quad& to)
	{
		const NormalizedQuad source = Normalize(from);
		const NormalizedQuad target = Normalize(to);
		if ((not IsConvex(source)) || (not IsConvex(target)))
		{
			return none;
		}
		const Mat3x3 result = MakeHomography(source, target);
		return IsRepresentable(result, from, target) ? Optional<Mat3x3>{ result } : none;
	}

    ////////////////////////////////////////////////////////////////
    //
    //	Formatter
    //
    ////////////////////////////////////////////////////////////////

    void Formatter(FormatData& formatData, const Mat3x3& value)
    {
        formatData.string.push_back(U'(');
        Formatter(formatData, Float3{ value._11, value._12, value._13 });
        formatData.string.push_back(U',');
        Formatter(formatData, Float3{ value._21, value._22, value._23 });
        formatData.string.push_back(U',');
        Formatter(formatData, Float3{ value._31, value._32, value._33 });
        formatData.string.push_back(U')');
    }
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::Mat3x3, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Mat3x3, s3d::char32>::format(const s3d::Mat3x3& value, s3d::BufferContext& ctx) const
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"(({}, {}, {}), ({}, {}, {}), ({}, {}, {}))",
			value._11, value._12, value._13, value._21, value._22, value._23, value._31, value._32, value._33);
	}
	else
	{
		const std::u32string format
			= (U"(({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}))");
		return format_to(ctx.out(), format, value._11, value._12, value._13, value._21, value._22, value._23, value._31, value._32, value._33);
	}
}
