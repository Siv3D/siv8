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
# include "Common.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	LineCap
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線端のスタイル | Line cap
	enum class LineCap : uint8
	{
		/// @brief フラット | Flat
		Flat,

		/// @brief 正方形 | Square
		Square,

		/// @brief 丸 | Round
		Round,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, LineCap value);
}
