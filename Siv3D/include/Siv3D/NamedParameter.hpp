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
# include <memory>		// std::addressof
# include <tuple>		// std::make_from_tuple
# include <concepts>	// std::convertible_to, std::constructible_from
# include <type_traits>	// std::decay_t
# include <utility>		// std::forward

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	NamedParameter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 名前付き変数作成クラス | Named Parameter Class
	/// @tparam Tag タグ名 | Tag name
	/// @tparam Type 変数の値の型 | Type of the value of the variable
	template <class Tag, class Type>
	class NamedParameter
	{
	public:

		[[nodiscard]]
		NamedParameter() = default;

		[[nodiscard]]
		constexpr NamedParameter(const Type& value) noexcept(std::is_nothrow_copy_constructible_v<Type>)
			: m_value{ value } {}

		[[nodiscard]]
		constexpr NamedParameter(Type&& value) noexcept(std::is_nothrow_move_constructible_v<Type>)
			: m_value{ std::move(value) } {}

		template <class U>
			requires std::convertible_to<U, Type>
		[[nodiscard]]
		constexpr NamedParameter(const NamedParameter<Tag, U>& other)
			: m_value{ static_cast<Type>(other.value()) } {}

		template <class... Args>
			requires std::constructible_from<Type, Args...>
		[[nodiscard]]
		constexpr NamedParameter(const NamedParameter<Tag, std::tuple<Args...>>& tuple)
			: m_value{ std::make_from_tuple<Type>(tuple.value()) } {}

		[[nodiscard]]
		constexpr const Type* operator ->() const noexcept
		{
			return std::addressof(m_value);
		}

		[[nodiscard]]
		constexpr const Type& operator *() const noexcept
		{
			return m_value;
		}

		[[nodiscard]]
		constexpr const Type& value() const noexcept
		{
			return m_value;
		}

	private:

		Type m_value{};
	};

	////////////////////////////////////////////////////////////////
	//
	//	NamedParameterHelper
	//
	////////////////////////////////////////////////////////////////

	template <class Tag>
	struct NamedParameterHelper
	{
		template <class Type>
		using named_argument_type = NamedParameter<Tag, Type>;

		template <class Type>
		[[nodiscard]]
		constexpr named_argument_type<std::decay_t<Type>> operator =(Type&& value) const
		{
			return{ std::forward<Type>(value) };
		}

		template <class... Args>
		[[nodiscard]]
		constexpr named_argument_type<std::tuple<std::decay_t<Args>...>> operator ()(Args&&... args) const
		{
			return{ std::make_tuple(std::forward<Args>(args)...) };
		}
	};
}

# define SIV3D_NAMED_PARAMETER(name) \
inline constexpr auto name = s3d::NamedParameterHelper<struct name##_tag>{};\
template <class Type> using name##_ = s3d::NamedParameter<struct name##_tag, Type>
