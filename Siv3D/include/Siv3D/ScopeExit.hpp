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
	namespace detail
	{
		template <class T>
		concept ExitFunction = std::invocable<std::add_lvalue_reference_t<T>> && (not std::is_rvalue_reference_v<T>);

		template <class T, class U>
		concept not_same_as_impl = (not std::is_same_v<T, U>);
		template <class T, class U>
		concept not_same_as = (not_same_as_impl<T, U> && not_same_as_impl<U, T>);
	}

	////////////////////////////////////////////////////////////////
	//
	//	ScopeExit
	//
	////////////////////////////////////////////////////////////////

	/// @brief スコープを抜けるときに指定した関数を実行するオブジェクト | An object that executes a specified function when it leaves the scope
	/// @tparam ExitFunction 実行する関数の型 | The type of the function to execute
	template <detail::ExitFunction ExitFunction>
	class ScopeExit final
	{
	public:

		ScopeExit(const ScopeExit&) = delete;

		ScopeExit& operator =(const ScopeExit&) = delete;

		[[nodiscard]]
		constexpr ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<ExitFunction>
			|| std::is_nothrow_copy_constructible_v<ExitFunction>);

		template <class Fty>
			requires detail::not_same_as<std::remove_cvref_t<Fty>, ScopeExit<ExitFunction>>
		[[nodiscard]]
		constexpr ScopeExit(Fty&& exitFunction) noexcept(std::is_nothrow_constructible_v<ExitFunction, Fty>
			|| std::is_nothrow_constructible_v<ExitFunction, Fty&>);

		constexpr ~ScopeExit() noexcept(std::is_nothrow_invocable_v<ExitFunction&>
			&& std::is_nothrow_destructible_v<ExitFunction>);

		/// @brief ScopeExit を非アクティブにします。 | Deactivates the ScopeExit.
		constexpr void release() noexcept;

	private:

		[[no_unique_address]] ExitFunction m_exitFunction;

		bool m_active = true;
	};

	template <detail::ExitFunction ExitFunction>
	ScopeExit(ExitFunction) -> ScopeExit<ExitFunction>;
}

# include "detail/ScopeExit.ipp"
