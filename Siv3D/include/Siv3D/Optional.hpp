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
# include "None.hpp"

namespace s3d
{
	template <class Type>
	using Optional = std::optional<Type>;

	//template <class Type>
	//class Optional
	//{
	//public:

	//	using base_type = std::optional<Type>;

	//	using value_type = Type;

	//	constexpr Optional() noexcept {}

	//	constexpr Optional(None_t) noexcept {}

	//	constexpr Optional(const Optional&) = default;

	//	constexpr Optional(Optional&&) = default;

	//	template <class U>
	//	constexpr explicit(not std::is_convertible_v<const U&, value_type>) Optional(const Optional<U>& other)
	//		: m_data{ other.m_data } {}

	//	template <class U>
	//	constexpr explicit(not std::is_convertible_v<U, value_type>) Optional(Optional<U>&& other)
	//		: m_data{ std::move(other.m_data) } {}

	//	template <class... Types>
	//		requires(std::is_constructible_v<Type, Types...>)
	//	constexpr explicit Optional(std::in_place_t, Types&&... args)
	//		: m_data{ std::in_place, std::forward<Types>(args)... } {}

	//	template <class Elem, class... Types>
	//		requires(std::is_constructible_v<Type, std::initializer_list<Elem>&, Types...>)
	//	constexpr explicit Optional(std::in_place_t, std::initializer_list<Elem> ilist, Types&&... args)
	//		: m_data{ std::in_place, ilist, std::forward<Types>(args)... } {}

	//	template <class U>
	//	using AllowDirectConversion = std::bool_constant<std::conjunction_v<std::negation<std::is_same<std::remove_cvref_t<U>, Optional>>,
	//		std::negation<std::is_same<std::remove_cvref_t<U>, std::in_place_t>>, std::is_constructible<Type, U>>>;

	//	template <class U = Type>
	//		requires(AllowDirectConversion<U>::value)
	//	constexpr explicit(not std::is_convertible_v<U, Type>) Optional(U&& other)
	//		: m_data{ std::in_place, std::forward<U>(other) } {}


	//	constexpr Optional& operator =(None_t) noexcept
	//	{
	//		reset();
	//		return *this;
	//	}

	//	constexpr Optional& operator=(const Optional&) = default;

	//	constexpr Optional& operator=(Optional&&) = default;

	//	template <class U = Type>
	//	constexpr Optional& operator=(U&& value);

	//	template <class U>
	//	constexpr Optional& operator=(const Optional<U>& other);

	//	template <class U>
	//	constexpr Optional& operator=(Optional<U>&& other);



	//	[[nodiscard]]
	//	constexpr const value_type* operator ->() const noexcept
	//	{
	//		return m_data.operator ->();
	//	}

	//	[[nodiscard]]
	//	constexpr value_type* operator ->() noexcept
	//	{
	//		return m_data.operator ->();
	//	}

	//	[[nodiscard]]
	//	constexpr const value_type& operator *() const noexcept
	//	{
	//		return m_data.operator *();
	//	}

	//	[[nodiscard]]
	//	constexpr value_type& operator *() noexcept
	//	{
	//		return m_data.operator *();
	//	}

	//	[[nodiscard]]
	//	constexpr explicit operator bool() const noexcept
	//	{
	//		return m_data.has_value();
	//	}

	//	[[nodiscard]]
	//	constexpr bool has_value() const noexcept
	//	{
	//		return m_data.has_value();
	//	}

	//	[[nodiscard]]
	//	constexpr const value_type& value() const&
	//	{
	//		return m_data.value();
	//	}

	//	[[nodiscard]]
	//	constexpr value_type& value()&
	//	{
	//		return m_data.value();
	//	}

	//	[[nodiscard]]
	//	constexpr value_type&& value()&&
	//	{
	//		return std::move(m_data).value();
	//	}

	//	[[nodiscard]]
	//	constexpr const value_type&& value() const&&
	//	{
	//		return std::move(m_data).value();
	//	}

	//	template <class U>
	//		requires(std::is_convertible_v<U, Type>)
	//	[[nodiscard]]
	//	constexpr value_type value_or(U&& defaultValue) const&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return **this;
	//		}
	//		else
	//		{
	//			return static_cast<value_type>(std::forward<U>(defaultValue));
	//		}
	//	}

	//	template <class U>
	//		requires(std::is_convertible_v<U, Type>)
	//	[[nodiscard]]
	//	constexpr value_type value_or(U&& defaultValue)&&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return std::move(*m_data);
	//		}
	//		else
	//		{
	//			return static_cast<value_type>(std::forward<U>(defaultValue));
	//		}
	//	}


	//	template <class Fty>
	//	constexpr auto and_then(Fty&& f)&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return std::invoke(std::forward<Fty>(f), m_data.value());
	//		}
	//		else
	//		{
	//			return std::remove_cvref_t<std::invoke_result_t<Fty, value_type&>>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto and_then(Fty&& f) const&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return std::invoke(std::forward<Fty>(f), m_data.value());
	//		}
	//		else
	//		{
	//			return std::remove_cvref_t<std::invoke_result_t<Fty, const value_type&>>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto and_then(Fty&& f)&&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return std::invoke(std::forward<Fty>(f), std::move(m_data.value()));
	//		}
	//		else
	//		{
	//			return std::remove_cvref_t<std::invoke_result_t<Fty, value_type>>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto and_then(Fty&& f) const&&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return std::invoke(std::forward<Fty>(f), std::move(m_data.value()));
	//		}
	//		else
	//		{
	//			return std::remove_cvref_t<std::invoke_result_t<Fty, const value_type>>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto transform(Fty&& f)&
	//	{
	//		using Ret = std::remove_cv_t<std::invoke_result_t<Fty, value_type&>>;
	//		
	//		if (m_data.has_value())
	//		{
	//			return Optional<Ret>{ std::invoke(std::forward<Fty>(f), m_data.value()) };
	//		}
	//		else
	//		{
	//			return Optional<Ret>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto transform(Fty&& f) const&
	//	{
	//		using Ret = std::remove_cv_t<std::invoke_result_t<Fty, const value_type&>>;

	//		if (m_data.has_value())
	//		{
	//			return Optional<Ret>{ std::invoke(std::forward<Fty>(f), m_data.value()) };
	//		}
	//		else
	//		{
	//			return Optional<Ret>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto transform(Fty&& f)&&
	//	{
	//		using Ret = std::remove_cv_t<std::invoke_result_t<Fty, value_type>>;

	//		if (m_data.has_value())
	//		{
	//			return Optional<Ret>{ std::invoke(std::forward<Fty>(f), std::move(m_data.value())) };
	//		}
	//		else
	//		{
	//			return Optional<Ret>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr auto transform(Fty&& f) const&&
	//	{
	//		using Ret = std::remove_cv_t<std::invoke_result_t<Fty, const value_type>>;

	//		if (m_data.has_value())
	//		{
	//			return Optional<Ret>{ std::invoke(std::forward<Fty>(f), std::move(m_data.value())) };
	//		}
	//		else
	//		{
	//			return Optional<Ret>{};
	//		}
	//	}

	//	template <class Fty>
	//	constexpr Optional or_else(Fty&& f) const&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return *this;
	//		}
	//		else
	//		{
	//			return std::forward<Fty>(f)();
	//		}
	//	}

	//	template <class Fty>
	//	constexpr Optional or_else(Fty&& f)&&
	//	{
	//		if (m_data.has_value())
	//		{
	//			return std::move(*this);
	//		}
	//		else
	//		{
	//			return std::forward<Fty>(f)();
	//		}
	//	}



	//	constexpr void swap(Optional& other)
	//		noexcept(std::is_nothrow_move_constructible_v<value_type>&& std::is_nothrow_swappable_v<value_type>)
	//	{
	//		m_data.swap(other.m_data);
	//	}

	//	constexpr void reset() noexcept
	//	{
	//		m_data.reset();
	//	}

	//	template <class... Types>
	//	constexpr value_type& emplace(Types&&... args)
	//	{
	//		return m_data.emplace(std::forward<Types>(args)...);
	//	}

	//	template <class Elem, class... Types>
	//		requires(std::is_constructible_v<value_type, std::initializer_list<Elem>&, Types...>)
	//	constexpr value_type& emplace(std::initializer_list<Elem> ilist, Types&&... args)
	//	{
	//		return m_data.emplace(ilist, std::forward<Types>(args)...);
	//	}


	//private:

	//	base_type m_data;
	//};

	//template <class Type1, class Type2>
	//[[nodiscard]]
	//constexpr bool operator ==(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type1, class Type2>
	//[[nodiscard]]
	//constexpr bool operator !=(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type1, class Type2>
	//[[nodiscard]]
	//constexpr bool operator <(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type1, class Type2>
	//[[nodiscard]]
	//constexpr bool operator <=(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type1, class Type2>
	//[[nodiscard]]
	//constexpr bool operator >(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type1, class Type2>
	//[[nodiscard]]
	//constexpr bool operator >=(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type1, std::three_way_comparable_with<Type1> Type2>
	//[[nodiscard]]
	//constexpr std::compare_three_way_result_t<Type1, Type2> operator <=>(const Optional<Type1>& lhs, const Optional<Type2>& rhs);

	//template <class Type>
	//[[nodiscard]]
	//constexpr bool operator ==(const Optional<Type>& opt, None_t) noexcept;

	//template <class Type>
	//[[nodiscard]]
	//constexpr std::strong_ordering operator <=>(const Optional<Type>& opt, None_t) noexcept;

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator ==(const Optional<Type>& opt, const U& value);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator ==(const U& value, const Optional<Type>& opt);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator !=(const Optional<Type>& opt, const U& value);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator !=(const U& value, const Optional<Type>& opt);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator <(const Optional<Type>& opt, const U& value);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator <(const U& value, const Optional<Type>& opt);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator <=(const Optional<Type>& opt, const U& value);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator <=(const U& value, const Optional<Type>& opt);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator >(const U& value, const Optional<Type>& opt);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator >(const Optional<Type>& opt, const U& value);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator >=(const Optional<Type>& opt, const U& value);

	//template <class Type, class U>
	//[[nodiscard]]
	//constexpr bool operator >=(const U& value, const Optional<Type>& opt);

	//namespace detail
	//{
	//	template <class T, template <class...> class Tmp>
	//	inline constexpr bool is_specialization_v = false;

	//	template <template <class...> class Tmp, class... Args>
	//	inline constexpr bool is_specialization_v<Tmp<Args...>, Tmp> = true;
	//}

	//template <class Type, class U>
	//	requires (not detail::is_specialization_v<U, Optional>) && std::three_way_comparable_with<Type, U>
	//[[nodiscard]]
	//constexpr std::compare_three_way_result_t<Type, U> operator <=>(const Optional<Type>& opt, const U& value);

	//template <class Type>
	//[[nodiscard]]
	//constexpr Optional<std::decay_t<Type>> MakeOptional(Type&& v);

	//template <class Type, class... Args>
	//[[nodiscard]]
	//constexpr Optional<Type> MakeOptional(Args&&... args);

	//template< class Type, class U, class... Args >
	//[[nodiscard]]
	//constexpr Optional<Type> MakeOptional(std::initializer_list<U> il, Args&&... args);

	//template <class Type>
	//constexpr void swap(Optional<Type>& a, Optional<Type>& b) noexcept(noexcept(a.swap(b)));

	//template <class Type>
	//Optional(Type)->Optional<Type>;
}

//template <class Type>
//struct std::hash<s3d::Optional<Type>>
//{
//	[[nodiscard]]
//	size_t operator ()(const s3d::Optional<Type>& value) const noexcept
//	{
//		if (value)
//		{
//			return std::hash<Type>{}(*value);
//		}
//		else
//		{
//			return 0;
//		}
//	}
//};
