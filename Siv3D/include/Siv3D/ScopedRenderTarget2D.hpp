//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "ColorRenderTarget.hpp"
# include "Optional.hpp"
# include "Rect.hpp"
# include "detail/RenderTargetState.hpp"
# include <initializer_list>
# include <span>

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
		/// @param renderTarget 描画先。RenderTexture / MSRenderTexture も指定できます。none は Scene を指定します。
		/// @throws Error 空の ColorRenderTarget（空の RenderTexture / MSRenderTexture を含む）を指定した場合。描画状態は変更しません。
		/// @remark 描画先が変わる場合は未処理の描画を発行します。GPU の処理完了は待ちません。
		/// @remark MSAA なら MSAA 側へ描画します。スコープ内で resolve する場合、先に Graphics2D::Flush() で描画を発行してください。
		[[nodiscard]]
		explicit ScopedRenderTarget2D(const Optional<ColorRenderTarget>& renderTarget);

		/// @brief 指定順に色出力 0 から割り当てて、複数の描画先を選択します。
		/// @param renderTargets 1～8 枚の描画先。各ハンドルを共有保持します。
		/// @throws Error 空配列、9 枚以上、空ハンドル、重複リソース、サイズ不一致、またはサンプル数不一致の場合。描画状態は変更せず、保留描画も発行しません。
		/// @remark 全描画先を同じサイズ・サンプル数（単一サンプル、2x、4x）にそろえます。フォーマットは混在できます。GPU が対応する組み合わせを指定してください。
		/// @remark MSAA では全出力の MSAA 画像へ描画します。各出力の resolve() は明示的に呼び出します。自動 Flush は行わないため、保留描画を含めるには先に Graphics2D::Flush() を呼び出してください。
		/// @remark 標準ピクセルシェーダは色出力 0 だけを更新します。カスタムピクセルシェーダは、指定した全出力へ値を返す必要があります。ブレンド状態は全出力に共通です。
		/// @remark 切り替え・復元・viewport・明示操作の規則は単一指定と共通です。
		[[nodiscard]]
		explicit ScopedRenderTarget2D(std::span<const ColorRenderTarget> renderTargets);

		/// @brief リストで複数の描画先を選択します。
		/// @param renderTargets 描画先。規則は span による指定と共通です。
		[[nodiscard]]
		explicit ScopedRenderTarget2D(std::initializer_list<ColorRenderTarget> renderTargets);

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

		Optional<detail::RenderTargetState> m_oldRenderTargets;

		Optional<Rect> m_oldViewport;
	};
}
