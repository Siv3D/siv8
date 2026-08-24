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
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL Quaternion::epsilonEquals(const Quaternion other, const float epsilon) const noexcept
	{
		return DirectX::XMVector4NearEqual(value.vec, other.value.vec, DirectX::XMVectorReplicate(epsilon));
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
	//	conjugated
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::conjugated() const noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionConjugate(value.vec) } };
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
	//	Identity
	//
	////////////////////////////////////////////////////////////////

	inline Quaternion SIV3D_VECTOR_CALL Quaternion::Identity() noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionIdentity() } };
	}

}
