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
	bool ConstantBuffer<Type>::isDirty() const noexcept
	{
		return m_isDirty;
	}

	template <class Type>
	inline Type& ConstantBuffer<Type>::operator *() noexcept
	{
		m_isDirty = true;
		return m_wrapper->data;
	}

	template <class Type>
	inline const Type& ConstantBuffer<Type>::operator *() const noexcept
	{
		return m_wrapper->data;
	}

	template <class Type>
	inline Type* ConstantBuffer<Type>::operator ->() noexcept
	{
		m_isDirty = true;
		return std::addressof(m_wrapper->data);
	}

	template <class Type>
	inline const Type* ConstantBuffer<Type>::operator ->() const noexcept
	{
		return std::addressof(m_wrapper->data);
	}

	template <class Type>
	IConstantBuffer* ConstantBuffer<Type>::_base() const noexcept
	{
		if (not m_base->_internal_init())
		{
			return nullptr;
		}

		return m_base.get();
	}

	template <class Type>
	bool ConstantBuffer<Type>::_update_if_dirty()
	{
		if (not m_isDirty)
		{
			return false;
		}

		if (const bool result = m_base->_internal_update(std::addressof(m_wrapper->data), Size))
		{
			m_isDirty = false;
			return true;
		}
		else
		{
			return false;
		}
	}
}
