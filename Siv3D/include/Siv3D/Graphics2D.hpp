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
# include "SamplerState.hpp"
# include "ShaderStage.hpp"

namespace s3d
{
	struct TextStyle;
	struct Mat3x2;

	namespace Graphics2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetColorMul
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されている乗算カラーを返します。
		/// @return 現在適用されている乗算カラー
		[[nodiscard]]
		Float4 GetColorMul();

		////////////////////////////////////////////////////////////////
		//
		//	GetColorAdd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されている加算カラーを返します。
		/// @return 現在適用されている加算カラー
		[[nodiscard]]
		Float3 GetColorAdd();

		////////////////////////////////////////////////////////////////
		//
		//	GetBlendState
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているブレンドステートを返します。
		/// @return 現在適用されているブレンドステート
		[[nodiscard]]
		BlendState GetBlendState();

		////////////////////////////////////////////////////////////////
		//
		//	GetRasterizerState
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているラスタライザーステートを返します。
		/// @return 現在適用されているラスタライザーステート
		[[nodiscard]]
		RasterizerState GetRasterizerState();

		////////////////////////////////////////////////////////////////
		//
		//	GetVSSamplerState
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点シェーダに現在適用されているサンプラーステートを返します。
		/// @param slot テクスチャスロット
		/// @return 頂点シェーダに現在適用されているサンプラーステート
		[[nodiscard]]
		SamplerState GetVSSamplerState(uint32 slot = 0);

		////////////////////////////////////////////////////////////////
		//
		//	GetPSSamplerState
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピクセルシェーダに現在適用されているサンプラーステートを返します。
		/// @param slot テクスチャスロット
		/// @return ピクセルシェーダに現在適用されているサンプラーステート
		[[nodiscard]]
		SamplerState GetPSSamplerState(uint32 slot = 0);

		////////////////////////////////////////////////////////////////
		//
		//	GetSamplerState
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているサンプラーステートを返します。
		/// @param shaderStage シェーダステージ
		/// @param slot テクスチャスロット
		/// @return 現在適用されているサンプラーステート
		[[nodiscard]]
		SamplerState GetSamplerState(ShaderStage shaderStage = ShaderStage::Pixel, uint32 slot = 0);

		////////////////////////////////////////////////////////////////
		//
		//	GetScissorRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在設定されているシザー矩形を返します。
		/// @return 現在設定されているシザー矩形
		[[nodiscard]]
		Optional<Rect> GetScissorRect();

		////////////////////////////////////////////////////////////////
		//
		//	GetViewport
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているビューポートを返します。
		/// @return 現在適用されているビューポート。シーンと一致する場合は none
		[[nodiscard]]
		Optional<Rect> GetViewport();

		////////////////////////////////////////////////////////////////
		//
		//	GetLocalTransform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているローカル座標変換を返します。
		/// @return 現在適用されているローカル座標変換
		[[nodiscard]]
		const Mat3x2& GetLocalTransform();

		////////////////////////////////////////////////////////////////
		//
		//	GetCameraTransform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されている 2D カメラ座標変換を返します。
		/// @return 現在適用されている 2D カメラ座標変換
		[[nodiscard]]
		const Mat3x2& GetCameraTransform();

		////////////////////////////////////////////////////////////////
		//
		//	GetMaxScaling
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されている座標変換による縦横の拡大縮小倍率のうち大きいほうを返します。 
		/// @return 現在適用されている座標変換による縦横の拡大縮小倍率のうち大きいほう
		/// @remark 線の太さをこの値で除算すれば、どのような拡大縮小倍率でも一定の太さになる線を描けます。
		[[nodiscard]]
		float GetMaxScaling() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetRenderTargetSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のレンダーターゲットのサイズ（ピクセル）を返します
		/// @return 現在のレンダーターゲットのサイズ（ピクセル）
		[[nodiscard]]
		Size GetRenderTargetSize() noexcept;
	}
}

# include "detail/Graphics2D.ipp"
