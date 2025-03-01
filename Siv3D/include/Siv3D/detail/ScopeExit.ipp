//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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

	template <detail::ExitFunction ExitFunction>
	constexpr ScopeExit<ExitFunction>::ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_copy_constructible_v<ExitFunction>)
		: m_exitFunction{ other.m_exitFunction }
		, m_active{ std::exchange(other.m_active, false) } {}

	template <detail::ExitFunction ExitFunction>
	constexpr ScopeExit<ExitFunction>::ScopeExit(ScopeExit&& other) noexcept
		requires std::is_nothrow_move_constructible_v<ExitFunction>
		: m_exitFunction{ std::forward<ExitFunction>(other.m_exitFunction) }
		, m_active{ std::exchange(other.m_active, false) } {}

	template <detail::ExitFunction ExitFunction>
	template <class Fty>
	constexpr ScopeExit<ExitFunction>::ScopeExit(Fty&& exitFunction) noexcept(std::is_nothrow_constructible_v<ExitFunction, Fty&>)
		requires detail::not_same_as<std::remove_cvref_t<Fty>, ScopeExit>
		: m_exitFunction{ exitFunction } {}

	template <detail::ExitFunction ExitFunction>
	template <class Fty>
	constexpr ScopeExit<ExitFunction>::ScopeExit(Fty&& exitFunction) noexcept
		requires (detail::not_same_as<std::remove_cvref_t<Fty>, ScopeExit> && (not std::is_lvalue_reference_v<Fty>) && std::is_nothrow_constructible_v<ExitFunction, Fty>)
		: m_exitFunction{ std::forward<Fty>(exitFunction) } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	template <detail::ExitFunction ExitFunction>
	constexpr ScopeExit<ExitFunction>::~ScopeExit() noexcept(std::is_nothrow_invocable_v<ExitFunction&>
		&& std::is_nothrow_destructible_v<ExitFunction>)
	{
		if (m_active)
		{
			std::invoke(m_exitFunction);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	template <detail::ExitFunction ExitFunction>
	constexpr void ScopeExit<ExitFunction>::release() noexcept
	{
		m_active = false;
	}
}
