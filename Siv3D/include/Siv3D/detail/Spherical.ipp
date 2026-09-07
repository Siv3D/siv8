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
	constexpr SphericalBase<Float>::SphericalBase(const value_type _r, const value_type _theta, const value_type _phi) noexcept
		: r{ _r }
		, theta{ _theta }
		, phi{ _phi } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Concept::Arithmetic auto _r, const Concept::Arithmetic auto _theta, const Concept::Arithmetic auto _phi) noexcept
		: r{ static_cast<value_type>(_r) }
		, theta{ static_cast<value_type>(_theta) }
		, phi{ static_cast<value_type>(_phi) } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Arg::r_<value_type> _r, const Arg::theta_<value_type> _theta, const Arg::phi_<value_type> _phi) noexcept
		: SphericalBase{ *_r, *_theta, *_phi } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Arg::r_<value_type> _r, const Arg::phi_<value_type> _phi, const Arg::theta_<value_type> _theta) noexcept
		: SphericalBase{ *_r, *_theta, *_phi } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Arg::theta_<value_type> _theta, const Arg::r_<value_type> _r, const Arg::phi_<value_type> _phi) noexcept
		: SphericalBase{ *_r, *_theta, *_phi } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Arg::theta_<value_type> _theta, const Arg::phi_<value_type> _phi, const Arg::r_<value_type> _r) noexcept
		: SphericalBase{ *_r, *_theta, *_phi } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Arg::phi_<value_type> _phi, const Arg::r_<value_type> _r, const Arg::theta_<value_type> _theta) noexcept
		: SphericalBase{ *_r, *_theta, *_phi } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>::SphericalBase(const Arg::phi_<value_type> _phi, const Arg::theta_<value_type> _theta, const Arg::r_<value_type> _r) noexcept
		: SphericalBase{ *_r, *_theta, *_phi } {}

	template <Concept::FloatingPoint Float>
	SphericalBase<Float>::SphericalBase(const position_type target) noexcept
		: r{ target.length() }
		, theta{ (r == 0) ? 0 : std::acos(std::clamp((target.y / r), value_type{ -1 }, value_type{ 1 })) }
		, phi{ ((target.x == 0) && (target.z == 0)) ? 0 : std::atan2(-target.z, target.x) } {}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::operator +() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::operator -() const noexcept
	{
		return{ r, (Math::Pi_v<value_type> - theta), (phi + Math::Pi_v<value_type>) };
	}

	template <Concept::FloatingPoint Float>
	typename SphericalBase<Float>::position_type SphericalBase<Float>::operator +(const position_type v) const noexcept
	{
		return (toPosition() + v);
	}

	template <Concept::FloatingPoint Float>
	typename SphericalBase<Float>::position_type SphericalBase<Float>::operator -(const position_type v) const noexcept
	{
		return (toPosition() - v);
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::withR(const value_type _r) const noexcept
	{
		return{ _r, theta, phi };
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::withTheta(const value_type _theta) const noexcept
	{
		return{ r, _theta, phi };
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::withPhi(const value_type _phi) const noexcept
	{
		return{ r, theta, _phi };
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::withOffsetR(const value_type _r) const noexcept
	{
		return{ (r + _r), theta, phi };
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::withOffsetTheta(const value_type _theta) const noexcept
	{
		return{ r, (theta + _theta), phi };
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::withOffsetPhi(const value_type _phi) const noexcept
	{
		return{ r, theta, (phi + _phi) };
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float> SphericalBase<Float>::rotated(const value_type angle) const noexcept
	{
		return SphericalBase{ *this }.rotate(angle);
	}

	template <Concept::FloatingPoint Float>
	constexpr SphericalBase<Float>& SphericalBase<Float>::rotate(const value_type angle) noexcept
	{
		phi += angle;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	SphericalBase<Float> SphericalBase<Float>::lerp(const SphericalBase& other, const double f) const noexcept
	{
		return{
			static_cast<value_type>(Math::Lerp(r, other.r, f)),
			static_cast<value_type>(Math::Lerp(theta, other.theta, f)),
			static_cast<value_type>(Math::LerpAngle(phi, other.phi, f))
		};
	}

	template <Concept::FloatingPoint Float>
	Float3 SphericalBase<Float>::toFloat3() const noexcept
	{
		const value_type sinTheta = std::sin(theta);
		return{
			static_cast<float>(r * sinTheta * std::cos(phi)),
			static_cast<float>(r * std::cos(theta)),
			static_cast<float>(-r * sinTheta * std::sin(phi))
		};
	}

	template <Concept::FloatingPoint Float>
	Vec3 SphericalBase<Float>::toVec3() const noexcept
	{
		const value_type sinTheta = std::sin(theta);
		return{
			(r * sinTheta * std::cos(phi)),
			(r * std::cos(theta)),
			(-r * sinTheta * std::sin(phi))
		};
	}

	template <Concept::FloatingPoint Float>
	Float3 SphericalBase<Float>::fastToFloat3() const noexcept
	{
		const auto [sinTheta, cosTheta] = FastMath::SinCos(theta);
		const auto [sinPhi, cosPhi] = FastMath::SinCos(phi);
		return{
			static_cast<float>(r * sinTheta * cosPhi),
			static_cast<float>(r * cosTheta),
			static_cast<float>(-r * sinTheta * sinPhi)
		};
	}

	template <Concept::FloatingPoint Float>
	Vec3 SphericalBase<Float>::fastToVec3() const noexcept
	{
		const auto [sinTheta, cosTheta] = FastMath::SinCos(theta);
		const auto [sinPhi, cosPhi] = FastMath::SinCos(phi);
		return{
			(r * sinTheta * cosPhi),
			(r * cosTheta),
			(-r * sinTheta * sinPhi)
		};
	}

	template <Concept::FloatingPoint Float>
	typename SphericalBase<Float>::position_type SphericalBase<Float>::toPosition() const noexcept
	{
		const value_type sinTheta = std::sin(theta);
		return{
			(r * sinTheta * std::cos(phi)),
			(r * std::cos(theta)),
			(-r * sinTheta * std::sin(phi))
		};
	}

	template <Concept::FloatingPoint Float>
	SphericalBase<Float>::operator position_type() const noexcept
	{
		return toPosition();
	}

	template <Concept::FloatingPoint Float>
	uint64 SphericalBase<Float>::hash() const noexcept
	{
		return HashFloats(r, theta, phi);
	}
}
