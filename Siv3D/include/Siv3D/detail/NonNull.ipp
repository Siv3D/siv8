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
	template <class Pointer>
	constexpr NonNull<Pointer>::NonNull(const Pointer pointer) noexcept
		: m_pointer{ pointer }
	{
		assert(m_pointer != nullptr);
	}

	template <class Pointer>
	constexpr NonNull<Pointer>::operator Pointer() const noexcept
	{
		return get();
	}

	template <class Pointer>
	constexpr std::conditional_t<std::is_copy_constructible_v<Pointer>, Pointer, const Pointer&> NonNull<Pointer>::get() const noexcept
	{
		return m_pointer;
	}
}
