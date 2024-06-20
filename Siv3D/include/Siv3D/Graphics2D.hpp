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
# include "2DShapes.hpp"
# include "BlendState.hpp"
# include "RasterizerState.hpp"

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

		/// @brief 現在適用されているラスタライザーステートを返します。
		/// @return 現在適用されているラスタライザーステート
		[[nodiscard]]
		RasterizerState GetRasterizerState();

		///// @brief 現在適用されているサンプラーステートを返します。
		///// @param shaderStage シェーダステージ
		///// @param slot テクスチャスロット
		///// @return 現在適用されているサンプラーステート
		//[[nodiscard]]
		//SamplerState GetSamplerState(ShaderStage shaderStage = ShaderStage::Pixel, uint32 slot = 0);

		/// @brief シザー矩形を設定します。
		/// @param rect シザー矩形
		/// @remark シザー矩形は RasterizerState で scissorEnable を true にすることで有効になります。
		void SetScissorRect(const Rect& rect);

		/// @brief 現在設定されているシザー矩形を返します。
		/// @return 現在設定されているシザー矩形
		[[nodiscard]]
		Rect GetScissorRect();
	}
}

# include "detail/Graphics2D.ipp"
