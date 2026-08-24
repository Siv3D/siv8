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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4::Mat4x4(const DirectX::XMMATRIX matrix) noexcept
		: value{ matrix } {}

	inline Mat4x4::Mat4x4(const SimdFloat4 row0, const SimdFloat4 row1, const SimdFloat4 row2, const SimdFloat4 row3) noexcept
		: value{ row0.vec, row1.vec, row2.vec, row3.vec } {}

	inline Mat4x4::Mat4x4(const Float4 row0, const Float4 row1, const Float4 row2, const Float4 row3) noexcept
		: Mat4x4{ SimdFloat4{ row0 }, SimdFloat4{ row1 }, SimdFloat4{ row2 }, SimdFloat4{ row3 } } {}

	inline Mat4x4::Mat4x4(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33) noexcept
		: value{ m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33 } {}

	inline Mat4x4::Mat4x4(const std::span<const float, 16> elements) noexcept
		: value{ elements.data() } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator +() const noexcept
	{
		return *this;
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator +(const Mat4x4& other) const noexcept
	{
		return Mat4x4{ value + other.value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator -() const noexcept
	{
		return Mat4x4{ -value };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator -(const Mat4x4& other) const noexcept
	{
		return Mat4x4{ value - other.value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator *(const float s) const noexcept
	{
		return Mat4x4{ value * s };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator *(const Mat4x4& other) const noexcept
	{
		return Mat4x4{ DirectX::XMMatrixMultiply(value, other.value) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL operator *(const float s, const Mat4x4& matrix) noexcept
	{
		return (matrix * s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::operator /(const float s) const noexcept
	{
		return Mat4x4{ value / s };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4& SIV3D_VECTOR_CALL Mat4x4::operator +=(const Mat4x4& other) & noexcept
	{
		value += other.value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4& SIV3D_VECTOR_CALL Mat4x4::operator -=(const Mat4x4& other) & noexcept
	{
		value -= other.value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4& SIV3D_VECTOR_CALL Mat4x4::operator *=(const float s) & noexcept
	{
		value *= s;
		return *this;
	}

	inline Mat4x4& SIV3D_VECTOR_CALL Mat4x4::operator *=(const Mat4x4& other) & noexcept
	{
		value = DirectX::XMMatrixMultiply(value, other.value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4& SIV3D_VECTOR_CALL Mat4x4::operator /=(const float s) & noexcept
	{
		value /= s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ==
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL operator ==(const Mat4x4& lhs, const Mat4x4& rhs) noexcept
	{
		return (DirectX::XMVector4Equal(lhs.value.r[0], rhs.value.r[0])
			&& DirectX::XMVector4Equal(lhs.value.r[1], rhs.value.r[1])
			&& DirectX::XMVector4Equal(lhs.value.r[2], rhs.value.r[2])
			&& DirectX::XMVector4Equal(lhs.value.r[3], rhs.value.r[3]));
	}

	////////////////////////////////////////////////////////////////
	//
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::epsilonEquals(const Mat4x4& other, const float epsilon) const noexcept
	{
		const DirectX::XMVECTOR tolerance = DirectX::XMVectorReplicate(epsilon);
		return (DirectX::XMVector4NearEqual(value.r[0], other.value.r[0], tolerance)
			&& DirectX::XMVector4NearEqual(value.r[1], other.value.r[1], tolerance)
			&& DirectX::XMVector4NearEqual(value.r[2], other.value.r[2], tolerance)
			&& DirectX::XMVector4NearEqual(value.r[3], other.value.r[3], tolerance));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNaN
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::hasNaN() const noexcept
	{
		return DirectX::XMMatrixIsNaN(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasInf
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::hasInf() const noexcept
	{
		return DirectX::XMMatrixIsInfinite(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isFinite
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::isFinite() const noexcept
	{
		return (not hasNaN() && not hasInf());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isIdentity
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::isIdentity() const noexcept
	{
		return DirectX::XMMatrixIsIdentity(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isAffine
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::isAffine(const float epsilon) const noexcept
	{
		const DirectX::XMVECTOR lastColumn = DirectX::XMVectorSet(
			DirectX::XMVectorGetW(value.r[0]),
			DirectX::XMVectorGetW(value.r[1]),
			DirectX::XMVectorGetW(value.r[2]),
			DirectX::XMVectorGetW(value.r[3]));
		return DirectX::XMVector4NearEqual(lastColumn, DirectX::g_XMIdentityR3.v, DirectX::XMVectorReplicate(epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	decompose
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::decompose(Float3& scale, Quaternion& rotation, Float3& translation) const noexcept
	{
		SimdFloat4 scaleVector;
		SimdFloat4 translationVector;
		const bool succeeded = DirectX::XMMatrixDecompose(
			&scaleVector.vec, &rotation.value.vec, &translationVector.vec, value);
		scale = scaleVector.xyz();
		translation = translationVector.xyz();
		return succeeded;
	}

	////////////////////////////////////////////////////////////////
	//
	//	transposed
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::transposed() const noexcept
	{
		return Mat4x4{ DirectX::XMMatrixTranspose(value) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	inverse
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::inverse() const noexcept
	{
		return Mat4x4{ DirectX::XMMatrixInverse(nullptr, value) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::inverse(float& determinant) const noexcept
	{
		DirectX::XMVECTOR determinantVector;
		const DirectX::XMMATRIX result = DirectX::XMMatrixInverse(&determinantVector, value);
		determinant = DirectX::XMVectorGetX(determinantVector);
		return Mat4x4{ result };
	}

	////////////////////////////////////////////////////////////////
	//
	//	tryInverse
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Mat4x4::tryInverse(Mat4x4& result) const noexcept
	{
		float determinant;
		return tryInverse(result, determinant);
	}

	inline bool SIV3D_VECTOR_CALL Mat4x4::tryInverse(Mat4x4& result, float& determinant) const noexcept
	{
		DirectX::XMVECTOR determinantVector;
		const DirectX::XMMATRIX inverseMatrix = DirectX::XMMatrixInverse(&determinantVector, value);
		determinant = DirectX::XMVectorGetX(determinantVector);

		if (determinant == 0.0f)
		{
			return false;
		}

		result.value = inverseMatrix;
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	determinant
	//
	////////////////////////////////////////////////////////////////

	inline float SIV3D_VECTOR_CALL Mat4x4::determinant() const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	translated
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::translated(const float x, const float y, const float z) const noexcept
	{
		return (*this * Translate(x, y, z));
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::translated(const Float3 offset) const noexcept
	{
		return (*this * Translate(offset));
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaled
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::scaled(const float scale) const noexcept
	{
		return (*this * Scale(scale));
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::scaled(const Float3 scale) const noexcept
	{
		return (*this * Scale(scale));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedX, rotatedY, rotatedZ
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedX(const float angle) const noexcept
	{
		return (*this * RotateX(angle));
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedY(const float angle) const noexcept
	{
		return (*this * RotateY(angle));
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedZ(const float angle) const noexcept
	{
		return (*this * RotateZ(angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedRollPitchYaw
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedRollPitchYaw(const float pitch, const float yaw, const float roll) const noexcept
	{
		return (*this * RollPitchYaw(pitch, yaw, roll));
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedRollPitchYaw(const Float3 pitchYawRoll) const noexcept
	{
		return (*this * RollPitchYaw(pitchYawRoll));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedNormal
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedNormal(const Float3 normalAxis, const float angle) const noexcept
	{
		return (*this * RotateNormal(normalAxis, angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAxis
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotatedAxis(const Float3 axis, const float angle) const noexcept
	{
		return (*this * RotateAxis(axis, angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::rotated(const Quaternion rotation) const noexcept
	{
		return (*this * Rotate(rotation));
	}

	////////////////////////////////////////////////////////////////
	//
	//	transform
	//
	////////////////////////////////////////////////////////////////

	inline Float4 SIV3D_VECTOR_CALL Mat4x4::transform(const Float4 vector) const noexcept
	{
		return SimdFloat4{ DirectX::XMVector4Transform(SimdFloat4{ vector }.vec, value) }.toFloat4();
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformPoint
	//
	////////////////////////////////////////////////////////////////

	inline Float3 SIV3D_VECTOR_CALL Mat4x4::transformPoint(const Float3 point) const noexcept
	{
		return SimdFloat4{ DirectX::XMVector3TransformCoord(SimdFloat4{ point }.vec, value) }.xyz();
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformVector
	//
	////////////////////////////////////////////////////////////////

	inline Float3 SIV3D_VECTOR_CALL Mat4x4::transformVector(const Float3 vector) const noexcept
	{
		return SimdFloat4{ DirectX::XMVector3TransformNormal(SimdFloat4{ vector }.vec, value) }.xyz();
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformPoints
	//
	////////////////////////////////////////////////////////////////

	inline void SIV3D_VECTOR_CALL Mat4x4::transformPoints(
		Float3* const output, const Float3* const input, const size_t vectorCount) const noexcept
	{
		transformPoints(output, sizeof(Float3), input, sizeof(Float3), vectorCount);
	}

	inline void SIV3D_VECTOR_CALL Mat4x4::transformPoints(
		Float3* const output, const size_t outputStrideBytes,
		const Float3* const input, const size_t inputStrideBytes, const size_t vectorCount) const noexcept
	{
		DirectX::XMVector3TransformCoordStream(
			static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(output)), outputStrideBytes,
			static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(input)), inputStrideBytes,
			vectorCount, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformVectors
	//
	////////////////////////////////////////////////////////////////

	inline void SIV3D_VECTOR_CALL Mat4x4::transformVectors(
		Float3* const output, const Float3* const input, const size_t vectorCount) const noexcept
	{
		transformVectors(output, sizeof(Float3), input, sizeof(Float3), vectorCount);
	}

	inline void SIV3D_VECTOR_CALL Mat4x4::transformVectors(
		Float3* const output, const size_t outputStrideBytes,
		const Float3* const input, const size_t inputStrideBytes, const size_t vectorCount) const noexcept
	{
		DirectX::XMVector3TransformNormalStream(
			static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(output)), outputStrideBytes,
			static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(input)), inputStrideBytes,
			vectorCount, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Identity
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Identity() noexcept
	{
		return Mat4x4{ DirectX::XMMatrixIdentity() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Translate
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Translate(const float x, const float y, const float z) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixTranslation(x, y, z) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Translate(const Float3 offset) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixTranslationFromVector(SimdFloat4{ offset }.vec) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Scale
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Scale(const float scale) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixScaling(scale, scale, scale) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Scale(const Float3 scale) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixScalingFromVector(SimdFloat4{ scale }.vec) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Scale(const float scale, const Float3 center) noexcept
	{
		return (Translate(-center.x, -center.y, -center.z) * Scale(scale) * Translate(center));
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Scale(const Float3 scale, const Float3 center) noexcept
	{
		return (Translate(-center.x, -center.y, -center.z) * Scale(scale) * Translate(center));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RotateX, RotateY, RotateZ
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateX(const float angle) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationX(angle) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateY(const float angle) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationY(angle) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateZ(const float angle) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationZ(angle) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RollPitchYaw
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RollPitchYaw(const float pitch, const float yaw, const float roll) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RollPitchYaw(const Float3 pitchYawRoll) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationRollPitchYawFromVector(SimdFloat4{ pitchYawRoll }.vec) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RotateNormal
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateNormal(const Float3 normalAxis, const float angle) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationNormal(SimdFloat4{ normalAxis }.vec, angle) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateNormal(
		const Float3 normalAxis, const float angle, const Float3 center) noexcept
	{
		return (Translate(-center.x, -center.y, -center.z)
			* RotateNormal(normalAxis, angle)
			* Translate(center));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RotateAxis
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateAxis(const Float3 axis, const float angle) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationAxis(SimdFloat4{ axis }.vec, angle) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::RotateAxis(
		const Float3 axis, const float angle, const Float3 center) noexcept
	{
		return (Translate(-center.x, -center.y, -center.z)
			* RotateAxis(axis, angle)
			* Translate(center));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Rotate
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Rotate(const Quaternion rotation) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixRotationQuaternion(rotation.value.vec) };
	}

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::Rotate(const Quaternion rotation, const Float3 center) noexcept
	{
		return (Translate(-center.x, -center.y, -center.z)
			* Rotate(rotation)
			* Translate(center));
	}

	////////////////////////////////////////////////////////////////
	//
	//	AffineTransform
	//
	////////////////////////////////////////////////////////////////

	inline Mat4x4 SIV3D_VECTOR_CALL Mat4x4::AffineTransform(
		const Float3 scale, const Quaternion rotation, const Float3 translation) noexcept
	{
		return Mat4x4{ DirectX::XMMatrixAffineTransformation(
			SimdFloat4{ scale }.vec,
			DirectX::XMVectorZero(),
			rotation.value.vec,
			SimdFloat4{ translation }.vec) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	template <class CharType>
	inline std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Mat4x4& matrix)
	{
		return output << CharType('(')
			<< SimdFloat4{ matrix.value.r[0] } << CharType(',')
			<< SimdFloat4{ matrix.value.r[1] } << CharType(',')
			<< SimdFloat4{ matrix.value.r[2] } << CharType(',')
			<< SimdFloat4{ matrix.value.r[3] } << CharType(')');
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	template <class CharType>
	inline std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Mat4x4& matrix)
	{
		CharType unused;
		Float4 row0, row1, row2, row3;
		input >> unused
			>> row0 >> unused
			>> row1 >> unused
			>> row2 >> unused
			>> row3 >> unused;
		matrix = Mat4x4{ row0, row1, row2, row3 };
		return input;
	}

}
