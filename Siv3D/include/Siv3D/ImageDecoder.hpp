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
# include "Common.hpp"
# include "IImageDecoder.hpp"

namespace s3d
{
	namespace ImageDecoder
	{	
		////////////////////////////////////////////////////////////////
		//
		//	GetImageInfo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像ファイルの情報を取得します。
		/// @param path 画像ファイルのパス
		/// @param imageFormat 画像フォーマット
		/// @return 画像ファイルの情報、取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> GetImageInfo(FilePathView path, ImageFormat imageFormat = ImageFormat::Unspecified);

		/// @brief 画像ファイルの情報を取得します。
		/// @param reader 画像ファイルをさす IReader
		/// @param imageFormat 画像フォーマット
		/// @return 画像ファイルの情報、取得に失敗した場合は none
		[[nodiscard]]
		Optional<ImageInfo> GetImageInfo(IReader& reader, ImageFormat imageFormat = ImageFormat::Unspecified);
	
		////////////////////////////////////////////////////////////////
		//
		//	Decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像ファイルをデコードします。
		/// @param path 画像ファイルのパス
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @param imageFormat 画像フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた画像、デコードに失敗した場合は空の画像
		[[nodiscard]]
		Image Decode(FilePathView path, PremultiplyAlpha premultiplyAlpha, ImageFormat imageFormat = ImageFormat::Unspecified);

		/// @brief 画像ファイルをデコードします。
		/// @param reader 画像ファイルをさす IReader
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @param imageFormat 画像フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた画像、デコードに失敗した場合は空の画像
		[[nodiscard]]
		Image Decode(IReader& reader, PremultiplyAlpha premultiplyAlpha, ImageFormat imageFormat = ImageFormat::Unspecified);
	
		////////////////////////////////////////////////////////////////
		//
		//	DecodeGray16
		//
		////////////////////////////////////////////////////////////////

		/// @brief 16-bit グレースケール画像をデコードします。
		/// @param path 画像ファイルのパス
		/// @param imageFormat 画像フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた 16-bit グレースケール画像、デコードに失敗した場合は空の画像
		[[nodiscard]]
		Grid<uint16> DecodeGray16(FilePathView path, ImageFormat imageFormat = ImageFormat::Unspecified);

		/// @brief 16-bit グレースケール画像をデコードします。
		/// @param reader 画像ファイルをさす IReader
		/// @param imageFormat 画像フォーマット、指定しない場合はファイルの拡張子から判断
		/// @return デコードされた 16-bit グレースケール画像、デコードに失敗した場合は空の画像
		[[nodiscard]]
		Grid<uint16> DecodeGray16(IReader& reader, ImageFormat imageFormat = ImageFormat::Unspecified);
	
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像デコーダを登録します。
		/// @tparam ImageDecoder 画像デコーダの型
		/// @return 登録に成功した場合 true, それ以外の場合は false
		template <class ImageDecoder> requires std::derived_from<ImageDecoder, IImageDecoder>
		bool Add();

		/// @brief 画像デコーダを登録します。
		/// @param decoder 画像デコーダ
		/// @return 登録に成功した場合 true, それ以外の場合は false
		bool Add(std::unique_ptr<IImageDecoder>&& decoder);
	
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像デコーダを登録解除します。
		/// @param decoderName 画像デコーダの名前
		void Remove(StringView decoderName);
	
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像デコーダが登録されているかを返します。
		/// @param decoderName 画像デコーダの名前
		/// @return 登録されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsRegistered(StringView decoderName) noexcept;
	
		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されている画像デコーダの一覧を返します。
		/// @return 登録されている画像デコーダの一覧
		[[nodiscard]]
		const Array<std::unique_ptr<IImageDecoder>>& Enum() noexcept;
	}
}

# include "detail/ImageDecoder.ipp"
