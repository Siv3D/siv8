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
# include <cstdlib>

namespace s3d
{
	/// @brief ポインタのサイズ | The size of a pointer
	inline constexpr size_t PointerSize{ sizeof(void*) };

	/// @brief 最小のメモリアラインメント | The minimum alignment of the memory
	inline constexpr size_t MinAlignment{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ };

	[[nodiscard]]
	SIV3D_RESTRICT
	void* Malloc(size_t size) noexcept;

	[[nodiscard]]
	SIV3D_RESTRICT
	void* Calloc(size_t count, size_t size) noexcept;

	[[nodiscard]]
	void* Realloc(void* p, size_t newsize) noexcept;

	/// @brief `Malloc()`, `Calloc()`, `Realloc()` で確保したメモリを解放します。
	/// @param p 解放するメモリのポインタ
	[[nodiscard]]
	void Free(void* p) noexcept;

	/// @brief アライメントを指定してメモリを確保します。
	/// @param size 確保するメモリのサイズ（バイト）
	/// @param alignment アライメントのサイズ（バイト）
	/// @return 確保したメモリ。確保に失敗した場合 nullptr
	[[nodiscard]]
	SIV3D_RESTRICT
	void* AlignedAlloc(size_t size, size_t alignment) noexcept;

	/// @brief `AlignedAlloc()` で確保したメモリを解放します。
	/// @param p 解放するメモリのポインタ
	[[nodiscard]]
	void AlignedFree(void* p) noexcept;

	/// @brief ポインタが指定したサイズにアライメントされているかを返します。
	/// @param p アライメントを調べるポインタ
	/// @param alignment アライメントのサイズ（バイト）
	/// @return 指定したサイズにアライメントされている場合 true, それ以外の場合は false
	[[nodiscard]]
	bool IsAligned(const void* p, size_t alignment) noexcept;
}

# include "detail/Memory.ipp"
