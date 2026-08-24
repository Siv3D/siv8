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

	inline Quaternion::Quaternion(const float x, const float y, const float z, const float w) noexcept
		: value{ x, y, z, w } {}

	inline Quaternion::Quaternion(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y,
		const Concept::Arithmetic auto z, const Concept::Arithmetic auto w) noexcept
		: Quaternion{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) } {}

	inline Quaternion::Quaternion(const Float4 xyzw) noexcept
		: value{ xyzw } {}

	inline Quaternion::Quaternion(const SimdFloat4 xyzw) noexcept
		: value{ xyzw } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::operator *(const Quaternion other) const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionMultiply(value.vec, other.value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion& SIV3D_VECTOR_CALL Quaternion::operator *=(const Quaternion other) & noexcept
	{
		value.vec = DirectX::XMQuaternionMultiply(value.vec, other.value.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Quaternion::epsilonEquals(const Quaternion other, const float epsilon) const noexcept
	{
		return DirectX::XMVector4NearEqual(value.vec, other.value.vec, DirectX::XMVectorReplicate(epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotationEquals
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Quaternion::rotationEquals(const Quaternion other, const float epsilon) const noexcept
	{
		const DirectX::XMVECTOR absoluteDot = DirectX::XMVectorAbs(DirectX::XMQuaternionDot(value.vec, other.value.vec));
		return DirectX::XMVector4NearEqual(absoluteDot, DirectX::XMVectorSplatOne(), DirectX::XMVectorReplicate(epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	angleTo
	//
	////////////////////////////////////////////////////////////////

	inline float SIV3D_VECTOR_CALL Quaternion::angleTo(const Quaternion other) const noexcept
	{
		DirectX::XMVECTOR cosine = DirectX::XMVectorAbs(DirectX::XMQuaternionDot(value.vec, other.value.vec));
		cosine = DirectX::XMVectorClamp(cosine, DirectX::XMVectorZero(), DirectX::XMVectorSplatOne());
		return (2.0f * DirectX::XMVectorGetX(DirectX::XMVectorACos(cosine)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat4
	//
	////////////////////////////////////////////////////////////////

	inline Float4 Quaternion::toFloat4() const noexcept
	{
		return value.toFloat4();
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyz
	//
	////////////////////////////////////////////////////////////////

	inline Float3 Quaternion::xyz() const noexcept
	{
		return value.xyz();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getX, getY, getZ, getW
	//
	////////////////////////////////////////////////////////////////

	inline float Quaternion::getX() const noexcept
	{
		return DirectX::XMVectorGetX(value.vec);
	}

	inline float Quaternion::getY() const noexcept
	{
		return DirectX::XMVectorGetY(value.vec);
	}

	inline float Quaternion::getZ() const noexcept
	{
		return DirectX::XMVectorGetZ(value.vec);
	}

	inline float Quaternion::getW() const noexcept
	{
		return DirectX::XMVectorGetW(value.vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	dot
	//
	////////////////////////////////////////////////////////////////

	inline float SIV3D_VECTOR_CALL Quaternion::dot(const Quaternion other) const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(value.vec, other.value.vec));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	inline float Quaternion::length() const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(value.vec));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	inline float Quaternion::lengthSq() const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(value.vec));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNaN
	//
	////////////////////////////////////////////////////////////////

	inline bool Quaternion::hasNaN() const noexcept
	{
		return DirectX::XMQuaternionIsNaN(value.vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasInf
	//
	////////////////////////////////////////////////////////////////

	inline bool Quaternion::hasInf() const noexcept
	{
		return DirectX::XMQuaternionIsInfinite(value.vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isFinite
	//
	////////////////////////////////////////////////////////////////

	inline bool Quaternion::isFinite() const noexcept
	{
		return (not hasNaN() && not hasInf());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isIdentity
	//
	////////////////////////////////////////////////////////////////

	inline bool Quaternion::isIdentity() const noexcept
	{
		return DirectX::XMQuaternionIsIdentity(value.vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNormalized
	//
	////////////////////////////////////////////////////////////////

	inline bool Quaternion::isNormalized(const float epsilon) const noexcept
	{
		return DirectX::XMVector4NearEqual(
			DirectX::XMQuaternionLengthSq(value.vec),
			DirectX::XMVectorSplatOne(),
			DirectX::XMVectorReplicate(epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalize
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion& SIV3D_VECTOR_CALL Quaternion::normalize() noexcept
	{
		value.vec = DirectX::XMQuaternionNormalize(value.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalized
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::normalized() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionNormalize(value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	fastNormalized
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::fastNormalized() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionNormalizeEst(value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	canonicalized
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::canonicalized() const noexcept
	{
		Quaternion result = *this;
		return result.canonicalize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	conjugated
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::conjugated() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionConjugate(value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	conjugate
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion& SIV3D_VECTOR_CALL Quaternion::conjugate() & noexcept
	{
		value.vec = DirectX::XMQuaternionConjugate(value.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	inverse
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::inverse() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionInverse(value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	inline Float3 SIV3D_VECTOR_CALL Quaternion::rotate(const Float3 v) const noexcept
	{
		return SimdFloat4{ DirectX::XMVector3Rotate(SimdFloat4{ v, 0.0f }.vec, value.vec) }.xyz();
	}

	////////////////////////////////////////////////////////////////
	//
	//	inverseRotate
	//
	////////////////////////////////////////////////////////////////

	inline Float3 SIV3D_VECTOR_CALL Quaternion::inverseRotate(const Float3 v) const noexcept
	{
		return SimdFloat4{ DirectX::XMVector3InverseRotate(SimdFloat4{ v, 0.0f }.vec, value.vec) }.xyz();
	}

	////////////////////////////////////////////////////////////////
	//
	//	slerp
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::slerp(const Quaternion other, const float t) const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionSlerp(value.vec, other.value.vec, t) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	nlerp
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::nlerp(const Quaternion other, const float t) const noexcept
	{
		const DirectX::XMVECTOR dotValue = DirectX::XMQuaternionDot(value.vec, other.value.vec);
		const DirectX::XMVECTOR adjustedOther = DirectX::XMVectorSelect(
			other.value.vec,
			DirectX::XMVectorNegate(other.value.vec),
			DirectX::XMVectorLess(dotValue, DirectX::XMVectorZero()));
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(value.vec, adjustedOther, t)) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	log
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::log() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionLn(value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	exp
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::exp() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionExp(value.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toAxisAngle
	//
	////////////////////////////////////////////////////////////////

	inline std::pair<Float3, float> SIV3D_VECTOR_CALL Quaternion::toAxisAngle() const noexcept
	{
		SimdFloat4 axis;
		float angle;
		DirectX::XMQuaternionToAxisAngle(&axis.vec, &angle, value.vec);
		axis.vec = DirectX::XMVector3Normalize(axis.vec);
		return{ axis.xyz(), angle };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Identity
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::Identity() noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionIdentity() } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RollPitchYaw
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RollPitchYaw(const float pitch, const float yaw, const float roll) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll) } };
	}

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RollPitchYaw(const Float3 pitchYawRoll) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationRollPitchYawFromVector(SimdFloat4{ pitchYawRoll, 0.0f }.vec) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RotationNormal
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RotationNormal(const Float3 normalAxis, const float angle) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationNormal(SimdFloat4{ normalAxis, 0.0f }.vec, angle) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RotationAxis
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RotationAxis(const Float3 axis, const float angle) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationAxis(SimdFloat4{ axis, 0.0f }.vec, angle) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RotateX, RotateY, RotateZ
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RotateX(const float angle) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationNormal(DirectX::g_XMIdentityR0.v, angle) } };
	}

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RotateY(const float angle) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationNormal(DirectX::g_XMIdentityR1.v, angle) } };
	}

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::RotateZ(const float angle) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationNormal(DirectX::g_XMIdentityR2.v, angle) } };
	}

}
