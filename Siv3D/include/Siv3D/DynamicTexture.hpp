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
# include "Texture.hpp"
# include <array>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DynamicTexture
	//
	////////////////////////////////////////////////////////////////

	/// @brief CPU 側の画像データで内容を更新できるテクスチャ
	/// @par 入力と形式
	/// Image / Grid / 生データは色変換せずにコピーします。入力は呼び出し後に変更・破棄できます。
	/// Image からの作成では desc.sRGB、それ以外では指定した形式が sRGB の有無を決定します。
	/// @anchor dynamic_texture_update_contract
	/// @par 更新の共通規則
	/// 既存のテクスチャではサイズと形式を保持し、ミップレベル 0 を更新します。領域更新では領域外を保持します。
	/// fill() と fillRegion() は転送領域がビジーなら再利用可能になるまで待ちます。
	/// fillIfNotBusy() と fillRegionIfNotBusy() はビジーなら待機・更新せず false を返します。
	/// Image / Grid を受け取る全体更新は、空のテクスチャに対しては入力から既定の設定で作成を試みます。
	/// 作成に失敗した場合は空のまま false を返します。
	/// 色・生データによる全体更新とすべての領域更新は、空のテクスチャに対して false を返します。
	/// @par 描画とミップマップ
	/// 更新とミップ生成は保留中の 2D 描画を発行しません。以前の内容を使う描画は呼び出し前に Graphics2D::Flush() で発行します。
	/// 更新は発行済みの描画より後に適用されます。更新の成功は GPU 処理の完了を意味しません。
	/// 更新時にミップマップを再生成しません。必要に応じて generateMips() を呼び出します。
	class DynamicTexture : public Texture
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の動的テクスチャを作成します。
		[[nodiscard]]
		DynamicTexture() = default;

		/// @brief 指定したサイズとピクセルフォーマットの動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param format 非圧縮のピクセルフォーマット
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Size& size, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 指定した色で塗りつぶされた動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param format 非圧縮のピクセルフォーマット
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		DynamicTexture(const Size& size, const ColorF& color, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 画像から `R8G8B8A8_Unorm` または `R8G8B8A8_Unorm_SRGB` 形式の動的テクスチャを作成します。
		/// @param image 初期画像
		/// @param desc ミップマップ、sRGB などの設定
		[[nodiscard]]
		explicit DynamicTexture(const Image& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8_Unorm` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<uint8>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8_Unorm` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<uint16>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16_Float` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<HalfFloat>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<std::pair<uint16, uint16>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<std::pair<HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32_Float` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<float>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<std::array<HalfFloat, 4>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32_Float` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<Float2>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32B32A32_Float` 形式の動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<Float4>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャ全体を指定した色で更新します。
		/// @param color 塗りつぶしの色
		/// @return 成功なら true。空のテクスチャ、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const ColorF& color);

		/// @brief 生データでテクスチャ全体を更新します。
		/// @anchor dynamic_texture_raw_contract
		/// @param data テクスチャと同じ非圧縮形式の入力データ。必要量は (高さ - 1) × sourceRowPitch + 1 行の画素バイト数以上。
		/// @param sourceRowPitch 入力の行間隔（バイト）。1 行の画素バイト数以上が必要です。
		/// @return 成功なら true。空のテクスチャ、不正な入力、転送失敗では false
		/// @remark 入力サイズは size()、領域更新では rect.size です。行末の余白はコピーせず、最終行の末尾パディングは不要です。
		/// @see @ref dynamic_texture_update_contract
		bool fill(std::span<const Byte> data, uint32 sourceRowPitch);

		/// @brief `R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Image& image);

		/// @brief `R8_Unorm` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<uint8>& image);

		/// @brief `R8G8_Unorm` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<uint16>& image);

		/// @brief `R16_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<HalfFloat>& image);

		/// @brief `R16G16_Unorm` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<std::pair<uint16, uint16>>& image);

		/// @brief `R16G16_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<std::pair<HalfFloat, HalfFloat>>& image);

		/// @brief `R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<Color>& image);

		/// @brief `R32_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<float>& image);

		//bool fill(const Grid<uint32>& image);

		/// @brief `R16G16B16A16_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<std::array<HalfFloat, 4>>& image);

		/// @brief `R32G32_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<Float2>& image);

		/// @brief `R32G32B32A32_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fill(const Grid<Float4>& image);

		////////////////////////////////////////////////////////////////
		//
		//	fillRegion
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャの指定領域を指定した色で更新します。
		/// @param color 塗りつぶしの色
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const ColorF& color, const Rect& rect);

		/// @brief 生データでテクスチャの指定領域を更新します。
		/// @param data rect.size の入力データ。形式と必要量は @ref dynamic_texture_raw_contract に従います。
		/// @param sourceRowPitch 入力の行間隔（バイト）。1 行の画素バイト数以上が必要です。
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、不正な入力・領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(std::span<const Byte> data, uint32 sourceRowPitch, const Rect& rect);

		/// @brief `R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Image& image, const Rect& rect);

		/// @brief `R8_Unorm` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<uint8>& image, const Rect& rect);

		/// @brief `R8G8_Unorm` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<uint16>& image, const Rect& rect);

		/// @brief `R16_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<HalfFloat>& image, const Rect& rect);

		/// @brief `R16G16_Unorm` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<std::pair<uint16, uint16>>& image, const Rect& rect);

		/// @brief `R16G16_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const Rect& rect);

		/// @brief `R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<Color>& image, const Rect& rect);

		/// @brief `R32_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<float>& image, const Rect& rect);

		/// @brief `R16G16B16A16_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<std::array<HalfFloat, 4>>& image, const Rect& rect);

		/// @brief `R32G32_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<Float2>& image, const Rect& rect);

		/// @brief `R32G32B32A32_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegion(const Grid<Float4>& image, const Rect& rect);

		////////////////////////////////////////////////////////////////
		//
		//	fillIfNotBusy
		//
		////////////////////////////////////////////////////////////////

		/// @brief ビジーでなければ、テクスチャ全体を指定した色で更新します。
		/// @param color 塗りつぶしの色
		/// @return 成功なら true。ビジー状態、空のテクスチャ、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const ColorF& color);

		/// @brief ビジーでなければ、`R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Image& image);

		/// @brief ビジーでなければ、生データでテクスチャ全体を更新します。
		/// @param data size() の入力データ。形式と必要量は @ref dynamic_texture_raw_contract に従います。
		/// @param sourceRowPitch 入力の行間隔（バイト）。1 行の画素バイト数以上が必要です。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、不正な入力、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(std::span<const Byte> data, uint32 sourceRowPitch);

		/// @brief ビジーでなければ、`R8_Unorm` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<uint8>& image);

		/// @brief ビジーでなければ、`R8G8_Unorm` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<uint16>& image);

		/// @brief ビジーでなければ、`R16_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<HalfFloat>& image);

		/// @brief ビジーでなければ、`R16G16_Unorm` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<std::pair<uint16, uint16>>& image);

		/// @brief ビジーでなければ、`R16G16_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<std::pair<HalfFloat, HalfFloat>>& image);

		/// @brief ビジーでなければ、`R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<Color>& image);

		/// @brief ビジーでなければ、`R32_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<float>& image);

		/// @brief ビジーでなければ、`R16G16B16A16_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<std::array<HalfFloat, 4>>& image);

		/// @brief ビジーでなければ、`R32G32_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<Float2>& image);

		/// @brief ビジーでなければ、`R32G32B32A32_Float` 形式のテクスチャ全体を更新します。
		/// @param image 既存のテクスチャと同サイズの入力データ
		/// @return 更新または作成に成功した場合 true, それ以外は false
		/// @see @ref dynamic_texture_update_contract
		bool fillIfNotBusy(const Grid<Float4>& image);

		////////////////////////////////////////////////////////////////
		//
		//	fillRegionIfNotBusy
		//
		////////////////////////////////////////////////////////////////

		/// @brief ビジーでなければ、テクスチャの指定領域を指定した色で更新します。
		/// @param color 塗りつぶしの色
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const ColorF& color, const Rect& rect);

		/// @brief ビジーでなければ、`R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Image& image, const Rect& rect);

		/// @brief ビジーでなければ、生データでテクスチャの指定領域を更新します。
		/// @param data rect.size の入力データ。形式と必要量は @ref dynamic_texture_raw_contract に従います。
		/// @param sourceRowPitch 入力の行間隔（バイト）。1 行の画素バイト数以上が必要です。
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、不正な入力・領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(std::span<const Byte> data, uint32 sourceRowPitch, const Rect& rect);

		/// @brief ビジーでなければ、`R8_Unorm` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<uint8>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R8G8_Unorm` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<uint16>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R16_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<HalfFloat>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R16G16_Unorm` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<std::pair<uint16, uint16>>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R16G16_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R8G8B8A8_Unorm` / `R8G8B8A8_Unorm_SRGB` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<Color>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R32_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<float>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R16G16B16A16_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<std::array<HalfFloat, 4>>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R32G32_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<Float2>& image, const Rect& rect);

		/// @brief ビジーでなければ、`R32G32B32A32_Float` 形式のテクスチャの指定領域を更新します。
		/// @param image rect.size と同サイズの入力データ
		/// @param rect 更新領域。幅・高さが正で、テクスチャ内に収まる必要があります。
		/// @return 成功なら true。ビジー状態、空のテクスチャ、サイズ・形式の不一致、不正な領域、転送失敗では false
		/// @see @ref dynamic_texture_update_contract
		bool fillRegionIfNotBusy(const Grid<Float4>& image, const Rect& rect);

		////////////////////////////////////////////////////////////////
		//
		//	generateMips
		//
		////////////////////////////////////////////////////////////////

		/// @brief ミップレベル 0 から残りのミップマップを再生成します。
		/// @remark 空またはミップレベルが 1 つの場合は何もしません。GPU 完了は待ちません。描画との順序は更新の共通規則に従います。
		/// @see @ref dynamic_texture_update_contract
		void generateMips() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の DynamicTexture と内容を交換します。
		/// @param other 別の DynamicTexture
		void swap(DynamicTexture& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの DynamicTexture の内容を交換します。
		/// @param lhs 一方の DynamicTexture
		/// @param rhs もう一方の DynamicTexture
		friend void swap(DynamicTexture& lhs, DynamicTexture& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	CreateR8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R8_Unorm` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8_Unorm` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8_Unorm(const Grid<uint8>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR8G8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R8G8_Unorm` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R, G 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G 成分が格納された `R8G8_Unorm` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8_Unorm(const Grid<uint16>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R16_Float` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16_Float(const Grid<HalfFloat>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR8G8B8A8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R8G8B8A8_Unorm` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8B8A8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8B8A8_Unorm(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR8G8B8A8_Unorm_SRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R8G8B8A8_Unorm_SRGB` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8B8A8_Unorm_SRGB(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm_SRGB` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8B8A8_Unorm_SRGB(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR16G16_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R16G16_Unorm` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R, G 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Unorm(const Grid<std::pair<uint16, uint16>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G 成分が格納された `R16G16_Unorm` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Unorm(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR16G16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R16G16_Float` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R, G 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Float(const Grid<std::pair<HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G 成分が格納された `R16G16_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Float(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R32_Float` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32_Float(const Grid<float>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR10G10B10A2_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R10G10B10A2_Unorm` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR10G10B10A2_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G, B, A 成分が格納された `R10G10B10A2_Unorm` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR10G10B10A2_Unorm(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR11G11B10_UFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R11G11B10_UFloat` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R, G, B 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR11G11B10_UFloat(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G, B 成分が格納された `R11G11B10_UFloat` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR11G11B10_UFloat(const Grid<uint32>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR16G16B16A16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R16G16B16A16_Float` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16B16A16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16B16A16_Float(const Grid<std::array<HalfFloat, 4>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G, B, A 成分が格納された `R16G16B16A16_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16B16A16_Float(const Grid<uint64>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR32G32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R32G32_Float` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色。R, G 成分を使用します。
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32_Float(const Grid<Float2>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	CreateR32G32B32A32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した色で塗りつぶされた `R32G32B32A32_Float` 形式の動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32B32A32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32B32A32_Float` データから動的テクスチャを作成します。
		/// @param image 1 要素を 1 ピクセルとする初期データ
		/// @param desc ミップマップなどの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32B32A32_Float(const Grid<Float4>& image, TextureDesc desc = TextureDesc::NoMipmap);

	private:

		bool fillRegionImpl(std::span<const Byte> src, const Size& srcSize, uint32 srcBytesPerRow,
			const TextureFormat& expectedFormat, bool allowSRGB, const Rect& rect, bool wait);

	protected:

		/// @brief 指定した形式の生データから動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param data 初期データ。各行を format.bytesPerRow(size.x) バイトで格納します。空なら初期データを指定しません。
		/// @param format 非圧縮のピクセルフォーマット
		/// @param desc ミップマップなどの設定
		[[nodiscard]]
		DynamicTexture(const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);
	};
}
