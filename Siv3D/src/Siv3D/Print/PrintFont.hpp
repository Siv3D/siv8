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
# include <Siv3D/Font.hpp>
# include <Siv3D/PixelShader.hpp>

namespace s3d
{
	struct PrintFont
	{
		/// @brief Print 用フォント
		Font textFont;
		
		/// @brief Print 専用ピクセルシェーダ
		PixelShader ps;
	};
}
