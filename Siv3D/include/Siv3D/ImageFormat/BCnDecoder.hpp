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
# include <Siv3D/IImageDecoder.hpp>
# include <Siv3D/BCnData.hpp>

namespace s3d
{	
	////////////////////////////////////////////////////////////////
	//
	//	BCnDecoder
	//
	////////////////////////////////////////////////////////////////

	/// @brief BCn 形式画像データのデコーダ
	/// @remark decode() と decodeNative() は途中の段までのミップ列を受け付けます。宣言段数が 0 の場合は 1 段とみなし、画像の寸法から決まる最大段数を超える場合は失敗します。
	class BCnDecoder : public IImageDecoder
	{
	public:
	
		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief デコーダの対応形式 `U"BCn"` を返します。
		/// @return 文字列 `U"BCn"`
		[[nodiscard]]
		StringView name() const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	imageFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief BCn 形式を示す ImageFormat を返します。
		/// @return `ImageFormat::BCn`
		[[nodiscard]]
		ImageFormat imageFormat() const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	isHeader
		//
		////////////////////////////////////////////////////////////////

		/// @brief BCn 形式の先頭データであるかを返します。
		/// @param bytes データの先頭 16 バイト
		/// @return BCn 形式と推測される場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief BCn 形式のファイルに想定される拡張子一覧 `{ U"dds" }` を返します。
		/// @return 配列 `{ U"dds" }`
		[[nodiscard]]
		std::span<const StringView> possibleExtensions() const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	getImageInfo
		//
		////////////////////////////////////////////////////////////////

		/// @brief BCn 形式の画像ファイルから画像情報を取得します。
		/// @param path 画像ファイルのパス
		/// @return 画像情報。取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> getImageInfo(FilePathView path) const override;

		/// @brief BCn 形式の画像データから画像情報を取得します。
		/// @param reader Reader オブジェクト
		/// @param pathHint ファイルパス（オプション）
		/// @return 画像情報。取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> getImageInfo(const IReader& reader, FilePathView pathHint = {}) const override;
	
		////////////////////////////////////////////////////////////////
		//
		//	decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief BCn 形式の画像ファイルの基底段をデコードして Image を作成します。
		/// @param path 画像ファイルのパス
		/// @param premultiplyAlpha アルファ乗算処理を適用するか（BCn では無視されます）
		/// @return 基底段の画像。読み込みに失敗した場合は空の Image
		[[nodiscard]]
		Image decode(FilePathView path, PremultiplyAlpha premultiplyAlpha) const override;

		/// @brief BCn 形式の画像データの基底段をデコードして Image を作成します。
		/// @param reader Reader オブジェクト
		/// @param pathHint ファイルパス（オプション）
		/// @param premultiplyAlpha アルファ乗算処理を適用するか（BCn では無視されます）
		/// @return 基底段の画像。読み込みに失敗した場合は空の Image
		[[nodiscard]]
		Image decode(std::unique_ptr<IReader> reader, FilePathView pathHint, PremultiplyAlpha premultiplyAlpha) const override;

		////////////////////////////////////////////////////////////////
		//
		//	decodeNative
		//
		////////////////////////////////////////////////////////////////

		/// @brief DDS ファイルから宣言された全段の圧縮データを読み込みます。
		/// @param path 画像ファイルのパス
		/// @param sRGB 対応する形式を sRGB として扱うか
		/// @return 圧縮データ。いずれかの段を最後まで読み込めない場合やヘッダが不正な場合は空の BCnData
		[[nodiscard]]
		BCnData decodeNative(FilePathView path, bool sRGB) const;

		/// @brief DDS 形式の画像データから宣言された全段の圧縮データを読み込みます。
		/// @param reader Reader オブジェクト
		/// @param sRGB 対応する形式を sRGB として扱うか
		/// @param pathHint ファイルパス（オプション）
		/// @return 圧縮データ。読み込みに失敗した場合は空の BCnData
		[[nodiscard]]
		BCnData decodeNative(std::unique_ptr<IReader> reader, bool sRGB, FilePathView pathHint = {}) const;
	};
}
