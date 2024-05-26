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
	template <class Type>
	ConstantBuffer<Type>::ConstantBuffer()
		: m_base{ IConstantBuffer::Create(Size) } {}

	template <class Type>
	ConstantBuffer<Type>::ConstantBuffer(const Type& data)
		: ConstantBuffer{}
	{
		m_wrapper->data = data;
	}

	template <class Type>
	template <class... Args>
		requires std::is_constructible_v<Type, Args...>
	ConstantBuffer<Type>::ConstantBuffer(Args&&... args)
		: ConstantBuffer{}
	{
		m_wrapper->data = Type(std::forward<Args>(args)...);
	}

	template <class Type>
	ConstantBuffer<Type>& ConstantBuffer<Type>::operator =(const Type& data)
	{
		m_wrapper->data = data;
		m_isDirty = true;
		return *this;
	}

	template <class Type>
	constexpr size_t ConstantBuffer<Type>::size() const noexcept
	{
		return Size;
	}

	template <class Type>
	const float* ConstantBuffer<Type>::data() const noexcept
	{
		return static_cast<const float*>(static_cast<const void*>(std::addressof(m_wrapper->data)));
	}

	template <class Type>
	Type& ConstantBuffer<Type>::get() noexcept
	{
		m_isDirty = true;
		return m_wrapper->data;
	}

	template <class Type>
	const Type& ConstantBuffer<Type>::get() const noexcept
	{
		return m_wrapper->data;
	}

	template <class Type>
	bool ConstantBuffer<Type>::isDirty() const noexcept
	{
		return m_isDirty;
	}

	//template <class Type>
	//bool ConstantBuffer<Type>::_update_if_dirty()
	//{
	//	if (not m_isDirty)
	//	{
	//		return false;
	//	}

	//	m_isDirty = false;

	//	return m_base._internal_update(data(), Size);
	//}
}
