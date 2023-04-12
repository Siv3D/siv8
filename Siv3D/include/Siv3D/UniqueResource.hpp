//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <utility>
# include <type_traits>

namespace s3d
{
	template <class Resource, class Deleter>
	class UniqueResource
	{
	public:

		UniqueResource(const UniqueResource&) = delete;

		UniqueResource& operator =(const UniqueResource&) = delete;

		[[nodiscard]]
		UniqueResource() = default;

		[[nodiscard]]
		explicit UniqueResource(Resource&& resource, Deleter&& deleter) noexcept;

		[[nodiscard]]
		UniqueResource(UniqueResource&& other) noexcept;

		~UniqueResource();

		UniqueResource& operator =(UniqueResource&& other);

		void reset();

		void reset(Resource&& other);

		void release() noexcept;

		[[nodiscard]]
		const Resource& get() const noexcept;

		[[nodiscard]]
		const Deleter& get_deleter() const noexcept;

		[[nodiscard]]
		std::add_lvalue_reference_t<std::remove_pointer_t<Resource>> operator *() const noexcept;

		[[nodiscard]]
		Resource operator ->() const noexcept;

	private:

		Resource m_resource;

		[[no_unique_address]] Deleter m_deleter;

		bool m_active = true;
	};

	template <class Resource, class Deleter>
	UniqueResource(Resource, Deleter) -> UniqueResource<Resource, Deleter>;

	template <class Resource, class Deleter>
	[[nodiscard]]
	UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> MakeUniqueResource(Resource&& r, Deleter&& d) noexcept;

	template <class Resource, class Deleter, class Invalid = std::decay_t<Resource>>
	[[nodiscard]]
	UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>> MakeUniqueResourceChecked(Resource&& r, const Invalid& invalid, Deleter&& d) noexcept;
}

# include "detail/UniqueResource.ipp"
