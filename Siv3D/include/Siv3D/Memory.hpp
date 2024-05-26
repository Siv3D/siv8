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
# include <cstdlib>
# include <bit>
# include "Platform.hpp"
# if SIV3D_USE_MIMALLOC
#	include <ThirdParty/mimalloc/mimalloc.h>
# endif

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PointerSize
	//
	////////////////////////////////////////////////////////////////

	/// @brief ポインタのサイズ | The size of a pointer
	inline constexpr size_t PointerSize{ sizeof(void*) };

	////////////////////////////////////////////////////////////////
	//
	//	MinAlignment
	//
	////////////////////////////////////////////////////////////////

	/// @brief 最小のメモリアラインメント | The minimum alignment of the memory
	inline constexpr size_t MinAlignment{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ };

	////////////////////////////////////////////////////////////////
	//
	//	Malloc
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	SIV3D_RESTRICT
	void* Malloc(size_t size) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Calloc
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	SIV3D_RESTRICT
	void* Calloc(size_t count, size_t size) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Realloc
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	void* Realloc(void* p, size_t newSize) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Free
	//
	////////////////////////////////////////////////////////////////

	/// @brief `Malloc()`, `Calloc()`, `Realloc()` で確保したメモリを解放します。 | Frees the memory allocated by `Malloc()`, `Calloc()`, `Realloc()`.
	/// @param p 解放するメモリのポインタ | Pointer to the memory to be freed
	void Free(void* p) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	AlignedMalloc
	//
	////////////////////////////////////////////////////////////////

	/// @brief アライメントを指定してメモリを確保します。 | Allocates memory with alignment.
	/// @param size 確保するメモリのサイズ（バイト） | Size of the memory to be allocated (in bytes)
	/// @param alignment アライメントのサイズ（バイト） | Alignment size (in bytes)
	/// @return 確保したメモリ。確保に失敗した場合 nullptr | Allocated memory, or `nullptr` if failed
	[[nodiscard]]
	SIV3D_RESTRICT
	void* AlignedMalloc(size_t size, size_t alignment) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	AlignedFree
	//
	////////////////////////////////////////////////////////////////

	/// @brief `AlignedMalloc()` で確保したメモリを解放します。 | Frees the memory allocated by `AlignedMalloc()`.
	/// @param p 解放するメモリのポインタ | Pointer to the memory to be freed
	void AlignedFree(void* p, size_t alignment) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	AlignedNew
	//
	////////////////////////////////////////////////////////////////

	/// @brief アライメントを考慮して、指定した型のためのメモリ領域を確保し、オブジェクトを構築します。
	/// @tparam Type 構築するオブジェクト
	/// @tparam ...Args コンストラクタ引数の型
	/// @param ...args コンストラクタ引数
	/// @remark 確保したポインタは `AlignedDelete()` で解放する必要があります。
	/// @return 確保したメモリ領域の先頭ポインタ
	template <class Type, class ...Args>
		requires std::is_constructible_v<Type, Args...>
	[[nodiscard]]
	Type* AlignedNew(Args&&... args);

	////////////////////////////////////////////////////////////////
	//
	//	AlignedDelete
	//
	////////////////////////////////////////////////////////////////

	/// @brief `AlignedNew()` で確保したオブジェクトを破棄し、メモリ領域を解放します。
	/// @tparam Type オブジェクトの型
	/// @param p 解放するメモリ領域の先頭ポインタ
	/// @param alignment アライメントのサイズ（バイト）
	/// @remark p が nullptr の場合は何も起こりません。
	template <class Type>
	void AlignedDelete(Type* p, size_t alignment);

	////////////////////////////////////////////////////////////////
	//
	//	IsAligned
	//
	////////////////////////////////////////////////////////////////

	/// @brief ポインタが指定したサイズにアライメントされているかを返します。 | Returns whether the pointer is aligned to the specified size.
	/// @param p アライメントを調べるポインタ | Pointer to be checked
	/// @param alignment アライメントのサイズ（バイト） | Alignment size (in bytes)
	/// @return 指定したサイズにアライメントされている場合 true, それ以外の場合は false | Returns true if the pointer is aligned to the specified size, otherwise false
	[[nodiscard]]
	bool IsAligned(const void* p, size_t alignment) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	GetAlignment
	//
	////////////////////////////////////////////////////////////////

	/// @brief ポインタのアライメントを返します。 | Returns the alignment of the pointer.
	/// @param p アライメントを調べるポインタ | Pointer to be checked
	/// @return ポインタのアライメント | Alignment of the pointer
	[[nodiscard]]
	size_t GetAlignment(const void* p) noexcept;
}

# include "detail/Memory.ipp"
