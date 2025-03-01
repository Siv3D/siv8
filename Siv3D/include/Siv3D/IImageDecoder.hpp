//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"
# include "ImageInfo.hpp"
# include "Image.hpp"
# include "Optional.hpp"
# include "Grid.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	class IReader;

	////////////////////////////////////////////////////////////////
	//
	//	IImageDecoder
	//
	////////////////////////////////////////////////////////////////

	struct IImageDecoder
	{		
		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~IImageDecoder() = default;
	
		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief デコーダの名前を返します。
		/// @return デコーダの名前
		[[nodiscard]]
		virtual StringView name() const noexcept = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	imageFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief デコーダがサポートする画像フォーマットを返します。
		/// @return デコーダがサポートする画像フォーマット
		[[nodiscard]]
		virtual ImageFormat imageFormat() const noexcept = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	isHeader
		//
		////////////////////////////////////////////////////////////////

		/// @brief 与えらえたバイト列が、このデコーダがサポートする画像フォーマットのヘッダであるかを返します。
		/// @param bytes バイト列
		/// @return 与えられたバイト列が、このデコーダがサポートする画像フォーマットのヘッダである場合 true, それ以外の場合は false
		[[nodiscard]]
		virtual bool isHeader(const uint8(&bytes)[16]) const noexcept = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief このデコーダがサポートする画像ファイルの拡張子の一覧を返します。
		/// @return このデコーダがサポートする画像ファイルの拡張子の一覧
		[[nodiscard]]
		virtual const Array<String>& possibleExtensions() const = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	getImageInfo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像ファイルの情報を取得します。
		/// @param path 画像ファイルのパス
		/// @return 画像ファイルの情報、このデコーダがサポートしていないフォーマットの場合は none
		[[nodiscard]]
		virtual Optional<ImageInfo> getImageInfo(FilePathView path) const;

		/// @brief 画像ファイルの情報を取得します。
		/// @param reader 画像ファイルをさす IReader
		/// @param pathHint 画像ファイルのパス
		/// @return 画像ファイルの情報、このデコーダがサポートしていないフォーマットの場合は none
		[[nodiscard]]
		virtual Optional<ImageInfo> getImageInfo(IReader& reader, FilePathView pathHint) const = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	decode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像ファイルをデコードします。
		/// @param path 画像ファイルのパス
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @return デコードされた画像
		[[nodiscard]]
		virtual Image decode(FilePathView path, PremultiplyAlpha premultiplyAlpha) const;

		/// @brief 画像ファイルをデコードします。
		/// @param reader IReader
		/// @param pathHint 画像ファイルのパス（わかる場合）
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @return デコードされた画像
		[[nodiscard]]
		virtual Image decode(IReader& reader, FilePathView pathHint, PremultiplyAlpha premultiplyAlpha) const = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	decodeGray16
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像ファイルを 16-bit グレースケール画像としてデコードします。
		/// @param path 画像ファイルのパス
		/// @return デコードされた 16-bit グレースケール画像の二次元配列、デコードに失敗した場合は空の配列
		/// @remark このデコーダが 16-bit グレースケール画像をサポートしていない場合は空の配列を返します。
		/// @remark 画像が 16-bit グレースケール画像でない場合は空の配列を返します。
		[[nodiscard]]
		virtual Grid<uint16> decodeGray16(FilePathView path) const;

		/// @brief 画像ファイルを 16-bit グレースケール画像としてデコードします。
		/// @param reader IReader
		/// @param pathHint 画像ファイルのパス（わかる場合）
		/// @return デコードされた 16-bit グレースケール画像の二次元配列、デコードに失敗した場合は空の配列
		/// @remark このデコーダが 16-bit グレースケール画像をサポートしていない場合は空の配列を返します。
		/// @remark 画像が 16-bit グレースケール画像でない場合は空の配列を返します。
		[[nodiscard]]
		virtual Grid<uint16> decodeGray16(IReader&, FilePathView) const;
	};
}
