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
# include "PointVector.hpp"
# include "BlendState.hpp"

namespace s3d
{
	struct TextStyle;

	namespace Graphics2D
	{
		/// @brief 現在適用されている乗算カラーを返します。
		/// @return 現在適用されている乗算カラー
		[[nodiscard]]
		Float4 GetColorMul();

		/// @brief 現在適用されている加算カラーを返します。
		/// @return 現在適用されている加算カラー
		[[nodiscard]]
		Float3 GetColorAdd();

		/// @brief 現在適用されているブレンドステートを返します。
		/// @return 現在適用されているブレンドステート
		[[nodiscard]]
		BlendState GetBlendState();
	}
}

# include "detail/Graphics2D.ipp"
