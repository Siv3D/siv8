//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <cstring>
# include <span>
# include <type_traits>

namespace s3d
{
	namespace detail
	{
		// Upload memory can be write-combined. No value conversion, read/modify/write,
		// proxy-to-proxy assignment, or assignment result may expose its contents.
		template <class Type>
		class WriteOnlyReference
		{
		public:

			explicit constexpr WriteOnlyReference(Type* pointer) noexcept : m_pointer{ pointer } {}

			WriteOnlyReference(const WriteOnlyReference&) = default;

			void operator =(const WriteOnlyReference&) = delete;

			void operator =(const Type value) const noexcept
			{
				*m_pointer = value;
			}

		private:

			Type* m_pointer;
		};

		template <>
		class WriteOnlyReference<Vertex2D>
		{
		public:

			explicit constexpr WriteOnlyReference(Vertex2D* pointer) noexcept : m_pointer{ pointer } {}

			WriteOnlyReference(const WriteOnlyReference&) = default;

			void operator =(const WriteOnlyReference&) = delete;

			[[nodiscard]]
			constexpr const WriteOnlyReference* operator ->() const noexcept { return this; }

			void set(const float x, const float y, const Float4 color) const noexcept { m_pointer->set(x, y, color); }

			void set(const float x, const float y, const float u, const float v, const Float4 color) const noexcept { m_pointer->set(x, y, u, v, color); }

			void set(const Float2 pos, const Float4 color) const noexcept { m_pointer->set(pos, color); }

			void set(const Float2 pos, const float u, const float v, const Float4 color) const noexcept { m_pointer->set(pos, u, v, color); }

			void set(const Float2 pos, const Float2 tex, const Float4 color) const noexcept { m_pointer->set(pos, tex, color); }

			void setPosition(const Float2 pos) const noexcept { m_pointer->pos = pos; }

			void setPosition(const float x, const float y) const noexcept { m_pointer->pos.set(x, y); }

			void setTex(const float u, const float v) const noexcept { m_pointer->tex.set(u, v); }

			void setColor(const Float4 color) const noexcept { m_pointer->color = color; }

		private:

			Vertex2D* m_pointer;
		};

		// A borrowed output cursor. Pointer arithmetic moves the cursor without
		// accessing the destination. Only the backend constructs it from raw memory.
		template <class Type>
		class WriteOnlyPointer
		{
			static_assert(std::is_trivially_copyable_v<Type>);

		public:

			constexpr WriteOnlyPointer(Type* pointer = nullptr) noexcept : m_pointer{ pointer } {}

			[[nodiscard]]
			explicit constexpr operator bool() const noexcept { return (m_pointer != nullptr); }

			[[nodiscard]]
			constexpr WriteOnlyReference<Type> operator *() const noexcept { return WriteOnlyReference<Type>{ m_pointer }; }

			[[nodiscard]]
			constexpr WriteOnlyReference<Type> operator ->() const noexcept { return **this; }

			[[nodiscard]]
			constexpr WriteOnlyReference<Type> operator [](const std::ptrdiff_t offset) const noexcept { return WriteOnlyReference<Type>{ m_pointer + offset }; }

			[[nodiscard]]
			constexpr WriteOnlyPointer operator +(const std::ptrdiff_t offset) const noexcept { return (m_pointer + offset); }

			[[nodiscard]]
			constexpr std::ptrdiff_t operator -(const WriteOnlyPointer other) const noexcept { return (m_pointer - other.m_pointer); }

			constexpr WriteOnlyPointer& operator +=(const std::ptrdiff_t offset) noexcept
			{
				m_pointer += offset;
				return *this;
			}

			constexpr WriteOnlyPointer& operator ++() noexcept
			{
				++m_pointer;
				return *this;
			}

			constexpr WriteOnlyPointer operator ++(int) noexcept
			{
				const auto previous = *this;
				++*this;
				return previous;
			}

			void copyFrom(const std::span<const Type> source) const noexcept
			{
				std::memcpy(m_pointer, source.data(), source.size_bytes());
			}

		private:

			Type* m_pointer;
		};
	}

	using Vertex2DWritePointer = detail::WriteOnlyPointer<Vertex2D>;
	using Vertex2DWriteReference = detail::WriteOnlyReference<Vertex2D>;
	using Index2DWritePointer = detail::WriteOnlyPointer<Vertex2D::IndexType>;

	struct Vertex2DBufferPointer
	{
		Vertex2DWritePointer pVertex;

		Index2DWritePointer pIndex;

		Vertex2D::IndexType indexOffset;
	};
}
