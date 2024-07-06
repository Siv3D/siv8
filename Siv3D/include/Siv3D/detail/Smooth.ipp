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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Smooth<Type>::Smooth(const value_type& value) noexcept
		: m_value{ value }
		, m_target{ value } {}

	template <class Type>
	template <class... Args>
		requires std::is_constructible_v<Type, Args...>
	constexpr Smooth<Type>::Smooth(Args&&... args) noexcept
		: m_value(std::forward<Args>(args)...)
		, m_target{ m_value } {}

	////////////////////////////////////////////////////////////////
	//
	//	setState
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Smooth<Type>& Smooth<Type>::setState(const value_type& value, const value_type& velocity) noexcept
	{
		m_value		= value;
		m_target	= value;
		m_velocity	= velocity;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTarget
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Smooth<Type>& Smooth<Type>::setTarget(const value_type& target) noexcept
	{
		m_target = target;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearTarget
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Smooth<Type>& Smooth<Type>::clearTarget() noexcept
	{
		m_target	= m_value;
		m_velocity	= value_type{};
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	jumpTo
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Smooth<Type>& Smooth<Type>::jumpTo(const value_type& target) noexcept
	{
		m_value		= target;
		m_target	= target;
		m_velocity	= value_type{};
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	jumpToTarget
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Smooth<Type>& Smooth<Type>::jumpToTarget() noexcept
	{
		m_value		= m_target;
		m_velocity	= value_type{};
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	const Smooth<Type>::value_type& Smooth<Type>::update(const Duration& smoothTime, const Optional<double>& maxSpeed, const double deltaTime) noexcept
	{
		return (m_value = Math::SmoothDamp(m_value, m_target, m_velocity, smoothTime.count(), maxSpeed, deltaTime));
	}

	template <class Type>
	const Smooth<Type>::value_type& Smooth<Type>::update(const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime) noexcept
	{
		return (m_value = Math::SmoothDamp(m_value, m_target, m_velocity, smoothTime, maxSpeed, deltaTime));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr const Smooth<Type>::value_type& Smooth<Type>::operator *() const noexcept
	{
		return m_value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	value
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr const Smooth<Type>::value_type& Smooth<Type>::value() const noexcept
	{
		return m_value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	target
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr const Smooth<Type>::value_type& Smooth<Type>::target() const noexcept
	{
		return m_target;
	}

	////////////////////////////////////////////////////////////////
	//
	//	velocity
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr const Smooth<Type>::value_type& Smooth<Type>::velocity() const noexcept
	{
		return m_velocity;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ->
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr const Smooth<Type>::value_type* Smooth<Type>::operator ->() const noexcept
	{
		return std::addressof(m_value);
	}
}
