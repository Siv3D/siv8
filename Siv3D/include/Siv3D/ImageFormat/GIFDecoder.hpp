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

namespace s3d
{
	/// @brief GIF 形式画像データのデコーダ
	class GIFDecoder final : public IImageDecoder
	{
	public:

		/// @brief デコーダの対応形式 `U"GIF"` を返します。
		/// @return 文字列 `U"GIF"`
		[[nodiscard]]
		StringView name() const noexcept override;

		/// @brief GIF 形式を示す ImageFormat を返します。
		/// @return `ImageFormat::GIF`
		[[nodiscard]]
		ImageFormat imageFormat() const noexcept override;

		/// @brief GIF 形式の先頭データであるかを返します。
		/// @param bytes データの先頭 16 バイト
		/// @return GIF 形式と推測される場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept override;

		/// @brief GIF 形式のファイルに想定される拡張子一覧 `{ U"gif" }` を返します。
		/// @return 配列 `{ U"gif" }`
		[[nodiscard]]
		std::span<const StringView> possibleExtensions() const noexcept override;

		/// @brief GIF 形式の画像ファイルから画像情報を取得します。
		/// @param path GIF ファイルのパス
		/// @return 画像情報。取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> getImageInfo(FilePathView path) const override;

		/// @brief GIF 形式の画像データから画像情報を取得します。
		/// @param reader Reader オブジェクト
		/// @param pathHint ファイルパス（オプション）
		/// @return 画像情報。取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> getImageInfo(const IReader& reader, FilePathView pathHint = {}) const override;

		/// @brief GIF 形式の画像ファイルをデコードします。
		/// @param path GIF ファイルのパス
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @return デコードされた最初の合成済みフレーム
		[[nodiscard]]
		Image decode(FilePathView path, PremultiplyAlpha premultiplyAlpha) const override;

		/// @brief GIF 形式の画像データをデコードします。
		/// @param reader Reader オブジェクト
		/// @param pathHint ファイルパス（オプション）
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @return デコードされた最初の合成済みフレーム
		[[nodiscard]]
		Image decode(std::unique_ptr<IReader> reader, FilePathView pathHint, PremultiplyAlpha premultiplyAlpha) const override;
	};
}
