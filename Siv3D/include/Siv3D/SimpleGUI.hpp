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
# include "Common.hpp"

namespace s3d
{
	class Font;

	namespace SimpleGUI
	{
		/// @brief SimpleGUI のデフォルトフォントを取得します。
		/// @remark ベースが CJK_Regular で、利用可能であれば Icon_MaterialDesign, ColorEmoji がフォールバックとして追加されています。
		/// @return デフォルトフォント
		[[nodiscard]]
		const Font& GetFont() noexcept;
	}
}
