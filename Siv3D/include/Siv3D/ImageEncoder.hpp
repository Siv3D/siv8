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
# include "IImageEncoder.hpp"

namespace s3d
{
	namespace ImageEncoder
	{	
		////////////////////////////////////////////////////////////////
		//
		//	Save
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をエンコードしてファイルに保存します。
		/// @param image 画像
		/// @param format 画像フォーマット
		/// @param path 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool Save(const Image& image, ImageFormat format, FilePathView path);

		/// @brief 画像をエンコードしてファイルに保存します。
		/// @param image 画像
		/// @param encoderName 使用するエンコーダ名 
		/// @param path 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool Save(const Image& image, StringView encoderName, FilePathView path);
	
		////////////////////////////////////////////////////////////////
		//
		//	Encode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をエンコードします。
		/// @param image 画像
		/// @param format 画像フォーマット
		/// @param writer 画像ファイルを書き込む IWriter
		/// @return エンコードに成功した場合 true, それ以外の場合は false
		bool Encode(const Image& image, ImageFormat format, IWriter& writer);

		/// @brief 画像をエンコードします。
		/// @param image 画像
		/// @param encoderName 使用するエンコーダ名
		/// @param writer 画像ファイルを書き込む IWriter
		/// @return エンコードに成功した場合 true, それ以外の場合は false
		bool Encode(const Image& image, StringView encoderName, IWriter& writer);

		/// @brief 画像をエンコードします。
		/// @param image 画像
		/// @param format 画像フォーマット
		/// @return エンコードされた画像データ
		[[nodiscard]]
		Blob Encode(const Image& image, ImageFormat format);

		/// @brief 画像をエンコードします。
		/// @param image 画像
		/// @param encoderName 使用するエンコーダ名
		/// @return エンコードされた画像データ
		[[nodiscard]]
		Blob Encode(const Image& image, StringView encoderName);
	
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像エンコーダを登録します。
		/// @tparam ImageEncoder 画像エンコーダの型
		/// @return 登録に成功した場合 true, それ以外の場合は false
		template <class ImageEncoder> requires std::derived_from<ImageEncoder, IImageEncoder>
		bool Add();

		/// @brief 画像エンコーダを登録します。
		/// @param encoder 画像エンコーダ
		/// @return 登録に成功した場合 true, それ以外の場合は false
		bool Add(std::unique_ptr<IImageEncoder>&& encoder);
	
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像エンコーダを登録解除します。
		/// @param encoderName 画像エンコーダの名前
		void Remove(StringView encoderName);
	
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像エンコーダが登録されているかを返します。
		/// @param encoderName 画像エンコーダの名前
		/// @return 登録されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsRegistered(StringView encoderName);
	
		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されている画像エンコーダの一覧を返します。
		/// @return 登録されている画像エンコーダの一覧
		[[nodiscard]]
		const Array<std::unique_ptr<IImageEncoder>>& Enum();
	}
}

# include "detail/ImageEncoder.ipp"
