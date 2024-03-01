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
# include <concepts>
# include <functional>
# include <type_traits>
# include <utility>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ScopeExit
	//
	////////////////////////////////////////////////////////////////

	/// @brief スコープを抜けるときに指定した関数を実行するオブジェクト | An object that executes a specified function when it leaves the scope
	/// @tparam ExitFunction 実行する関数の型 | The type of the function to execute
	template <std::invocable ExitFunction>
	class ScopeExit final
	{
	public:

		ScopeExit(const ScopeExit&) = delete;

		ScopeExit& operator =(const ScopeExit&) = delete;

		[[nodiscard]]
		constexpr ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<ExitFunction>
			|| std::is_nothrow_copy_constructible_v<ExitFunction>);

		template <class Fty>
		[[nodiscard]]
		constexpr ScopeExit(Fty&& exitFunction) noexcept(std::is_nothrow_constructible_v<ExitFunction, Fty>
			|| std::is_nothrow_constructible_v<ExitFunction, Fty&>);

		constexpr ~ScopeExit() noexcept(std::is_nothrow_invocable_v<ExitFunction>
			&& std::is_nothrow_destructible_v<ExitFunction>);

		/// @brief ScopeExit を非アクティブにします。 | Deactivates the ScopeExit.
		constexpr void release() noexcept;

	private:

		[[no_unique_address]] ExitFunction m_exitFunction;

		bool m_active = true;
	};

	template <std::invocable ExitFunction>
	ScopeExit(ExitFunction) -> ScopeExit<ExitFunction>;
}

# include "detail/ScopeExit.ipp"
