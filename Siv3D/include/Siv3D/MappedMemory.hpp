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
# include <cstddef>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MappedMemory
	//
	////////////////////////////////////////////////////////////////

	/// @brief マップされたメモリの情報 | Information of the mapped memory
	/// @remark 所有権を持たない範囲です。コピーしてもマップの寿命は延びず、所有元のアンマップ・クローズ・破棄後は使用できません。
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

		/// @brief 先頭アドレスが nullptr でないかを返します。
		/// @return data が nullptr でない場合 true, それ以外の場合は false
		/// @remark 所有元のマップが存続しているかは検査しません。
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;
	};
}

# include "detail/MappedMemory.ipp"
