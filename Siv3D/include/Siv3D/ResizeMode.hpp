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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	ResizeMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief シーンの自動リサイズモード
	enum class ResizeMode : uint8
	{
		/// @brief シーンの論理サイズをウィンドウの実サイズに合わせる
		Actual,

		/// @brief シーンの論理サイズをウィンドウの仮想サイズに合わせる（デフォルト）
		Virtual,

		/// @brief シーンの論理サイズは変更しない
		Keep,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, ResizeMode value);
}
