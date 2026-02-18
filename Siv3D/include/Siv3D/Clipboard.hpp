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
# include "String.hpp"
# include "Array.hpp"
# include "Image.hpp"
# include "Optional.hpp"
# include "Blob.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	namespace Clipboard
	{
		////////////////////////////////////////////////////////////////
		//
		//	HasChanged
		//
		////////////////////////////////////////////////////////////////

		/// @brief クリップボードの内容が前回の呼び出し以降に変更されたかを返します。 | Returns whether the clipboard contents have changed since the previous call.
		/// @remark 内部的に OS のシーケンス番号を参照し、この関数を呼ぶたびに比較基準が更新されます。 | Internally uses the OS sequence number and updates the baseline each time this function is called.
		/// @return 変更されていれば true, そうでなければ false | true if changed, otherwise false
		[[nodiscard]]
		bool HasChanged();

		////////////////////////////////////////////////////////////////
		//
		//	GetSequenceNumber
		//
		////////////////////////////////////////////////////////////////

		/// @brief クリップボードのシーケンス番号を返します。 | Returns the clipboard sequence number.
		/// @remark シーケンス番号は、クリップボードの内容が変化するたびに増加します（OS 依存）。 | The sequence number increases whenever the clipboard contents change (OS-dependent).
		/// @return クリップボードのシーケンス番号 | Clipboard sequence number
		[[nodiscard]]
		uint64 GetSequenceNumber();

		////////////////////////////////////////////////////////////////
		//
		//	Clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief クリップボードの内容を消去します。 | Clears the clipboard contents.
		void Clear();

		////////////////////////////////////////////////////////////////
		//
		//	SetText, GetText, HasText
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストをクリップボードにセットします。 | Sets the text to the clipboard.
		/// @param text テキスト | Text
		void SetText(StringView text);

		/// @brief クリップボードからテキストを取得します。 | Retrieves text from the clipboard.
		/// @return テキストを取得できた場合はその値、取得できない場合は none | The text if available, otherwise none
		[[nodiscard]]
		Optional<String> GetText();

		/// @brief クリップボードからテキストを取得します。 | Retrieves text from the clipboard.
		/// @param text 取得したテキストの格納先 | Destination for the retrieved text
		/// @return 取得に成功した場合 true, 失敗した場合 false | true on success, otherwise false
		bool GetText(String& text);

		/// @brief クリップボードにテキストが含まれているかを返します。 | Returns whether the clipboard contains text.
		/// @return テキストが含まれていれば true, そうでなければ false | true if text is available, otherwise false
		[[nodiscard]]
		bool HasText();

		////////////////////////////////////////////////////////////////
		//
		//	SetImage, GetImage, HasImage
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をクリップボードにセットします。 | Sets the image to the clipboard.
		/// @param image 画像（非アルファ乗算） | Image (non-premultiplied alpha)
		void SetImage(const Image& image);

		/// @brief クリップボードから画像を取得します。 | Retrieves an image from the clipboard.
		/// @param premultiplyAlpha 取得した画像をアルファ乗算するかどうか | Whether to premultiply the alpha of the retrieved image
		/// @return 画像を取得できた場合はその値、取得できない場合は none | The image if available, otherwise none
		[[nodiscard]]
		Optional<Image> GetImage(PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes);

		/// @brief クリップボードから画像を取得します。 | Retrieves an image from the clipboard.
		/// @param image 取得した画像の格納先 | Destination for the retrieved image
		/// @param premultiplyAlpha 取得した画像をアルファ乗算するかどうか | Whether to premultiply the alpha of the retrieved image
		/// @return 取得に成功した場合 true, 失敗した場合 false | true on success, otherwise false
		bool GetImage(Image& image, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes);

		/// @brief クリップボードに画像が含まれているかを返します。 | Returns whether the clipboard contains an image.
		/// @return 画像が含まれていれば true, そうでなければ false | true if an image is available, otherwise false
		[[nodiscard]]
		bool HasImage();

		////////////////////////////////////////////////////////////////
		//
		//	SetFilePaths, GetFilePaths, HasFilePaths
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルパスの一覧をクリップボードにセットします。 | Sets a list of file paths to the clipboard.
		/// @param paths ファイルパスの一覧 | List of file paths
		void SetFilePaths(const Array<FilePath>& paths);

		/// @brief クリップボードからファイルパスの一覧を取得します。 | Retrieves a list of file paths from the clipboard.
		/// @return 取得できた場合はその値、取得できない場合は none | The list of file paths if available, otherwise none
		[[nodiscard]]
		Optional<Array<FilePath>> GetFilePaths();

		/// @brief クリップボードからファイルパスの一覧を取得します。 | Retrieves a list of file paths from the clipboard.
		/// @param paths 取得したファイルパスの一覧の格納先 | Destination for the retrieved list of file paths
		/// @return 取得に成功した場合 true, 失敗した場合 false | true on success, otherwise false
		bool GetFilePaths(Array<FilePath>& paths);

		/// @brief クリップボードにファイルパスの一覧が含まれているかを返します。 | Returns whether the clipboard contains a list of file paths.
		/// @return ファイルパスの一覧が含まれていれば true, そうでなければ false | true if file paths are available, otherwise false
		[[nodiscard]]
		bool HasFilePaths();

		////////////////////////////////////////////////////////////////
		//
		//	SetRichText
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief リッチテキスト（RTF）をクリップボードにセットします。 | Sets rich text (RTF) to the clipboard.
		/// @param richText リッチテキスト（RTF） | Rich text (RTF)
		/// @param plainTextFallback 取得先アプリが RTF を扱えない場合に使われるプレーンテキスト（省略可） | Plain-text fallback used when the target application cannot consume RTF (optional)
		void SetRichText(StringView richText, StringView plainTextFallback = U"");

		////////////////////////////////////////////////////////////////
		//
		//	SetHTML
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief HTML をクリップボードにセットします。 | Sets HTML to the clipboard.
		/// @param html HTML | HTML
		/// @param plainTextFallback 取得先アプリが HTML を扱えない場合に使われるプレーンテキスト（省略可） | Plain-text fallback used when the target application cannot consume HTML (optional)
		void SetHTML(StringView html, StringView plainTextFallback = U"");

		////////////////////////////////////////////////////////////////
		//
		//	SetData, GetData
		//
		////////////////////////////////////////////////////////////////

		/// @brief 任意のバイナリデータを MIME タイプ付きでクリップボードにセットします。 | Sets arbitrary binary data to the clipboard with a MIME type.
		/// @param mimeType MIME タイプ（例: `application/octet-stream`） | MIME type (e.g., `application/octet-stream`)
		/// @param data データ先頭へのポインタ | Pointer to the data
		/// @param size データサイズ（バイト） | Data size in bytes
		void SetData(StringView mimeType, const void* data, size_t size);

		/// @brief 任意のバイナリデータを MIME タイプ付きでクリップボードにセットします。 | Sets arbitrary binary data to the clipboard with a MIME type.
		/// @param mimeType MIME タイプ（例: `application/octet-stream`） | MIME type (e.g., `application/octet-stream`)
		/// @param data データ | Data
		void SetData(StringView mimeType, std::span<const Byte> data);

		/// @brief 指定した MIME タイプのデータをクリップボードから取得します。 | Retrieves clipboard data for the specified MIME type.
		/// @param mimeType MIME タイプ | MIME type
		/// @return データが取得できた場合はその値、取得できない場合は none | The data if available, otherwise none
		[[nodiscard]]
		Optional<Blob> GetData(StringView mimeType);

		/// @brief 指定した MIME タイプのデータをクリップボードから取得します。 | Retrieves clipboard data for the specified MIME type.
		/// @param mimeType MIME タイプ | MIME type
		/// @param data 取得したデータの格納先 | Destination for the retrieved data
		/// @return 取得に成功した場合 true, 失敗した場合 false | true on success, otherwise false
		bool GetData(StringView mimeType, Blob& data);

		////////////////////////////////////////////////////////////////
		//
		//	GetAvailableMimeTypes
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 現在のクリップボードから取得可能な MIME タイプの一覧を返します。 | Returns a list of MIME types currently available from the clipboard.
		/// @return MIME タイプの一覧 | List of available MIME types
		[[nodiscard]]
		Array<String> GetAvailableMimeTypes();
	}
}
