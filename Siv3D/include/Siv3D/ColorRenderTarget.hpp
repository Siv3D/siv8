//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "RenderTexture.hpp"
# include "MSRenderTexture.hpp"

namespace s3d
{
	/// @brief RenderTexture / MSRenderTexture を共通に扱う、色描画先の共有ハンドル
	/// @remark 変換とコピーは元のリソースを共有します。元のハンドルを解放しても、このハンドルは有効です。
	/// @remark ScopedRenderTarget2D に指定できます。空のハンドルは Scene の指定とは異なります。
	class ColorRenderTarget
	{
	public:

		/// @brief 空の色描画先を作成します。
		[[nodiscard]]
		ColorRenderTarget() = default;

		/// @brief 単一サンプルの描画先を共有保持します。
		/// @param texture 描画先。空の場合は空になります。
		[[nodiscard]]
		ColorRenderTarget(const RenderTexture& texture);

		/// @brief MSAA の描画先と resolve 結果を共有保持します。
		/// @param texture 描画先。空の場合は空になります。
		[[nodiscard]]
		ColorRenderTarget(const MSRenderTexture& texture);

		/// @brief 空であるかを返します。
		/// @return 空なら true、それ以外は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief 有効な描画先を保持しているかを返します。
		/// @return 空でなければ true、それ以外は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		/// @brief このハンドルを空にします。他の共有ハンドルは有効なままです。
		void release();

		/// @brief 元のテクスチャと共通のアセット ID を返します。
		/// @return テクスチャの ID。空なら Null ID
		[[nodiscard]]
		Texture::IDType id() const noexcept;

		/// @brief 描画先のサイズを返します。
		/// @return サイズ（ピクセル）。Texture::size() と共通です。
		[[nodiscard]]
		Size size() const;

		/// @brief 色の格納フォーマットを返します。
		/// @return フォーマット。Texture::format() と共通です。
		[[nodiscard]]
		TextureFormat format() const;

		/// @brief 描画先の MSAA サンプル数を返します。
		/// @return 単一サンプルまたは空なら NoMSAA、MSAA なら X2MSAA / X4MSAA
		[[nodiscard]]
		MSAASampleCount getMSAASampleCount() const;

		/// @brief 描画先を指定色でクリアします。MSAA の場合は MSAA 側だけを更新します。
		/// @param color 線形の色。色空間の規則は RenderTexture::clear() と共通です。
		/// @return *this
		/// @remark 空なら何もしません。保留中の 2D 描画を発行します。自動 resolve と GPU 完了待ちは行いません。
		const ColorRenderTarget& clear(const ColorF& color) const;

		/// @brief MSAA の描画先なら resolve します。単一サンプルまたは空なら何もしません。
		/// @remark MSRenderTexture::resolve() と同じ規則を使います。自動 Flush・ミップ生成・GPU 完了待ちは行いません。
		void resolve() const;

		/// @brief サンプリング用画像のミップマップを再生成します。
		/// @remark RenderTexture::generateMips() / MSRenderTexture::generateMips() と同じ規則を使います。MSAA なら最後の resolve 結果を使い、自動 resolve は行いません。
		void generateMips() const;

		/// @brief サンプリング用画像の共有ハンドルを返します。
		/// @return 単一サンプルなら同じ画像、MSAA なら最後の resolve 結果を参照する Texture。空なら空の Texture
		/// @remark 返されたハンドルは、この ColorRenderTarget の解放後も有効です。Flush・resolve・GPU コピーは行いません。
		[[nodiscard]]
		Texture getTexture() const;

		/// @brief 共有しているリソースを交換します。
		/// @param other 交換相手
		void swap(ColorRenderTarget& other) noexcept;

		/// @brief 同じリソースを参照しているかを返します。空同士も等しくなります。
		/// @param lhs 一方の描画先
		/// @param rhs もう一方の描画先
		/// @return 同じリソースなら true、それ以外は false
		[[nodiscard]]
		friend bool operator ==(const ColorRenderTarget& lhs, const ColorRenderTarget& rhs) noexcept
		{
			return (lhs.m_texture == rhs.m_texture);
		}

		/// @brief 共有しているリソースを交換します。
		/// @param lhs 一方の描画先
		/// @param rhs もう一方の描画先
		friend void swap(ColorRenderTarget& lhs, ColorRenderTarget& rhs) noexcept
		{
			lhs.swap(rhs);
		}

	private:

		Texture m_texture;
	};
}

# include "detail/ColorRenderTarget.ipp"
