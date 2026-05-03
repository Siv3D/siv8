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
# include "ColorHSV.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PDFRenderOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief PDF のページをレンダリングする際のオプション
	struct PDFRenderOptions
	{
		/// @brief ページの拡大率
		double scale = 1.0;

		/// @brief ページの回転
		/// @remark 0: 回転なし、1: 90 度時計回り、2: 180 度、3: 270 度時計回り
		int32 rotation = 0;
		
		/// @brief 背景色
		Color backgroundColor = Palette::White;
		
		/// @brief 注釈を描画するかどうか
		bool renderAnnotations = true;
	};
}
