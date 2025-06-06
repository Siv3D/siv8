﻿//-----------------------------------------------
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
# include <Siv3D/IImageEncoder.hpp>

namespace s3d
{	
	////////////////////////////////////////////////////////////////
	//
	//	BCnEncoder
	//
	////////////////////////////////////////////////////////////////

	/// @brief BCn 形式画像のエンコーダ
	class BCnEncoder : public IImageEncoder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BCnEncoder();
	
		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief エンコーダの対応形式 `U"BCn"` を返します。
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
		//	possibleExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief BCn 形式のファイルに想定される拡張子一覧 `{ U"dds" }` を返します。
		/// @return 配列 `{ U"dds" }` 
		[[nodiscard]]
		const Array<String>& possibleExtensions() const override;
	
		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief Image を BC7 形式でエンコードしてファイルに保存します。
		/// @param image エンコードする Image
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(const Image& image, FilePathView path) const override;
	
		////////////////////////////////////////////////////////////////
		//
		//	encode
		//
		////////////////////////////////////////////////////////////////

		/// @brief Image を BC7 形式でエンコードして書き出します。
		/// @param image エンコードする Image
		/// @param writer 書き出し先の IWriter インタフェース
		/// @return 書き出しに成功した場合 true, それ以外の場合は false
		bool encode(const Image& image, IWriter& writer) const override;

		/// @brief Image を BC7 形式でエンコードした結果を Blob で返します。
		/// @param image エンコードする Image
		/// @return エンコード結果
		[[nodiscard]]
		Blob encode(const Image& image) const override;

		////////////////////////////////////////////////////////////////
		//
		//	encodeBC1
		//
		////////////////////////////////////////////////////////////////

		bool encodeBC1(const Image& image, IWriter& writer, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		[[nodiscard]]
		Blob encodeBC1(const Image& image, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	encodeBC3
		//
		////////////////////////////////////////////////////////////////

		bool encodeBC3(const Image& image, IWriter& writer, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		[[nodiscard]]
		Blob encodeBC3(const Image& image, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	encodeBC4
		//
		////////////////////////////////////////////////////////////////

		bool encodeBC4(const Image& image, IWriter& writer, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		[[nodiscard]]
		Blob encodeBC4(const Image& image, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	encodeBC5
		//
		////////////////////////////////////////////////////////////////

		bool encodeBC5(const Image& image, IWriter& writer, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		[[nodiscard]]
		Blob encodeBC5(const Image& image, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	encodeBC7
		//
		////////////////////////////////////////////////////////////////

		bool encodeBC7(const Image& image, IWriter& writer, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;

		[[nodiscard]]
		Blob encodeBC7(const Image& image, GenerateMipmaps generateMipmaps = GenerateMipmaps::Yes) const;
	};
}
