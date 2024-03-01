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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline UniqueResource<Resource, Deleter>::UniqueResource(Resource&& resource, Deleter&& deleter) noexcept
		: m_resource{ std::forward<Resource>(resource) }
		, m_deleter{ std::forward<Deleter>(deleter) } {}

	template <class Resource, class Deleter>
	inline UniqueResource<Resource, Deleter>::UniqueResource(UniqueResource&& other) noexcept
		: m_resource{ std::exchange(other.m_resource, {}) }
		, m_deleter{ std::move(other.m_deleter) }
		, m_active{ std::exchange(other.m_active, false) } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline UniqueResource<Resource, Deleter>::~UniqueResource()
	{
		reset();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline UniqueResource<Resource, Deleter>& UniqueResource<Resource, Deleter>::operator =(UniqueResource&& other)
	{
		reset();
		m_resource = std::exchange(other.m_resource, {});
		m_deleter = std::move(other.m_deleter);
		m_active = std::exchange(other.m_active, false);
		return *this;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline void UniqueResource<Resource, Deleter>::reset()
	{
		if (m_active)
		{
			std::invoke(get_deleter(), get());
			m_active = false;
		}
	}

	template <class Resource, class Deleter>
	inline void UniqueResource<Resource, Deleter>::reset(Resource&& other)
	{
		reset();
		m_resource = std::move(other);
		m_active = true;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline void UniqueResource<Resource, Deleter>::release() noexcept
	{
		m_active = false;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline const Resource& UniqueResource<Resource, Deleter>::get() const noexcept
	{
		return m_resource;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	get_deleter
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline const Deleter& UniqueResource<Resource, Deleter>::get_deleter() const noexcept
	{
		return m_deleter;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline std::add_lvalue_reference_t<std::remove_pointer_t<Resource>> UniqueResource<Resource, Deleter>::operator *() const noexcept
	{
		return *m_resource;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator ->
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline Resource UniqueResource<Resource, Deleter>::operator ->() const noexcept
	{
		return m_resource;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	MakeUniqueResource
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter>
	inline UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> MakeUniqueResource(Resource&& r, Deleter&& d) noexcept
	{
		return{ std::forward<Resource>(r), std::forward<Deleter>(d) };
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	MakeUniqueResourceChecked
	//
	////////////////////////////////////////////////////////////////

	template <class Resource, class Deleter, class Invalid>
	inline UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> MakeUniqueResourceChecked(Resource&& r, const Invalid& invalid, Deleter&& d) noexcept
	{
		UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> ur{ std::forward<Resource>(r), std::forward<Deleter>(d) };

		if (static_cast<bool>(ur.get() == invalid))
		{
			ur.release();
		}

		return ur;
	}
}
