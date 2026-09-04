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
	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const value_type _r, const value_type _phi, const value_type _y) noexcept
		: r{ _r }
		, phi{ _phi }
		, y{ _y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Concept::Arithmetic auto _r, const Concept::Arithmetic auto _phi, const Concept::Arithmetic auto _y) noexcept
		: r{ static_cast<value_type>(_r) }
		, phi{ static_cast<value_type>(_phi) }
		, y{ static_cast<value_type>(_y) } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Arg::r_<value_type> _r, const Arg::phi_<value_type> _phi, const Arg::y_<value_type> _y) noexcept
		: CylindricalBase{ *_r, *_phi, *_y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Arg::r_<value_type> _r, const Arg::y_<value_type> _y, const Arg::phi_<value_type> _phi) noexcept
		: CylindricalBase{ *_r, *_phi, *_y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Arg::phi_<value_type> _phi, const Arg::r_<value_type> _r, const Arg::y_<value_type> _y) noexcept
		: CylindricalBase{ *_r, *_phi, *_y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Arg::phi_<value_type> _phi, const Arg::y_<value_type> _y, const Arg::r_<value_type> _r) noexcept
		: CylindricalBase{ *_r, *_phi, *_y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Arg::y_<value_type> _y, const Arg::r_<value_type> _r, const Arg::phi_<value_type> _phi) noexcept
		: CylindricalBase{ *_r, *_phi, *_y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>::CylindricalBase(const Arg::y_<value_type> _y, const Arg::phi_<value_type> _phi, const Arg::r_<value_type> _r) noexcept
		: CylindricalBase{ *_r, *_phi, *_y } {}

	template <Concept::FloatingPoint Float>
	CylindricalBase<Float>::CylindricalBase(const position_type target) noexcept
		: r{ target.xz().length() }
		, phi{ (r == 0) ? 0 : std::atan2(-target.z, target.x) }
		, y{ target.y } {}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::operator +() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::operator -() const noexcept
	{
		return{ r, (phi + Math::Pi_v<value_type>), -y };
	}

	template <Concept::FloatingPoint Float>
	typename CylindricalBase<Float>::position_type CylindricalBase<Float>::operator +(const position_type v) const noexcept
	{
		return (toPosition() + v);
	}

	template <Concept::FloatingPoint Float>
	typename CylindricalBase<Float>::position_type CylindricalBase<Float>::operator -(const position_type v) const noexcept
	{
		return (toPosition() - v);
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::withR(const value_type _r) const noexcept
	{
		return{ _r, phi, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::withPhi(const value_type _phi) const noexcept
	{
		return{ r, _phi, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::withY(const value_type _y) const noexcept
	{
		return{ r, phi, _y };
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::withOffsetR(const value_type _r) const noexcept
	{
		return{ (r + _r), phi, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::withOffsetPhi(const value_type _phi) const noexcept
	{
		return{ r, (phi + _phi), y };
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::withOffsetY(const value_type _y) const noexcept
	{
		return{ r, phi, (y + _y) };
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float> CylindricalBase<Float>::rotated(const value_type angle) const noexcept
	{
		return CylindricalBase{ *this }.rotate(angle);
	}

	template <Concept::FloatingPoint Float>
	constexpr CylindricalBase<Float>& CylindricalBase<Float>::rotate(const value_type angle) noexcept
	{
		phi += angle;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	CylindricalBase<Float> CylindricalBase<Float>::lerp(const CylindricalBase& other, const double f) const noexcept
	{
		return{
			static_cast<value_type>(Math::Lerp(r, other.r, f)),
			static_cast<value_type>(Math::LerpAngle(phi, other.phi, f)),
			static_cast<value_type>(Math::Lerp(y, other.y, f))
		};
	}

	template <Concept::FloatingPoint Float>
	Float3 CylindricalBase<Float>::toFloat3() const noexcept
	{
		return{ static_cast<float>(r * std::cos(phi)), static_cast<float>(y), static_cast<float>(-r * std::sin(phi)) };
	}

	template <Concept::FloatingPoint Float>
	Vec3 CylindricalBase<Float>::toVec3() const noexcept
	{
		return{ (r * std::cos(phi)), y, (-r * std::sin(phi)) };
	}

	template <Concept::FloatingPoint Float>
	Float3 CylindricalBase<Float>::fastToFloat3() const noexcept
	{
		const auto [s, c] = FastMath::SinCos(phi);
		return{ static_cast<float>(r * c), static_cast<float>(y), static_cast<float>(-r * s) };
	}

	template <Concept::FloatingPoint Float>
	Vec3 CylindricalBase<Float>::fastToVec3() const noexcept
	{
		const auto [s, c] = FastMath::SinCos(phi);
		return{ (r * c), y, (-r * s) };
	}

	template <Concept::FloatingPoint Float>
	typename CylindricalBase<Float>::position_type CylindricalBase<Float>::toPosition() const noexcept
	{
		return{ (r * std::cos(phi)), y, (-r * std::sin(phi)) };
	}

	template <Concept::FloatingPoint Float>
	CylindricalBase<Float>::operator position_type() const noexcept
	{
		return toPosition();
	}

	template <Concept::FloatingPoint Float>
	uint64 CylindricalBase<Float>::hash() const noexcept
	{
		return HashFloats(r, phi, y);
	}
}
