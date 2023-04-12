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
	template <std::invocable ExitFunction>
	constexpr ScopeExit<ExitFunction>::ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<ExitFunction>
																				|| std::is_nothrow_copy_constructible_v<ExitFunction>)
		: m_exitFunction{ std::move(other.m_exitFunction) }
		, m_active{ std::exchange(other.m_active, false) } {}
	
	template <std::invocable ExitFunction>
	template <class Fty>
	constexpr ScopeExit<ExitFunction>::ScopeExit(Fty&& exitFunction) noexcept(std::is_nothrow_constructible_v<ExitFunction, Fty>
																				|| std::is_nothrow_constructible_v<ExitFunction, Fty&>)
		: m_exitFunction{ std::forward<ExitFunction>(exitFunction) } {}

	template <std::invocable ExitFunction>
	constexpr ScopeExit<ExitFunction>::~ScopeExit() noexcept(std::is_nothrow_invocable_v<ExitFunction>
		&& std::is_nothrow_destructible_v<ExitFunction>)
	{
		if (m_active)
		{
			std::invoke(m_exitFunction);
		}
	}

	template <std::invocable ExitFunction>
	constexpr void ScopeExit<ExitFunction>::release() noexcept
	{
		m_active = false;
	}
}
