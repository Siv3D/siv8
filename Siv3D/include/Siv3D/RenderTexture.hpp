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
# include <array>

namespace s3d
{
	/// @brief 深度を持たない、単一サンプルのレンダーテクスチャ
	/// @remark コピーは同じテクスチャを共有します。clear() は共有するすべてのハンドルに反映されます。
	/// @remark 非圧縮の色フォーマット、各辺 1～16384 ピクセルに対応します。不正なサイズ・フォーマット、非対応の設定、GPU テクスチャ・転送領域の確保失敗では空になります。
	/// @remark GPU コマンドの記録を開始できない場合は InternalEngineError を送出します。
	/// @remark 明示した format が sRGB の有無を決定します。Image から作成する場合は desc.sRGB を使います。
	/// @remark 初期データを指定して作成すると、ミップマップも初期化されます。clear() 後は必要に応じて generateMips() を呼んでください。
	class RenderTexture : public Texture
	{
	public:

		/// @brief 空のレンダーテクスチャを作成します。
		[[nodiscard]]
		RenderTexture() = default;

		/// @brief 全成分を 0 で初期化したレンダーテクスチャを作成します。
		/// @param size サイズ（ピクセル）
		/// @param format ピクセルフォーマット
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Size& size, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 指定した色で初期化したレンダーテクスチャを作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ色空間で指定します。
		/// @param format ピクセルフォーマット
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		RenderTexture(const Size& size, const ColorF& color, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 画像から RGBA8 のレンダーテクスチャを作成します。
		/// @param image 初期画像。バイト値は変換せずに転送します。
		/// @param desc ミップマップ、sRGB などの設定
		[[nodiscard]]
		explicit RenderTexture(const Image& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief パディングのない行を並べた生データからレンダーテクスチャを作成します。
		/// @param size サイズ（ピクセル）
		/// @param data 初期データ。空なら全成分を 0 に初期化します。それ以外は width × height × format.pixelSize() バイトが必要です。
		/// @param format ピクセルフォーマット
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		RenderTexture(const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<uint8>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<uint16>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<HalfFloat>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<std::pair<uint16, uint16>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<std::pair<HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<float>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<std::array<HalfFloat, 4>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<Float2>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32B32A32_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit RenderTexture(const Grid<Float4>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief ミップレベル 0 を指定した色でクリアします。
		/// @param color 線形の色。sRGB 形式では RGB が sRGB にエンコードされ、アルファは変換されません。
		/// @return *this
		/// @remark 空なら何もしません。既に要求した描画との順序を保ち、GPU 完了は待ちません。
		const RenderTexture& clear(const ColorF& color) const;

		/// @brief ミップレベル 0 から残りのミップマップを再生成します。
		/// @remark 空またはミップレベルが 1 つの場合は何もしません。GPU 完了は待ちません。
		void generateMips() const;

		/// @brief 指定したミップレベルを生バイト列に読み戻します。
		/// @param destination パディングのない行を格納する出力先。対象レベルの幅 × 高さ × pixelSize() バイトが必要です。
		/// @param mipLevel ミップレベル（0 が元のサイズ）
		/// @return 成功なら true。空、不正なレベル・出力サイズ、GPU 転送失敗では出力を変更せず false
		/// @remark GPU 転送の完了を待ちます。フォーマット変換や sRGB のデコードは行いません。
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

		/// @brief `R8_Unorm` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8_Unorm(const Grid<uint8>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8_Unorm` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8G8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8G8_Unorm(const Grid<uint16>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16_Float` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16_Float(const Grid<HalfFloat>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8G8B8A8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8G8B8A8_Unorm(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm_SRGB` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8G8B8A8_Unorm_SRGB(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm_SRGB` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR8G8B8A8_Unorm_SRGB(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16_Unorm(const Grid<std::pair<uint16, uint16>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16_Unorm(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16_Float(const Grid<std::pair<HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16_Float(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32_Float` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR32_Float(const Grid<float>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R10G10B10A2_Unorm` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR10G10B10A2_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R10G10B10A2_Unorm` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR10G10B10A2_Unorm(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R11G11B10_UFloat` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR11G11B10_UFloat(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R11G11B10_UFloat` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR11G11B10_UFloat(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16B16A16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16B16A16_Float(const Grid<std::array<HalfFloat, 4>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR16G16B16A16_Float(const Grid<uint64>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32_Float` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR32G32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR32G32_Float(const Grid<Float2>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32B32A32_Float` を指定した色で作成します。
		/// @param size サイズ（ピクセル）
		/// @param color 初期色。clear() と同じ規則を使います。
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR32G32B32A32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32B32A32_Float` の生データから作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成したレンダーテクスチャ
		[[nodiscard]]
		static RenderTexture CreateR32G32B32A32_Float(const Grid<Float4>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 別のレンダーテクスチャとハンドルを交換します。
		/// @param other 交換する相手
		void swap(RenderTexture& other) noexcept;

		/// @brief 2 つのレンダーテクスチャを交換します。
		/// @param lhs 一方のテクスチャ
		/// @param rhs もう一方のテクスチャ
		friend void swap(RenderTexture& lhs, RenderTexture& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}

# include "detail/RenderTexture.ipp"
