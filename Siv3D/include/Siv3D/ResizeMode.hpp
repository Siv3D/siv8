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
		/// @brief シーンのサイズをウィンドウの実サイズに合わせる
		Actual,

		/// @brief シーンのサイズをウィンドウの仮想サイズに合わせる（デフォルト）
		Virtual,

		/// @brief シーンのサイズは変更しない
		Keep,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, ResizeMode value);
}

