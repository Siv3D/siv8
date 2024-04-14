//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/IImageDecoder.hpp>

namespace s3d
{	
	////////////////////////////////////////////////////////////////
	//
	//	JPEGDecoder
	//
	////////////////////////////////////////////////////////////////

	/// @brief JPEG 形式画像データのデコーダ
	class JPEGDecoder : public IImageDecoder
	{
	public:
	
		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief デコーダの対応形式 `U"JPEG"` を返します。
		/// @return 文字列 `U"JPEG"`
		[[nodiscard]]
		StringView name() const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	imageFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief JPEG 形式を示す ImageFormat を返します。
		/// @return `ImageFormat::JPEG`
		[[nodiscard]]
		ImageFormat imageFormat() const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	isHeader
		//
		////////////////////////////////////////////////////////////////

		/// @brief JPEG 形式の先頭データであるかを返します。
		/// @param bytes データの先頭 16 バイト
		/// @return JPEG 形式と推測される場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isHeader(const uint8(&bytes)[16]) const noexcept override;
	
		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief JPEG 形式のファイルに想定される拡張子一覧 `{ U"jpg", U"jpeg" }` を返します。
		/// @return 配列 `{ U"jpg", U"jpeg" }`
		[[nodiscard]]
		const Array<String>& possibleExtensions() const override;
	
		////////////////////////////////////////////////////////////////
		//
		//	getImageInfo
		//
		////////////////////////////////////////////////////////////////

		/// @brief JPEG 形式の画像ファイルから画像情報を取得します。
		/// @param path 画像ファイルのパス
		/// @return 画像情報。取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> getImageInfo(FilePathView path) const override;

		/// @brief JPEG 形式の画像データから画像情報を取得します。
		/// @param reader 画像データの IReader インタフェース
		/// @param pathHint ファイルパス（オプション）
		/// @return 画像情報。取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> getImageInfo(IReader& reader, FilePathView pathHint = {}) const override;
	
		////////////////////////////////////////////////////////////////
		//
		//	decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief JPEG 形式の画像ファイルをデコードして Image を作成します。
		/// @param path 画像ファイルのパス
		/// @param premultiplyAlpha アルファ乗算処理を適用するか（JPEG では無視されます）
		/// @return 作成した Image
		[[nodiscard]]
		Image decode(FilePathView path, PremultiplyAlpha premultiplyAlpha) const override;

		/// @brief JPEG 形式の画像データをデコードして Image を作成します。
		/// @param reader 画像データの IReader インタフェース
		/// @param pathHint ファイルパス（オプション）
		/// @param premultiplyAlpha アルファ乗算処理を適用するか（JPEG では無視されます）
		/// @return 作成した Image
		[[nodiscard]]
		Image decode(IReader& reader, FilePathView pathHint, PremultiplyAlpha premultiplyAlpha) const override;
	};
}
