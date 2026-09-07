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
# include <array>
# include <limits>
# include <sstream>
# include <type_traits>

namespace
{
	constexpr float MatrixEpsilon = 1e-5f;

	Float4 GetRow(const Mat4x4& matrix, const size_t index)
	{
		return SimdFloat4{ matrix.value.r[index] }.toFloat4();
	}

	void CheckVector(const Float3 actual, const Float3 expected, const float epsilon = MatrixEpsilon)
	{
		CHECK(actual.epsilonEquals(expected, epsilon));
	}
}

TEST_CASE("Mat4x4::constructors")
{
	static_assert(sizeof(Mat4x4) == 64);
	static_assert(alignof(Mat4x4) == 16);
	static_assert(std::is_trivially_copyable_v<Mat4x4>);
	static_assert(std::is_trivially_default_constructible_v<Mat4x4>);

	const Mat4x4 rows{
		Float4{ 1, 2, 3, 4 }, Float4{ 5, 6, 7, 8 },
		Float4{ 9, 10, 11, 12 }, Float4{ 13, 14, 15, 16 }
	};
	CHECK_EQ(GetRow(rows, 0), (Float4{ 1, 2, 3, 4 }));
	CHECK_EQ(GetRow(rows, 3), (Float4{ 13, 14, 15, 16 }));

	const Mat4x4 simdRows{
		SimdFloat4{ 1, 2, 3, 4 }, SimdFloat4{ 5, 6, 7, 8 },
		SimdFloat4{ 9, 10, 11, 12 }, SimdFloat4{ 13, 14, 15, 16 }
	};
	CHECK_EQ(simdRows, rows);

	const Mat4x4 elements{
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 12,
		13, 14, 15, 16
	};
	CHECK_EQ(elements, rows);

	const std::array<float, 16> array{
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 12,
		13, 14, 15, 16
	};
	CHECK_EQ(Mat4x4{ std::span<const float, 16>{ array } }, rows);
	CHECK_EQ(Mat4x4{ rows.value }, rows);
	CHECK_EQ(Mat4x4{ rows }, rows);
}

TEST_CASE("Mat4x4::arithmetic_and_comparison")
{
	const Mat4x4 identity = Mat4x4::Identity();
	const Mat4x4 twice = (identity * 2.0f);
	CHECK_EQ(+identity, identity);
	CHECK_EQ(-(-identity), identity);
	CHECK_EQ(identity + identity, twice);
	CHECK_EQ(twice - identity, identity);
	CHECK_EQ(2.0f * identity, twice);
	CHECK_EQ(twice / 2.0f, identity);

	Mat4x4 value = identity;
	CHECK_EQ(&(value += identity), &value);
	CHECK_EQ(value, twice);
	CHECK_EQ(&(value -= identity), &value);
	CHECK_EQ(&(value *= 2.0f), &value);
	CHECK_EQ(&(value /= 2.0f), &value);
	CHECK_EQ(value, identity);

	const Mat4x4 nearIdentity{
		1.000001f, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CHECK_FALSE(nearIdentity == identity);
	CHECK(nearIdentity.epsilonEquals(identity, 0.00001f));
	CHECK_FALSE(nearIdentity.epsilonEquals(identity, 0.0000001f));
}

TEST_CASE("Mat4x4::classification")
{
	const Mat4x4 identity = Mat4x4::Identity();
	CHECK(identity.isIdentity());
	CHECK(identity.isAffine());
	CHECK(identity.isFinite());
	CHECK_FALSE(identity.hasNaN());
	CHECK_FALSE(identity.hasInf());

	const Mat4x4 projective{
		1, 0, 0, 0.5f,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CHECK_FALSE(projective.isAffine());

	const float nan = std::numeric_limits<float>::quiet_NaN();
	const float inf = std::numeric_limits<float>::infinity();
	const Mat4x4 nanMatrix{
		nan, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CHECK(nanMatrix.hasNaN());
	CHECK_FALSE(nanMatrix.isFinite());

	const Mat4x4 infMatrix{
		1, 0, 0, 0,
		0, inf, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CHECK(infMatrix.hasInf());
	CHECK_FALSE(infMatrix.isFinite());
}

TEST_CASE("Mat4x4::inverse_and_decompose")
{
	const Float3 expectedScale{ 2.0f, 3.0f, 4.0f };
	const Quaternion expectedRotation = Quaternion::RollPitchYaw(0.3f, -0.4f, 0.5f);
	const Float3 expectedTranslation{ 5.0f, -6.0f, 7.0f };
	const Mat4x4 matrix = Mat4x4::AffineTransform(expectedScale, expectedRotation, expectedTranslation);

	CHECK(matrix.determinant() == doctest::Approx(24.0f).epsilon(MatrixEpsilon));
	float inverseDeterminant = 0.0f;
	const Mat4x4 inverse = matrix.inverse(inverseDeterminant);
	CHECK(inverseDeterminant == doctest::Approx(24.0f).epsilon(MatrixEpsilon));
	CHECK((matrix * inverse).epsilonEquals(Mat4x4::Identity(), MatrixEpsilon));
	CHECK(matrix.inverse().epsilonEquals(inverse, MatrixEpsilon));

	Mat4x4 tryResult = Mat4x4::Identity();
	float tryDeterminant = 0.0f;
	CHECK(matrix.tryInverse(tryResult, tryDeterminant));
	CHECK(tryResult.epsilonEquals(inverse, MatrixEpsilon));
	CHECK(tryDeterminant == doctest::Approx(24.0f).epsilon(MatrixEpsilon));
	CHECK(matrix.tryInverse(tryResult));

	Float3 scale;
	Quaternion rotation;
	Float3 translation;
	CHECK(matrix.decompose(scale, rotation, translation));
	CheckVector(scale, expectedScale);
	CHECK(rotation.rotationEquals(expectedRotation));
	CheckVector(translation, expectedTranslation);
	CHECK(Mat4x4::AffineTransform(scale, rotation, translation).epsilonEquals(matrix, MatrixEpsilon));

	const Mat4x4 singular = Mat4x4::Scale(Float3{ 1.0f, 0.0f, 2.0f });
	const Mat4x4 sentinel = Mat4x4::Translate(1.0f, 2.0f, 3.0f);
	tryResult = sentinel;
	tryDeterminant = 1.0f;
	CHECK_FALSE(singular.tryInverse(tryResult, tryDeterminant));
	CHECK_EQ(tryResult, sentinel);
	CHECK_EQ(tryDeterminant, 0.0f);
	CHECK_FALSE(singular.tryInverse(tryResult));
	CHECK_EQ(tryResult, sentinel);

	const Mat4x4 shear{
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CHECK_FALSE(shear.decompose(scale, rotation, translation));
}

TEST_CASE("Mat4x4::composition_and_transform")
{
	const Float3 point{ 1.0f, 2.0f, 3.0f };
	const Float3 vector{ 1.0f, 2.0f, 3.0f };
	const Mat4x4 translate = Mat4x4::Translate(4.0f, 5.0f, 6.0f);
	CheckVector(translate.transformPoint(point), Float3{ 5.0f, 7.0f, 9.0f });
	CheckVector(translate.transformVector(vector), vector);
	CHECK(translate.transform(Float4{ point, 1.0f }).epsilonEquals(Float4{ 5.0f, 7.0f, 9.0f, 1.0f }, MatrixEpsilon));

	const Mat4x4 scale = Mat4x4::Scale(Float3{ 2.0f, 3.0f, 4.0f });
	const Mat4x4 composed = (translate * scale);
	CheckVector(composed.transformPoint(point), Float3{ 10.0f, 21.0f, 36.0f });
	CHECK(translate.scaled(Float3{ 2.0f, 3.0f, 4.0f }).epsilonEquals(composed, MatrixEpsilon));
	CHECK(Mat4x4::Identity().translated(4.0f, 5.0f, 6.0f).epsilonEquals(translate, MatrixEpsilon));
	CHECK(Mat4x4::Identity().translated(Float3{ 4.0f, 5.0f, 6.0f }).epsilonEquals(translate, MatrixEpsilon));
	CHECK(Mat4x4::Identity().scaled(2.0f).epsilonEquals(Mat4x4::Scale(2.0f), MatrixEpsilon));

	Mat4x4 mutableValue = translate;
	CHECK_EQ(&(mutableValue *= scale), &mutableValue);
	CHECK(mutableValue.epsilonEquals(composed, MatrixEpsilon));

	const Mat4x4 transposed = composed.transposed();
	CHECK(transposed.transposed().epsilonEquals(composed, MatrixEpsilon));
}

TEST_CASE("Mat4x4::rotation_factories")
{
	const float angle = 0.75f;
	CHECK(Mat4x4::RotateX(angle).epsilonEquals(Mat4x4::Rotate(Quaternion::RotateX(angle)), MatrixEpsilon));
	CHECK(Mat4x4::RotateY(angle).epsilonEquals(Mat4x4::Rotate(Quaternion::RotateY(angle)), MatrixEpsilon));
	CHECK(Mat4x4::RotateZ(angle).epsilonEquals(Mat4x4::Rotate(Quaternion::RotateZ(angle)), MatrixEpsilon));
	CHECK(Mat4x4::RotateNormal(Float3::UnitZ(), angle)
		.epsilonEquals(Mat4x4::RotateAxis(Float3{ 0.0f, 0.0f, 3.0f }, angle), MatrixEpsilon));
	CHECK(Mat4x4::RollPitchYaw(Float3{ 0.2f, -0.3f, 0.4f })
		.epsilonEquals(Mat4x4::RollPitchYaw(0.2f, -0.3f, 0.4f), MatrixEpsilon));
	CHECK(Mat4x4::RollPitchYaw(0.2f, -0.3f, 0.4f)
		.epsilonEquals(Mat4x4::Rotate(Quaternion::RollPitchYaw(0.2f, -0.3f, 0.4f)), MatrixEpsilon));

	const Mat4x4 identity = Mat4x4::Identity();
	CHECK(identity.rotatedX(angle).epsilonEquals(Mat4x4::RotateX(angle), MatrixEpsilon));
	CHECK(identity.rotatedY(angle).epsilonEquals(Mat4x4::RotateY(angle), MatrixEpsilon));
	CHECK(identity.rotatedZ(angle).epsilonEquals(Mat4x4::RotateZ(angle), MatrixEpsilon));
	CHECK(identity.rotatedRollPitchYaw(0.2f, -0.3f, 0.4f)
		.epsilonEquals(Mat4x4::RollPitchYaw(0.2f, -0.3f, 0.4f), MatrixEpsilon));
	CHECK(identity.rotatedRollPitchYaw(Float3{ 0.2f, -0.3f, 0.4f })
		.epsilonEquals(Mat4x4::RollPitchYaw(0.2f, -0.3f, 0.4f), MatrixEpsilon));
	CHECK(identity.rotatedNormal(Float3::UnitZ(), angle)
		.epsilonEquals(Mat4x4::RotateNormal(Float3::UnitZ(), angle), MatrixEpsilon));
	CHECK(identity.rotatedAxis(Float3{ 0.0f, 0.0f, 3.0f }, angle)
		.epsilonEquals(Mat4x4::RotateAxis(Float3{ 0.0f, 0.0f, 3.0f }, angle), MatrixEpsilon));
	CHECK(identity.rotated(Quaternion::RotateZ(angle))
		.epsilonEquals(Mat4x4::RotateZ(angle), MatrixEpsilon));

	const Float3 center{ 2.0f, -3.0f, 4.0f };
	CheckVector(Mat4x4::Scale(2.0f, center).transformPoint(center), center);
	CheckVector(Mat4x4::Scale(Float3{ 2.0f, 3.0f, 4.0f }, center).transformPoint(center), center);
	CheckVector(Mat4x4::RotateNormal(Float3::UnitZ(), angle, center).transformPoint(center), center);
	CheckVector(Mat4x4::RotateAxis(Float3{ 0.0f, 0.0f, 3.0f }, angle, center).transformPoint(center), center);
	CheckVector(Mat4x4::Rotate(Quaternion::RotateZ(angle), center).transformPoint(center), center);
}

TEST_CASE("Mat4x4::stream_transform")
{
	const Mat4x4 matrix = Mat4x4::Scale(Float3{ 2.0f, 3.0f, 4.0f }).translated(5.0f, 6.0f, 7.0f);
	const std::array<Float3, 2> input{ Float3{ 1, 2, 3 }, Float3{ -1, -2, -3 } };
	std::array<Float3, 2> points;
	std::array<Float3, 2> vectors;
	matrix.transformPoints(points.data(), input.data(), input.size());
	matrix.transformVectors(vectors.data(), input.data(), input.size());
	for (size_t i = 0; i < input.size(); ++i)
	{
		CheckVector(points[i], matrix.transformPoint(input[i]));
		CheckVector(vectors[i], matrix.transformVector(input[i]));
	}

	struct StridedVector
	{
		Float3 value;
		float padding;
	};
	const std::array<StridedVector, 2> stridedInput{
		StridedVector{ input[0], 123.0f }, StridedVector{ input[1], 456.0f }
	};
	std::array<StridedVector, 2> stridedOutput{
		StridedVector{ Float3::Zero(), 321.0f }, StridedVector{ Float3::Zero(), 654.0f }
	};
	matrix.transformPoints(&stridedOutput[0].value, sizeof(StridedVector),
		&stridedInput[0].value, sizeof(StridedVector), stridedInput.size());
	CheckVector(stridedOutput[0].value, points[0]);
	CheckVector(stridedOutput[1].value, points[1]);
	CHECK_EQ(stridedOutput[0].padding, 321.0f);
	CHECK_EQ(stridedOutput[1].padding, 654.0f);

	matrix.transformVectors(&stridedOutput[0].value, sizeof(StridedVector),
		&stridedInput[0].value, sizeof(StridedVector), stridedInput.size());
	CheckVector(stridedOutput[0].value, vectors[0]);
	CheckVector(stridedOutput[1].value, vectors[1]);
}

TEST_CASE("Mat4x4::Quaternion_round_trip")
{
	const Quaternion source = Quaternion::RollPitchYaw(0.3f, -0.7f, 1.1f);
	const Quaternion roundTrip = Quaternion::FromRotationMatrix(Mat4x4::Rotate(source));
	CHECK(roundTrip.rotationEquals(source));
}

TEST_CASE("Mat4x4::format_and_stream")
{
	const Mat4x4 identity = Mat4x4::Identity();
	const String expected = U"((1, 0, 0, 0),(0, 1, 0, 0),(0, 0, 1, 0),(0, 0, 0, 1))";
	CHECK_EQ(Format(identity), expected);

	std::stringstream output;
	output << identity;
	CHECK_EQ(output.str(), "((1, 0, 0, 0),(0, 1, 0, 0),(0, 0, 1, 0),(0, 0, 0, 1))");

	std::stringstream input{ "((1, 0, 0, 0),(0, 2, 0, 0),(0, 0, 3, 0),(4, 5, 6, 1))" };
	Mat4x4 parsed;
	input >> parsed;
	CHECK(input);
	CHECK(parsed.epsilonEquals(Mat4x4::Scale(Float3{ 1, 2, 3 }).translated(4, 5, 6), MatrixEpsilon));
}
