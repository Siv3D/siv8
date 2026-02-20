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
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"
# include "Optional.hpp"

namespace s3d
{
	/// @brief シリアルポートの情報
	struct SerialPortInfo
	{
		/// @brief シリアルポートの名前（例: "COM3", "/dev/ttyUSB0"）
		String port;

		/// @brief OS から提供されるデバイスの説明（例: "USB Serial Device (COM3)"）
		String description;

		/// @brief ハードウェア ID
		String hardwareID;

		/// @brief 製造元の名前
		String manufacturer;

		/// @brief ベンダー ID（VID）
		Optional<uint16> vendorID;

		/// @brief プロダクト ID（PID）
		Optional<uint16> productID;

		/// @brief 特定の VID / PID を持つデバイスかどうかを判定します
		[[nodiscard]]
		constexpr bool isDevice(uint16 vid, uint16 pid) const noexcept;
	};

	namespace System
	{
		/// @brief 利用可能なシリアルポートの一覧を返します。
		/// @return シリアルポート情報のリスト
		[[nodiscard]]
		Array<SerialPortInfo> EnumerateSerialPorts();
	}
}

# include "detail/SerialPortInfo.ipp"
