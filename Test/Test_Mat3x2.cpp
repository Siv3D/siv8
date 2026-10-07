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

# include "Siv3DTest.hpp"
# include <cmath>

TEST_CASE("Mat3x2::rmsScaling")
{
	static_assert(noexcept(Mat3x2::Identity().rmsScaling()));

	CHECK(Mat3x2::Identity().rmsScaling() == 1.0f);
	CHECK(Mat3x2::Translate(100.0f, -200.0f).rmsScaling() == 1.0f);
	CHECK(Mat3x2::Scale(3.0f).rmsScaling() == 3.0f);
	CHECK(Mat3x2::Scale(-3.0f).rmsScaling() == 3.0f);
	CHECK(Mat3x2::Scale(-1.0f, 1.0f).rmsScaling() == 1.0f);
	CHECK(Mat3x2::Scale(0.0f).rmsScaling() == 0.0f);
	CHECK(Mat3x2::Scale(0.0f).translated(100.0f, -200.0f).rmsScaling() == 0.0f);
	CHECK(std::abs(Mat3x2::Scale(4.0f, 1.0f).rmsScaling() - std::sqrt(8.5f)) < 1e-5f);
	CHECK(std::abs(Mat3x2::Scale(4.0f, 0.0f).rmsScaling() - std::sqrt(8.0f)) < 1e-5f);
	CHECK(std::abs(Mat3x2::ShearX(2.0f).rmsScaling() - std::sqrt(3.0f)) < 1e-5f);
	CHECK(std::abs(Mat3x2::ShearY(2.0f).rmsScaling() - std::sqrt(3.0f)) < 1e-5f);

	// A nonzero linear map that annihilates the diagonal direction still has scale.
	const Mat3x2 collapsedDiagonal{ 1, 0, -1, 0, 0, 0 };
	CHECK(collapsedDiagonal.rmsScaling() == 1.0f);
}

TEST_CASE("Mat3x2::rmsScaling_rotation_invariance")
{
	const Mat3x2 transforms[] = {
		Mat3x2::Scale(4.0f, 1.0f),
		Mat3x2::ShearX(2.0f),
		Mat3x2::Scale(-2.0f, 0.0f),
	};

	for (const auto& transform : transforms)
	{
		for (int32 i = 0; i < 24; ++i)
		{
			const Mat3x2 rotation = Mat3x2::Rotate(i * 15_deg);
			CHECK(std::abs((rotation * transform).rmsScaling() - transform.rmsScaling()) < 1e-5f);
			CHECK(std::abs((transform * rotation).rmsScaling() - transform.rmsScaling()) < 1e-5f);
		}
	}
}

TEST_CASE("Mat3x2::rmsScaling_directional_mean")
{
	const Mat3x2 transform{ 2.0f, -1.0f, 3.0f, 0.5f, 0.0f, 0.0f };
	double squaredLengthSum = 0.0;
	constexpr int32 Samples = 360;
	for (int32 i = 0; i < Samples; ++i)
	{
		const double angle = (i * 1_deg);
		const Float2 direction{ static_cast<float>(std::cos(angle)), static_cast<float>(std::sin(angle)) };
		squaredLengthSum += transform.transformPoint(direction).lengthSq();
	}
	CHECK(std::abs(transform.rmsScaling() - std::sqrt(squaredLengthSum / Samples)) < 1e-5);
}
