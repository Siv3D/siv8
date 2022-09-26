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
# include <concepts>

namespace s3d
{
	template <class ExitFunction>
		requires std::invocable<ExitFunction>
	class ScopeGuard final
	{
	public:

		ScopeGuard(const ScopeGuard&) = delete;

		ScopeGuard& operator =(const ScopeGuard&) = delete;

		[[nodiscard]]
		constexpr ScopeGuard(ScopeGuard&& other) noexcept(std::is_nothrow_move_constructible_v<ExitFunction>
															|| std::is_nothrow_copy_constructible_v<ExitFunction>);
		template <class Fty>
		[[nodiscard]]
		constexpr ScopeGuard(Fty&& exitFunction) noexcept(std::is_nothrow_constructible_v<ExitFunction, Fty>
															|| std::is_nothrow_constructible_v<ExitFunction, Fty&>);

		constexpr ~ScopeGuard() noexcept(std::is_nothrow_invocable_v<ExitFunction>
											&& std::is_nothrow_destructible_v<ExitFunction>);
	
		constexpr void release() noexcept;

	private:

		ExitFunction m_exitFunction;

		bool m_active = true;
	};

	template <class ExitFunction>
	ScopeGuard(ExitFunction) -> ScopeGuard<ExitFunction>;
}

# include "detail/ScopeGuard.ipp"
