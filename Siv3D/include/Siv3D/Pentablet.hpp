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

namespace s3d
{
	struct PenCaps;
	struct PenState;

	namespace Pentablet
	{
		/// @brief ペンタブレットのドライバがインストールされているかを返します。
		/// @return ペンタブレットのドライバがインストールされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsAvailable();

		[[nodiscard]]
		bool IsConnected();

		[[nodiscard]]
		const String& GetName();

		[[nodiscard]]
		const PenCaps& GetCaps();

		[[nodiscard]]
		const PenState& GetState();
	}
}