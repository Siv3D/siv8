//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Mat3x3.hpp>

# if __has_include(<opencv2/core.hpp>)
#	include <opencv2/core.hpp>
# endif
# if  __has_include(<opencv2/imgproc.hpp>)
#	include <opencv2/imgproc.hpp>
# endif

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Homography
	//
	////////////////////////////////////////////////////////////////

	Mat3x3 Mat3x3::Homography(const Quad& to)
	{
		const double x0 = to.p0.x;
		const double y0 = to.p0.y;
		const double x1 = to.p1.x;
		const double y1 = to.p1.y;
		const double x2 = to.p2.x;
		const double y2 = to.p2.y;
		const double x3 = to.p3.x;
		const double y3 = to.p3.y;

		const double dx10x11	= (x1 - x2);
		const double dx01x11	= (x3 - x2);
		const double dxAll		= (x0 - x3 - x1 + x2);
		const double dy10y11	= (y1 - y2);
		const double dy01y11	= (y3 - y2);
		const double dyAll		= (y0 - y3 - y1 + y2);

		const double denom1 = (dx01x11 * dy10y11 - dx10x11 * dy01y11);
		const double denom2 = (dx10x11 * dy01y11 - dx01x11 * dy10y11);

		assert(1e-9 < std::fabs(denom1));
		assert(1e-9 < std::fabs(denom2));

		const double m23 = (dxAll * dy10y11 - dx10x11 * dyAll) / denom1;
		const double m13 = (dxAll * dy01y11 - dx01x11 * dyAll) / denom2;
		const double m11 = (x1 - x0) + m13 * x1;
		const double m21 = (x3 - x0) + m23 * x3;
		const double m12 = (y1 - y0) + m13 * y1;
		const double m22 = (y3 - y0) + m23 * y3;
		const double m31 = x0;
		const double m32 = y0;

		return{
			static_cast<float>(m11), static_cast<float>(m12), static_cast<float>(m13),
			static_cast<float>(m21), static_cast<float>(m22), static_cast<float>(m23),
			static_cast<float>(m31), static_cast<float>(m32), 1.0f
		};
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
		const std::array<cv::Point2f, 4> fromQ = {
			cv::Point2f(static_cast<float>(from.p0.x), static_cast<float>(from.p0.y)),
			cv::Point2f(static_cast<float>(from.p1.x), static_cast<float>(from.p1.y)),
			cv::Point2f(static_cast<float>(from.p2.x), static_cast<float>(from.p2.y)),
			cv::Point2f(static_cast<float>(from.p3.x), static_cast<float>(from.p3.y)),
		};

		const std::array<cv::Point2f, 4> toQ = {
			cv::Point2f(static_cast<float>(to.p0.x), static_cast<float>(to.p0.y)),
			cv::Point2f(static_cast<float>(to.p1.x), static_cast<float>(to.p1.y)),
			cv::Point2f(static_cast<float>(to.p2.x), static_cast<float>(to.p2.y)),
			cv::Point2f(static_cast<float>(to.p3.x), static_cast<float>(to.p3.y)),
		};

		const cv::Mat t = cv::getPerspectiveTransform(fromQ, toQ);
		const double* p0 = t.ptr<double>(0);
		const double* p1 = t.ptr<double>(1);
		const double* p2 = t.ptr<double>(2);

		return{
			static_cast<float>(p0[0]), static_cast<float>(p1[0]), static_cast<float>(p2[0]),
			static_cast<float>(p0[1]), static_cast<float>(p1[1]), static_cast<float>(p2[1]),
			static_cast<float>(p0[2]), static_cast<float>(p1[2]), static_cast<float>(p2[2]),
		};
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

s3d::BufferContext::iterator fmt::formatter<s3d::Mat3x3, s3d::char32>::format(const s3d::Mat3x3& value, s3d::BufferContext& ctx)
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
