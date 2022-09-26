//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	template <class ExitFunction>
		requires std::invocable<ExitFunction>
	constexpr ScopeGuard<ExitFunction>::ScopeGuard(ScopeGuard&& other) noexcept(std::is_nothrow_move_constructible_v<ExitFunction>
																				|| std::is_nothrow_copy_constructible_v<ExitFunction>)
		: m_exitFunction{ std::forward<ExitFunction>(other.m_callback) }
		, m_active{ other.m_active }
	{
		other.release();
	}
	
	template <class ExitFunction>
		requires std::invocable<ExitFunction>
	template <class Fty>
	constexpr ScopeGuard<ExitFunction>::ScopeGuard(Fty&& exitFunction) noexcept(std::is_nothrow_constructible_v<ExitFunction, Fty>
																				|| std::is_nothrow_constructible_v<ExitFunction, Fty&>)
		: m_exitFunction{ std::forward<ExitFunction>(exitFunction) } {}

	template <class ExitFunction>
		requires std::invocable<ExitFunction>
	constexpr ScopeGuard<ExitFunction>::~ScopeGuard() noexcept(std::is_nothrow_invocable_v<ExitFunction>
		&& std::is_nothrow_destructible_v<ExitFunction>)
	{
		if (m_active)
		{
			m_exitFunction();
		}
	}

	template <class ExitFunction>
		requires std::invocable<ExitFunction>
	constexpr void ScopeGuard<ExitFunction>::release() noexcept
	{
		m_active = false;
	}
}
