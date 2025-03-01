//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <bit>
# include "Common.hpp"

namespace s3d
{	
	////////////////////////////////////////////////////////////////
	//
	//	SwapEndian
	//
	////////////////////////////////////////////////////////////////

	/// @brief エンディアンを変換します。 | Convert endian
	/// @param value エンディアンを変換する値 | Value to convert endian
	/// @return エンディアン変換後の値 | Value after endian conversion
	[[nodiscard]]
	constexpr uint16 SwapEndian(uint16 value) noexcept;

	/// @brief エンディアンを変換します。 | Convert endian
	/// @param value エンディアンを変換する値 | Value to convert endian
	/// @return エンディアン変換後の値 | Value after endian conversion
	[[nodiscard]]
	constexpr uint32 SwapEndian(uint32 value) noexcept;

	/// @brief エンディアンを変換します。 | Convert endian
	/// @param value エンディアンを変換する値 | Value to convert endian
	/// @return エンディアン変換後の値 | Value after endian conversion
	[[nodiscard]]
	constexpr uint64 SwapEndian(uint64 value) noexcept;
}

# include "detail/Endian.ipp"
