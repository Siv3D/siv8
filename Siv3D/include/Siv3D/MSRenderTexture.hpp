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
# include "Texture.hpp"
# include "MSAASampleCount.hpp"
# include <array>

namespace s3d
{
	/// @brief 深度を持たない、MSAA 用と resolve 後の色テクスチャを内包するレンダーテクスチャ
	/// @remark Texture としての描画・参照は最後の resolve 結果を使います。コピーは 2 枚をまとめて共有します。
	/// @remark 生成時は両側とミップを初期色で初期化します。clear() は MSAA 側だけを更新します。
	/// @remark 各辺 1～16384 ピクセル、2x / 4x MSAA に対応します。不正な指定、GPU が非対応のフォーマット・サンプル数・ミップ設定、GPU リソースの確保失敗では空になります。設定の自動変更は行いません。
	/// @remark 明示した format が sRGB の有無を決定します。初期色と clear() は RenderTexture::clear() と同じ色空間の規則を使います。
	/// @remark GPU コマンドの記録を開始できない場合は InternalEngineError を送出します。
	class MSRenderTexture : public Texture
	{
	public:

		/// @brief 空のマルチサンプルレンダーテクスチャを作成します。
		[[nodiscard]]
		MSRenderTexture() = default;

		/// @brief 全成分を 0 で初期化したマルチサンプルレンダーテクスチャを作成します。
		/// @param size サイズ（ピクセル）
		/// @param format ピクセルフォーマット
		/// @param desc resolve 後のテクスチャのミップマップなどの設定
		/// @param sampleCount X2MSAA または X4MSAA。Scene の設定には連動しません。
		[[nodiscard]]
		explicit MSRenderTexture(const Size& size, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap, MSAASampleCount sampleCount = MSAASampleCount::X4MSAA);

		/// @brief 指定した色で初期化したマルチサンプルレンダーテクスチャを作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色
		/// @param format ピクセルフォーマット
		/// @param desc resolve 後のテクスチャのミップマップなどの設定
		/// @param sampleCount X2MSAA または X4MSAA
		[[nodiscard]]
		MSRenderTexture(const Size& size, const ColorF& color, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap, MSAASampleCount sampleCount = MSAASampleCount::X4MSAA);

		/// @brief サンプル数を指定し、全成分を 0 で初期化して作成します。
		/// @param size サイズ（ピクセル）
		/// @param sampleCount X2MSAA または X4MSAA
		/// @param format ピクセルフォーマット
		/// @param desc resolve 後のテクスチャのミップマップなどの設定
		[[nodiscard]]
		MSRenderTexture(const Size& size, MSAASampleCount sampleCount, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 初期色とサンプル数を指定して作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色
		/// @param sampleCount X2MSAA または X4MSAA
		/// @param format ピクセルフォーマット
		/// @param desc resolve 後のテクスチャのミップマップなどの設定
		[[nodiscard]]
		MSRenderTexture(const Size& size, const ColorF& color, MSAASampleCount sampleCount, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 作成時の MSAA サンプル数を返します。
		/// @return X2MSAA または X4MSAA。空なら NoMSAA
		[[nodiscard]]
		MSAASampleCount getMSAASampleCount() const;

		/// @brief MSAA 側の全サンプルを指定した色でクリアします。
		/// @param color 線形の色
		/// @return *this
		/// @remark 空なら何もしません。保留中の 2D 描画を発行します。resolve 後のテクスチャは変更せず、GPU 完了は待ちません。
		const MSRenderTexture& clear(const ColorF& color) const;

		/// @brief MSAA 側から単一サンプル側のミップレベル 0 を更新します。
		/// @remark 空なら何もしません。MSAA 側の内容を保持します。ミップ生成と GPU 完了待ちは行いません。
		/// @remark 保留描画を自動 Flush しません。このテクスチャへの描画や、以前の resolve 結果を使う描画を先に実行したい場合は、必要なレンダラーを呼び出し側で Flush してください。
		void resolve() const;

		/// @brief 最後の resolve 結果から、単一サンプル側のミップマップを再生成します。
		/// @remark 空またはミップレベルが 1 つの場合は何もしません。保留中の 2D 描画を発行します。自動 resolve と GPU 完了待ちは行いません。
		void generateMips() const;

		/// @brief 指定したミップレベルを生バイト列に読み戻します。
		/// @param destination パディングのない行を格納する出力先。対象レベルの幅 × 高さ × pixelSize() バイトが必要です。
		/// @param mipLevel ミップレベル（0 が元のサイズ）
		/// @return 成功なら true。空、不正なレベル・出力サイズ、GPU 転送失敗では出力を変更せず false
		/// @remark 最後の resolve 結果を読みます。自動 resolve は行いません。保留中の 2D 描画を発行し、GPU 転送の完了を待ちます。フォーマット変換や sRGB のデコードは行いません。
		bool read(std::span<Byte> destination, uint32 mipLevel = 0) const;

		/// @brief RGBA8（sRGB を含む）を Image に読み戻します。
		/// @param image 出力画像
		/// @param mipLevel ミップレベル
		/// @return 成功なら true。空、型・レベルの不一致、GPU 転送失敗では出力を空にして false
		/// @remark read(std::span<Byte>, uint32) と同じ同期と色空間の規則を使います。出力は対象レベルのサイズに変更します。
		bool readAsImage(Image& image, uint32 mipLevel = 0) const;

		/// @brief `R8_Unorm` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<uint8>& image, uint32 mipLevel = 0) const;

		/// @brief `R8G8_Unorm` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<uint16>& image, uint32 mipLevel = 0) const;

		/// @brief `R16_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<HalfFloat>& image, uint32 mipLevel = 0) const;

		/// @brief `R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<Color>& image, uint32 mipLevel = 0) const;

		/// @brief `R16G16_Unorm` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<std::pair<uint16, uint16>>& image, uint32 mipLevel = 0) const;

		/// @brief `R16G16_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<std::pair<HalfFloat, HalfFloat>>& image, uint32 mipLevel = 0) const;

		/// @brief `R32_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<float>& image, uint32 mipLevel = 0) const;

		/// @brief `R16G16B16A16_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<std::array<HalfFloat, 4>>& image, uint32 mipLevel = 0) const;

		/// @brief `R32G32_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<Float2>& image, uint32 mipLevel = 0) const;

		/// @brief `R32G32B32A32_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<Float4>& image, uint32 mipLevel = 0) const;

		/// @brief `R16G16_Unorm` / `R16G16_Float` / `R10G10B10A2_Unorm` / `R11G11B10_UFloat` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<uint32>& image, uint32 mipLevel = 0) const;

		/// @brief `R16G16B16A16_Float` を読み戻します。
		/// @param image 出力データ。同期・サイズ変更・失敗時の規則は readAsImage() と共通です。
		/// @param mipLevel ミップレベル
		/// @return 成功なら true, それ以外は false
		bool read(Grid<uint64>& image, uint32 mipLevel = 0) const;

		/// @brief 2 つのマルチサンプルレンダーテクスチャを交換します。
		/// @param other 交換する相手
		void swap(MSRenderTexture& other) noexcept;

		/// @brief 2 つのマルチサンプルレンダーテクスチャを交換します。
		/// @param lhs 一方のテクスチャ
		/// @param rhs もう一方のテクスチャ
		friend void swap(MSRenderTexture& lhs, MSRenderTexture& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}

# include "detail/MSRenderTexture.ipp"
