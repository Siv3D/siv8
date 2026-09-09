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
# include "PointVector.hpp"
# include "2DShapes.hpp"
# include "BlendState.hpp"
# include "RasterizerState.hpp"
# include "SamplerState.hpp"
# include "ShaderStage.hpp"
# include "VertexShader.hpp"
# include "PixelShader.hpp"

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
		//	GetCustomVertexShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているカスタム頂点シェーダを返します。
		/// @return 現在適用されているカスタム頂点シェーダ。デフォルトの場合は none
		[[nodiscard]]
		Optional<VertexShader> GetCustomVertexShader();

		////////////////////////////////////////////////////////////////
		//
		//	GetCustomPixelShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在適用されているカスタムピクセルシェーダを返します。
		/// @return 現在適用されているカスタムピクセルシェーダ。デフォルトの場合は none
		[[nodiscard]]
		Optional<PixelShader> GetCustomPixelShader();

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
		//	GetRMSScaling
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のローカル座標変換と 2D カメラ座標変換を合成した行列の RMS 拡大率を返します。
		/// @return 合成行列の Mat3x2::rmsScaling() の値。線形部分がゼロの場合は 0
		/// @remark 等方スケールでは、線幅をこの値で割ることで画面上の太さを補正できます。非一様スケールやせん断では近似になります。
		/// @remark 戻り値が 0 の場合は、線幅やパターンの補正に逆数を使用できません。
		[[nodiscard]]
		float GetRMSScaling() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetRenderTargetSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のレンダーターゲットのサイズ（ピクセル）を返します
		/// @return 現在のレンダーターゲットのサイズ（ピクセル）
		[[nodiscard]]
		Size GetRenderTargetSize() noexcept;



		////////////////////////////////////////////////////////////////
		//
		//	SetMSDFParameters
		//
		////////////////////////////////////////////////////////////////

		/// @brief MSDF 描画用のパラメータを設定します。
		/// @param textStyle テキストスタイル
		/// @param fontBaseSize フォントの基本サイズ
		void SetMSDFParameters(const TextStyle& textStyle, int32 fontBaseSize);

	}
}

# include "detail/Graphics2D.ipp"
