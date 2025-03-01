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
# include "ImageFormat.hpp"
# include "Image.hpp"
# include "Blob.hpp"

namespace s3d
{
	class IWriter;

	////////////////////////////////////////////////////////////////
	//
	//	IImageEncoder
	//
	////////////////////////////////////////////////////////////////

	struct IImageEncoder
	{	
		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~IImageEncoder() = default;
	
		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief エンコーダの名前を返します。
		/// @return エンコーダの名前
		[[nodiscard]]
		virtual StringView name() const noexcept = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	imageFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief エンコーダがサポートする画像フォーマットを返します。
		/// @return エンコーダがサポートする画像フォーマット
		[[nodiscard]]
		virtual ImageFormat imageFormat() const noexcept = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエンコーダがサポートするファイルの拡張子一覧を返します。
		/// @return このエンコーダがサポートするファイルの拡張子一覧
		[[nodiscard]]
		virtual const Array<String>& possibleExtensions() const = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をエンコードしてファイルに保存します。
		/// @param image 保存する画像
		/// @param path 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		virtual bool save(const Image& image, FilePathView path) const = 0;
	
		////////////////////////////////////////////////////////////////
		//
		//	encode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をエンコードします。
		/// @param image エンコードする画像
		/// @param writer 書き込み先の IWriter
		/// @return エンコードに成功した場合 true, それ以外の場合は false
		virtual bool encode(const Image& image, IWriter& writer) const = 0;

		/// @brief 画像をエンコードします。
		/// @param image エンコードする画像
		/// @return エンコードされたデータ
		[[nodiscard]]
		virtual Blob encode(const Image& image) const = 0;
	};
}
