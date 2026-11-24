//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <array>
# include "TextureRegion.hpp"
# include "RectF.hpp"

namespace s3d
{
	/// @brief 四隅の形を保ちながら、辺と中央を伸縮・反復して描画する 9 パッチテクスチャ
	/// @remark テクスチャ資源を共有します。コピー後の分割・表示設定は独立しています。
	/// @pre 入力する座標・寸法・UV・倍率は有限値とします。
	class NinePatch
	{
	public:

		/// @brief 素材の見た目の上・右・下・左から測った分割幅
		/// @remark 単位は TextureRegion::size の絶対値に対応する描画ピクセルです。
		struct Slice
		{
			/// @brief 上辺の高さ
			double top = 0;
			/// @brief 右辺の幅
			double right = 0;
			/// @brief 下辺の高さ
			double bottom = 0;
			/// @brief 左辺の幅
			double left = 0;

			[[nodiscard]]
			friend constexpr bool operator ==(const Slice&, const Slice&) noexcept = default;
		};

		/// @brief 反復する領域。false の領域は伸縮します。
		struct Tiling
		{
			/// @brief 上辺・下辺を横方向に反復するか
			bool topBottom = false;
			/// @brief 左辺・右辺を縦方向に反復するか
			bool leftRight = false;
			/// @brief 中央を縦横に反復するか
			bool center = false;

			/// @brief 辺と中央をすべて反復する設定を返します。
			[[nodiscard]]
			static constexpr Tiling All() noexcept;

			[[nodiscard]]
			friend constexpr bool operator ==(const Tiling&, const Tiling&) noexcept = default;
		};

		/// @brief 空の 9 パッチを作成します。
		[[nodiscard]]
		NinePatch() = default;

		/// @brief テクスチャ全体を、四辺共通の幅で分割します。
		/// @param texture 素材
		/// @param slice 四辺共通の分割幅
		/// @see NinePatch(const TextureRegion&, Slice)
		[[nodiscard]]
		NinePatch(const Texture& texture, double slice);

		/// @brief テクスチャ全体を、四辺別の幅で分割します。
		/// @param texture 素材
		/// @param slice 四辺の分割幅
		/// @see NinePatch(const TextureRegion&, Slice)
		[[nodiscard]]
		NinePatch(const Texture& texture, Slice slice);

		/// @brief 部分テクスチャを、四辺共通の幅で分割します。
		/// @param source 素材
		/// @param slice 四辺共通の分割幅
		/// @see NinePatch(const TextureRegion&, Slice)
		[[nodiscard]]
		NinePatch(const TextureRegion& source, double slice);

		/// @brief 部分テクスチャを、四辺別の幅で分割します。
		/// @param source 素材。現在の描画サイズと、UV・負のサイズによる反転を使用します。
		/// @param slice 反転を適用した見た目の四辺から測る分割幅
		/// @remark 空のテクスチャ、負の分割幅、分割後の中央の幅または高さが 0 以下の場合は空になります。
		/// @remark 構築後に元の TextureRegion を変更しても分割は変わりません。同じテクスチャ資源の画像更新は描画に反映されます。
		[[nodiscard]]
		NinePatch(const TextureRegion& source, Slice slice);

		/// @brief 空であるかを返します。
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief 空でないかを返します。
		[[nodiscard]]
		explicit operator bool() const noexcept;

		/// @brief 素材を返します。空の場合は空の TextureRegion です。
		[[nodiscard]]
		const TextureRegion& source() const noexcept;

		/// @brief 分割幅を返します。空の場合はすべて 0 です。
		[[nodiscard]]
		Slice slice() const noexcept;

		/// @brief 表示する枠と反復模様の倍率を設定します。素材の分割位置は変わりません。
		/// @param scale 倍率。既定値は 1。負の値は 0 として扱います。
		/// @return *this
		NinePatch& setBorderScale(double scale) noexcept;

		/// @brief 表示する枠の倍率を返します。
		[[nodiscard]]
		double borderScale() const noexcept;

		/// @brief 辺・中央の反復を設定します。既定値はすべて false です。
		/// @param tiling 反復する領域
		/// @return *this
		/// @remark 反復は各領域の左上から始まり、右端・下端の余りを切り取ります。
		/// @remark タイルの寸法は素材の中央サイズに枠の実表示倍率（小さい描画先での縮小を含む）を掛けた値です。枠倍率 0 では伸縮します。
		/// @remark 各軸の反復は最大 128 回です。超える場合は反復模様の倍率だけを縦横共通で引き上げます。枠幅と innerRect() は変わりません。
		/// @remark 通常の部分テクスチャ描画と同じサンプリングを使います。Linear や mipmap による隣接領域の色の混入は除去しません。
		NinePatch& setTiling(Tiling tiling) noexcept;

		/// @brief 反復する領域を返します。
		[[nodiscard]]
		Tiling tiling() const noexcept;

		/// @brief 現在の倍率で、角を縮めずに描ける最小サイズを返します。
		/// @return 左右の枠幅の和と上下の枠幅の和。空の場合は (0, 0)
		[[nodiscard]]
		SizeF minimumSize() const noexcept;

		/// @brief 描画先の中央パッチの領域を返します。
		/// @param rect 描画先
		/// @return draw() と同じ縮小規則を適用した中央領域。空、または rect の幅・高さが 0 以下の場合は { rect.pos, 0, 0 }
		[[nodiscard]]
		RectF innerRect(const RectF& rect) const noexcept;

		/// @brief 四隅・辺・中央を描画します。
		/// @param rect 描画先
		/// @param color 乗算する色
		/// @return rect。描画するパッチがない場合は { rect.pos, 0, 0 }
		/// @remark 枠が収まらない場合は四辺を共通倍率で縮め、角の縦横比を保ちます。
		/// @remark 空、または rect の幅・高さが 0 以下の場合は描画しません。面積 0 のパッチは省略します。
		/// @remark 現在の描画変換・色・サンプラー・シザーを使用します。枠倍率 0 は中央だけを伸縮し、全分割幅 0 は素材全体を使用します。
		/// @see setTiling()
		RectF draw(const RectF& rect, const ColorF& color = Palette::White) const;

		/// @brief 中央を除いた枠を描画します。
		/// @param rect 描画先
		/// @param color 乗算する色
		/// @return rect。描画するパッチがない場合は { rect.pos, 0, 0 }
		/// @remark 密度制限を含め、反復模様の倍率は draw() と同じです。
		/// @see draw()
		RectF drawFrame(const RectF& rect, const ColorF& color = Palette::White) const;

	private:

		TextureRegion m_source;
		Slice m_slice;
		std::array<float, 4> m_u{};
		std::array<float, 4> m_v{};
		double m_borderScale = 1;
		Tiling m_tiling;

		RectF drawImpl(const RectF& rect, const ColorF& color, bool drawCenter) const;

		RectF drawTiled(const RectF& rect, const ColorF& color, bool drawCenter) const;
	};
}

# include "detail/NinePatch.ipp"
