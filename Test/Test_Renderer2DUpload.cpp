//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBufferPointer.hpp"
# include <type_traits>

namespace
{
	using Index = Vertex2D::IndexType;
	using IndexReference = decltype(*std::declval<Index2DWritePointer>());

	template <class Pointer, class Value>
	concept CanRead = requires(Pointer p) { static_cast<Value>(*p); };

	template <class Pointer>
	concept CanCopyDestination = requires(Pointer p) { p[0] = p[1]; };

	template <class Pointer>
	concept CanModifyDestination = requires(Pointer p) { *p += 1; };

	template <class Pointer>
	concept CanChainAssignment = requires(Pointer p) { p[0] = p[1] = Index{ 1 }; };

	template <class Pointer>
	concept CanReadPosition = requires(Pointer p) { p[0].pos; };

	template <class Pointer>
	concept CanReadColor = requires(Pointer p) { p[0].color; };

	template <class Pointer>
	concept CanReadTex = requires(Pointer p) { p[0].tex; };

	template <class Pointer>
	concept CanUseRawMemory = requires(Pointer p, const void* source) { std::memcpy(p, source, 1); };

	// These assertions are compiled on both platforms in every test build.
	static_assert(not CanRead<Index2DWritePointer, Index>);
	static_assert(not CanRead<Vertex2DWritePointer, Vertex2D>);
	static_assert(not CanCopyDestination<Index2DWritePointer>);
	static_assert(not CanCopyDestination<Vertex2DWritePointer>);
	static_assert(not CanModifyDestination<Index2DWritePointer>);
	static_assert(not CanChainAssignment<Index2DWritePointer>);
	static_assert(not CanReadPosition<Vertex2DWritePointer>);
	static_assert(not CanReadColor<Vertex2DWritePointer>);
	static_assert(not CanReadTex<Vertex2DWritePointer>);
	static_assert(not CanUseRawMemory<Index2DWritePointer>);
	static_assert(not CanUseRawMemory<Vertex2DWritePointer>);
	static_assert(not std::is_convertible_v<Vertex2DWritePointer, Vertex2D*>);
	static_assert(not std::is_convertible_v<Index2DWritePointer, Index*>);
	static_assert(not std::is_convertible_v<Vertex2DWriteReference, Vertex2D*>);
	static_assert(not std::is_convertible_v<IndexReference, Index*>);
	static_assert(std::is_void_v<decltype(std::declval<IndexReference>() = Index{ 1 })>);
	static_assert(std::is_void_v<decltype(std::declval<Vertex2DWriteReference>().setColor(Float4{}))>);
	static_assert(std::is_trivially_copyable_v<Vertex2DWritePointer>);
	static_assert(std::is_trivially_copyable_v<Index2DWritePointer>);
	static_assert(sizeof(Vertex2DWritePointer) == sizeof(Vertex2D*));
	static_assert(sizeof(Vertex2DWriteReference) == sizeof(Vertex2D*));
	static_assert(sizeof(Index2DWritePointer) == sizeof(Index*));
	static_assert(alignof(Vertex2DWritePointer) == alignof(Vertex2D*));
	struct RawBuffer { Vertex2D* vertices; Index* indices; Index offset; };
	static_assert(sizeof(Vertex2DBufferPointer) == sizeof(RawBuffer));
	static_assert(std::is_trivially_copyable_v<Vertex2DBufferPointer>);
}

TEST_CASE("Renderer2DUpload.index_cursor")
{
	CHECK_FALSE(Index2DWritePointer{});
	std::array<Index, 6> storage{ 99, 99, 99, 99, 99, 99 };
	Index2DWritePointer output = (storage.data() + 1);
	REQUIRE(static_cast<bool>(output));
	const auto first = output;
	*output++ = 10;
	output[1] = 30;
	*output = 20;
	++output;
	CHECK((output - first) == 2);
	output += 1;
	output[-1] = 31;
	*(output + 0) = 40;
	CHECK(storage == std::array<Index, 6>{ 99, 10, 20, 31, 40, 99 });
}

TEST_CASE("Renderer2DUpload.vertex_attributes")
{
	CHECK_FALSE(Vertex2DWritePointer{});
	const Float4 color{ 0.1f, 0.2f, 0.3f, 0.4f };
	Vertex2D sentinel;
	sentinel.set(-1, -2, -3, -4, color);
	std::array<Vertex2D, 7> storage;
	storage.fill(sentinel);
	Vertex2DWritePointer output = (storage.data() + 1);
	output[0].set(1, 2, color);
	output[1].set(3, 4, 5, 6, color);
	output[2].set(Float2{ 7, 8 }, color);
	output[3].set(Float2{ 9, 10 }, 11, 12, color);
	output[4].set(Float2{ 13, 14 }, Float2{ 15, 16 }, color);
	CHECK(storage[1].pos == Float2{ 1, 2 });
	CHECK(storage[1].tex == sentinel.tex);
	CHECK(storage[2].pos == Float2{ 3, 4 });
	CHECK(storage[2].tex == Float2{ 5, 6 });
	CHECK(storage[3].pos == Float2{ 7, 8 });
	CHECK(storage[3].tex == sentinel.tex);
	CHECK(storage[4].pos == Float2{ 9, 10 });
	CHECK(storage[4].tex == Float2{ 11, 12 });
	CHECK(storage[5].pos == Float2{ 13, 14 });
	CHECK(storage[5].tex == Float2{ 15, 16 });
	(output++)->setPosition(21, 22);
	output->setPosition(Float2{ 23, 24 });
	output->setTex(25, 26);
	output->setColor(Float4{ 1, 1, 1, 1 });
	CHECK(storage[1].pos == Float2{ 21, 22 });
	CHECK(storage[1].tex == sentinel.tex);
	CHECK(storage[1].color == color);
	CHECK(storage[2].pos == Float2{ 23, 24 });
	CHECK(storage[2].tex == Float2{ 25, 26 });
	CHECK(storage[2].color == Float4{ 1, 1, 1, 1 });
	for (const auto i : { 0u, 6u })
	{
		CHECK(storage[i].pos == sentinel.pos);
		CHECK(storage[i].tex == sentinel.tex);
		CHECK(storage[i].color == sentinel.color);
	}
}

TEST_CASE("Renderer2DUpload.bulk_copy")
{
	std::array<Index, 5> indices{ 99, 99, 99, 99, 99 };
	const std::array<Index, 3> source{ 0, 65535, 42 };
	Index2DWritePointer{ indices.data() + 1 }.copyFrom(source);
	Index2DWritePointer{ indices.data() + 1 }.copyFrom(std::span<const Index>{ source.data(), 0 });
	CHECK(indices == std::array<Index, 5>{ 99, 0, 65535, 42, 99 });
	std::array<Vertex2D, 2> vertices;
	vertices[0].set(1, 2, 3, 4, Float4{ 0.1f, 0.2f, 0.3f, 0.4f });
	vertices[1].set(5, 6, 7, 8, Float4{ 0.5f, 0.6f, 0.7f, 0.8f });
	std::array<Vertex2D, 2> copied;
	Vertex2DWritePointer{ copied.data() }.copyFrom(vertices);
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		CHECK(copied[i].pos == vertices[i].pos);
		CHECK(copied[i].tex == vertices[i].tex);
		CHECK(copied[i].color == vertices[i].color);
	}
}
