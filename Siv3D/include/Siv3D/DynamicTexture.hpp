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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DynamicTexture
	//
	////////////////////////////////////////////////////////////////

	/// @brief 中身を更新できる、動的テクスチャクラス
	/// @remark テクスチャの中身を更新する場合、新しい Texture を毎回作成する代わりにこのクラスの `fill()` を使うのが効率的です。
	/// @remark 空の動的テクスチャに対して色指定以外の `fill()` または `fillIfNotBusy()` を呼ぶと、入力データから動的テクスチャを作成します。
	/// @remark `fill()` と `fillRegion()` は、前回の更新処理がビジー状態の場合、更新可能になるまで待機します。
	/// @remark `fillIfNotBusy()` と `fillRegionIfNotBusy()` は既存の動的テクスチャの更新待ちを行わず、更新処理がビジー状態の場合は false を返します。
	/// @remark ミップマップを持つ場合、更新後に必要に応じて `generateMips()` を呼び出してください。
	class DynamicTexture : public Texture
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ。空の動的テクスチャを作成します。
		[[nodiscard]]
		DynamicTexture() = default;

		/// @brief 指定したサイズとピクセルフォーマットの動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param format テクスチャのピクセルフォーマット
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Size& size, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 指定した色で塗りつぶされた動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param color 塗りつぶしの色
		/// @param format テクスチャのピクセルフォーマット
		/// @param desc テクスチャの設定
		[[nodiscard]]
		DynamicTexture(const Size& size, const ColorF& color, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 画像から `R8G8B8A8_Unorm` または `R8G8B8A8_Unorm_SRGB` 形式の動的テクスチャを作成します。
		/// @param image 画像
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Image& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R8_Unorm` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<uint8>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R16_Float` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<HalfFloat>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R8G8B8A8_Unorm` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<Color>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R16G16_Unorm` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<std::pair<uint16, uint16>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R16G16_Float` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<std::pair<HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R32_Float` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<float>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R16G16B16A16_Float` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R32G32_Float` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<Float2>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 1 要素を 1 ピクセルとして、`R32G32B32A32_Float` 形式の動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit DynamicTexture(const Grid<Float4>& image, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 動的テクスチャ全体を指定した色で更新します。
		/// @param color 塗りつぶしの色
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fill(const ColorF& color);

		/// @brief 動的テクスチャの中身を同じ大きさの画像で更新します。
		/// @remark 動的テクスチャが空の場合は新しい動的テクスチャを画像から作成します。
		/// @param image 画像
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Image& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R8_Unorm` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<uint8>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R16_Float` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<HalfFloat>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R16G16_Unorm` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<std::pair<uint16, uint16>>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R16G16_Float` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<std::pair<HalfFloat, HalfFloat>>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R8G8B8A8_Unorm` または `R8G8B8A8_Unorm_SRGB` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<Color>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R32_Float` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<float>& image);

		//bool fill(const Grid<uint32>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R16G16B16A16_Float` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R32G32_Float` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<Float2>& image);

		/// @brief 動的テクスチャの中身を同じ大きさの `R32G32B32A32_Float` データで更新します。
		/// @param image テクスチャの内容
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Grid<Float4>& image);

		////////////////////////////////////////////////////////////////
		//
		//	fillRegion
		//
		////////////////////////////////////////////////////////////////

		/// @brief 動的テクスチャの指定した領域を色で更新します。
		/// @param color 塗りつぶしの色
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const ColorF& color, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を画像で更新します。
		/// @param image 更新に使う画像。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Image& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R8_Unorm` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<uint8>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R16_Float` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<HalfFloat>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R16G16_Unorm` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<std::pair<uint16, uint16>>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R16G16_Float` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R8G8B8A8_Unorm` または `R8G8B8A8_Unorm_SRGB` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<Color>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R32_Float` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<float>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R16G16B16A16_Float` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R32G32_Float` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<Float2>& image, const Rect& rect);

		/// @brief 動的テクスチャの指定した領域を `R32G32B32A32_Float` データで更新します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, それ以外の場合は false
		bool fillRegion(const Grid<Float4>& image, const Rect& rect);

		////////////////////////////////////////////////////////////////
		//
		//	fillIfNotBusy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を画像で更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 画像。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Image& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R8_Unorm` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<uint8>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R16_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<HalfFloat>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R16G16_Unorm` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<std::pair<uint16, uint16>>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R16G16_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<std::pair<HalfFloat, HalfFloat>>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R8G8B8A8_Unorm` または `R8G8B8A8_Unorm_SRGB` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<Color>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R32_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<float>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R16G16B16A16_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R32G32_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<Float2>& image);

		/// @brief 動的テクスチャが更新可能な場合に限り、全体を `R32G32B32A32_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image テクスチャの内容。サイズは動的テクスチャと一致している必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillIfNotBusy(const Grid<Float4>& image);

		////////////////////////////////////////////////////////////////
		//
		//	fillRegionIfNotBusy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を画像で更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使う画像。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Image& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R8_Unorm` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<uint8>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R16_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<HalfFloat>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R16G16_Unorm` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<std::pair<uint16, uint16>>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R16G16_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R8G8B8A8_Unorm` または `R8G8B8A8_Unorm_SRGB` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<Color>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R32_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<float>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R16G16B16A16_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R32G32_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<Float2>& image, const Rect& rect);

		/// @brief 動的テクスチャが更新可能な場合に限り、指定した領域を `R32G32B32A32_Float` データで更新します。
		/// @remark 更新処理がビジー状態の場合は、更新を行わずに false を返します。
		/// @param image 更新に使うデータ。サイズは `rect.size` と一致している必要があります。
		/// @param rect 更新する領域。動的テクスチャの範囲内に収まっている必要があります。
		/// @return 更新に成功した場合 true, 更新処理がビジー状態または更新できない場合は false
		bool fillRegionIfNotBusy(const Grid<Float4>& image, const Rect& rect);

		////////////////////////////////////////////////////////////////
		//
		//	generateMips
		//
		////////////////////////////////////////////////////////////////

		/// @brief ミップマップを生成します。
		/// @remark この関数は、テクスチャの作成時に Mipmap を有効にした場合にのみ効果があります。
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

		/// @brief 2 つの DynamicTexture を入れ替えます。
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8_Unorm` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G 成分が格納された `R8G8_Unorm` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8B8A8_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR8G8B8A8_Unorm_SRGB(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R8G8B8A8_Unorm_SRGB` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Unorm` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Unorm(const Grid<std::pair<uint16, uint16>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G 成分が格納された `R16G16_Unorm` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16_Float(const Grid<std::pair<HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G 成分が格納された `R16G16_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR10G10B10A2_Unorm(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G, B, A 成分が格納された `R10G10B10A2_Unorm` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR11G11B10_UFloat(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G, B 成分が格納された `R11G11B10_UFloat` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16B16A16_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R16G16B16A16_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR16G16B16A16_Float(const Grid<std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>>& image, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief 各要素に R, G, B, A 成分が格納された `R16G16B16A16_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
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
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32B32A32_Float(const Size& size, const ColorF& color, TextureDesc desc = TextureDesc::NoMipmap);

		/// @brief `R32G32B32A32_Float` データから動的テクスチャを作成します。
		/// @param image テクスチャの内容
		/// @param desc テクスチャの設定
		/// @return 作成された動的テクスチャ
		[[nodiscard]]
		static DynamicTexture CreateR32G32B32A32_Float(const Grid<Float4>& image, TextureDesc desc = TextureDesc::NoMipmap);

	private:

		bool fillRegionImpl(std::span<const Byte> src, const Size& srcSize, uint32 srcBytesPerRow,
			const TextureFormat& expectedFormat, bool allowSRGB, const Rect& rect, bool wait);

	protected:

		/// @brief 指定したバイト列から動的テクスチャを作成します。
		/// @param size テクスチャのサイズ（ピクセル）
		/// @param data テクスチャの内容
		/// @param format テクスチャのピクセルフォーマット
		/// @param desc テクスチャの設定
		[[nodiscard]]
		DynamicTexture(const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);
	};
}
