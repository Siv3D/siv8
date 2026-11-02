//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "RenderTexture.hpp"
# include "MSRenderTexture.hpp"
# include "Optional.hpp"
# include "Rect.hpp"

namespace s3d
{
	/// @brief スコープ内の 2D 描画先を変更し、破棄時に以前の描画先と viewport を復元します。
	/// @remark 開始時の viewport は描画先全体です。色・ブレンド・シザー・各変換は引き継ぎます。
	/// @remark テクスチャでは 1 座標単位が 1 ピクセルです。Scene の解像度倍率は適用しません。
	/// @remark 切り替えは内容を保持します。クリア・resolve・ミップ生成は明示的に行ってください。
	/// @remark 現在の描画先の RenderTexture を入力として描画しようとすると Error を送出します。MSRenderTexture の入力参照は別画像の resolve 結果なので使用できます。
	/// @remark 対象のテクスチャはスコープ中保持されます。スコープはネストした順序で破棄してください。
	class ScopedRenderTarget2D
	{
	public:

		/// @brief 描画状態を変更しないスコープを作成します。
		[[nodiscard]]
		ScopedRenderTarget2D() = default;

		/// @brief 描画先を変更します。
		/// @param renderTarget 描画先。none は Scene を指定します。
		/// @throws Error 空の RenderTexture を指定した場合。描画状態は変更しません。
		/// @remark 描画先が変わる場合は未処理の描画を発行します。GPU の処理完了は待ちません。
		[[nodiscard]]
		explicit ScopedRenderTarget2D(const Optional<RenderTexture>& renderTarget);

		/// @brief MSAA 側を描画先にします。切り替えと復元の規則は RenderTexture の場合と共通です。
		/// @param renderTarget 描画先。none は Scene を指定します。
		/// @throws Error 空の MSRenderTexture を指定した場合。描画状態は変更しません。
		/// @remark スコープ終了時も自動 resolve しません。スコープ内で resolve する場合、先に Graphics2D::Flush() で描画を発行してください。
		[[nodiscard]]
		explicit ScopedRenderTarget2D(const Optional<MSRenderTexture>& renderTarget);

		/// @brief 一時的に Scene を描画先にします。
		[[nodiscard]]
		explicit ScopedRenderTarget2D(None_t);

		/// @brief 状態を復元する責任を移動します。移動元の破棄では復元しません。
		/// @param other 移動元
		[[nodiscard]]
		ScopedRenderTarget2D(ScopedRenderTarget2D&& other) noexcept;

		/// @brief 保存した描画先と viewport を復元します。描画先が変わる場合は未処理の描画を発行します。
		/// @remark 描画の発行に失敗しても状態を復元します。例外処理中の発行失敗はログに記録します。
		~ScopedRenderTarget2D() noexcept(false);

		ScopedRenderTarget2D(const ScopedRenderTarget2D&) = delete;
		ScopedRenderTarget2D& operator =(const ScopedRenderTarget2D&) = delete;

	private:

		struct TargetTag {};

		ScopedRenderTarget2D(const Optional<Texture>& renderTarget, TargetTag);

		Optional<Optional<Texture>> m_oldRenderTarget;

		Optional<Rect> m_oldViewport;
	};
}
