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
	//	MappedMemory
	//
	////////////////////////////////////////////////////////////////

	/// @brief マップされたメモリの情報 | Information of the mapped memory
	struct MappedMemory
	{
		/// @brief メモリの先頭アドレス | The pointer to the memory
		void* data = nullptr;

		/// @brief メモリのサイズ（バイト） | The size of the memory in bytes
		size_t size = 0;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリが有効かを返します。
		/// @return メモリが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;
	};
}

# include "detail/MappedMemory.ipp"
