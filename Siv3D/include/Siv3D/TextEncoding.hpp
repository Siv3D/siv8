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
# include "StringView.hpp"

namespace s3d
{
	struct FormatData;
	class IReader;

	////////////////////////////////////////////////////////////////
	//
	//	TextEncoding
	//
	////////////////////////////////////////////////////////////////

	/// @brief テキストファイルのエンコーディング形式 | Text file encoding
	enum class TextEncoding : uint8
	{
		/// @brief UTF-8
		UTF8_NO_BOM,

		/// @brief UTF-8 with BOM
		UTF8_WITH_BOM,

		/// @brief UTF-16(LE) with BOM
		UTF16LE,

		/// @brief UTF-16(BE) with BOM
		UTF16BE,

		/// @brief デフォルト [UTF-8] | Default [UTF-8]
		Default = UTF8_NO_BOM,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Byte Order Mark (BOM)
	//
	////////////////////////////////////////////////////////////////

	/// @brief UTF-8 の BOM
	inline constexpr uint8 BOM_UTF8[3] = { 0xEF, 0xBB, 0xBF };

	/// @brief UTF-16LE の BOM
	inline constexpr uint8 BOM_UTF16LE[2] = { 0xFF, 0xFE };

	/// @brief UTF-16BE の BOM
	inline constexpr uint8 BOM_UTF16BE[2] = { 0xFE, 0xFF };

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief テキストファイルのエンコーディング形式を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value テキストファイルのエンコーディング形式
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, TextEncoding value);

	namespace Unicode
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetTextEncoding
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルがテキストファイルである場合、そのエンコーディング形式を返します。
		/// @param reader IReader
		/// @return テキストファイルのエンコーディング形式	
		[[nodiscard]]
		TextEncoding GetTextEncoding(const IReader& reader);

		/// @brief ファイルがテキストファイルである場合、そのエンコーディング形式を返します。
		/// @param path ファイルパス
		/// @return テキストファイルのエンコーディング形式
		[[nodiscard]]
		TextEncoding GetTextEncoding(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	GetBOMSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストファイルの BOM のサイズ（バイト）を返します。
		/// @param encoding エンコーディング形式
		/// @return テキストファイルの BOM のサイズ（バイト）
		[[nodiscard]]
		constexpr int32 GetBOMSize(TextEncoding encoding) noexcept;
	}
}

# include "detail/TextEncoding.ipp"
