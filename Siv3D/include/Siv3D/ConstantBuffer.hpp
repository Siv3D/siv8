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
# include <memory>
# include "Common.hpp"
# include "Memory.hpp"
# include "Utility.hpp"
# include "IConstantBuffer.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ConstantBuffer
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	class ConstantBuffer
	{
	public:

		static_assert(sizeof(Type) <= (16 * 4096)); // <= 64KB

		static_assert(std::is_trivially_copyable_v<Type>);

		static constexpr size_t _alignment = Max<size_t>(alignof(Type), 16);

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4324)

		struct alignas(_alignment) WrapperType
		{
			Type data;
		};

	SIV3D_DISABLE_MSVC_WARNINGS_POP()

		static constexpr size_t Size		= sizeof(WrapperType);

		static constexpr size_t NumVectors	= (Size / 16);

		static constexpr size_t Alignment	= alignof(WrapperType);

		[[nodiscard]]
		ConstantBuffer();

		[[nodiscard]]
		ConstantBuffer(const Type& data);

		template <class... Args>
			requires std::is_constructible_v<Type, Args...>
		[[nodiscard]]
		explicit ConstantBuffer(Args&&... args);

		ConstantBuffer& operator =(const Type& data);

		[[nodiscard]]
		constexpr size_t size() const noexcept;

		[[nodiscard]]
		const float* data() const noexcept;

		[[nodiscard]]
		bool isDirty() const noexcept;

		[[nodiscard]]
		Type& operator *() noexcept;

		[[nodiscard]]
		const Type& operator *() const noexcept;

		[[nodiscard]]
		Type* operator ->() noexcept;

		[[nodiscard]]
		const Type* operator ->() const noexcept;

		[[nodiscard]]
		IConstantBuffer* _base() const noexcept;

		bool _update_if_dirty();

	private:

		struct Deleter
		{
			void operator()(WrapperType* p) const
			{
				AlignedDelete<WrapperType>(p, Alignment);
			}
		};

		std::unique_ptr<WrapperType, Deleter> m_wrapper{ AlignedNew<WrapperType>() };

		std::unique_ptr<IConstantBuffer> m_base;

		bool m_isDirty = true;
	};
}

# include "detail/ConstantBuffer.ipp"
