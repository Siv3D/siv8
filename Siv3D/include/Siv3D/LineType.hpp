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
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	LineType
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線の種類
	enum class LineType : uint8
	{
		/// @brief 実線
		Solid,

		/// @brief ドット
		Dotted,

		/// @brief ダッシュ
		Dashed,

		/// @brief 長いダッシュ
		LongDash,

		/// @brief ダッシュとドット
		DashDot,

		/// @brief 丸いドット
		RoundDot,
	};

	////////////////////////////////////////////////////////////////
	//
	//	LineType
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, LineType value);
}
