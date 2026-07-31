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

namespace s3d
{
	/// @brief アニメーション画像のストリーム読み込み状態
	enum class AnimatedImageReadStatus : uint8
	{
		/// @brief フレームを読み込んだ
		Frame,

		/// @brief ストリームの終端に達した
		EndOfStream,

		/// @brief エラーが発生した
		Error,
	};
}
