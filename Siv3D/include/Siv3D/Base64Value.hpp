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
# include "String.hpp"
# include "Unicode.hpp"
# include "Result.hpp"

namespace s3d
{
	class Blob;

	////////////////////////////////////////////////////////////////
	//
	//	Base64Value
	//
	////////////////////////////////////////////////////////////////

	/// @brief Base64 値
	class Base64Value
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の Base64 値を作成します。
		[[nodiscard]]
		Base64Value() = default;

		/// @brief Base64 値を作成します。
		/// @param base64 Base64 値
		[[nodiscard]]
		explicit constexpr Base64Value(const char8* base64);

		/// @brief Base64 値を作成します。
		/// @param base64 Base64 値
		[[nodiscard]]
		explicit constexpr Base64Value(std::string_view base64);

		/// @brief Base64 値を作成します。
		/// @param base64 Base64 値
		[[nodiscard]]
		explicit constexpr Base64Value(const std::string& base64);

		/// @brief Base64 値を作成します。
		/// @param base64 Base64 値
		[[nodiscard]]
		explicit constexpr Base64Value(std::string&& base64);

		/// @brief Base64 値を作成します。
		/// @param base64 Base64 値
		/// @remark このオーバーロードは、入力を UTF-8 に変換するコストがかかります。
		[[nodiscard]]
		explicit Base64Value(StringView base64);

		////////////////////////////////////////////////////////////////
		//
		//	encodeFromMemory
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリ上のデータから Base64 値を作成します。
		/// @param src データの先頭ポインタ
		/// @param size データのサイズ（バイト）
		void encodeFromMemory(const void* src, size_t size);

		////////////////////////////////////////////////////////////////
		//
		//	encodeFromBlob
		//
		////////////////////////////////////////////////////////////////

		/// @brief Blob から Base64 値を作成します。
		/// @param blob Blob
		void encodeFromBlob(const Blob& blob);

		////////////////////////////////////////////////////////////////
		//
		//	encodeFromFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの内容から Base64 値を作成します。
		/// @param path ファイルパス
		void encodeFromFile(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	encodeFromUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 文字列から Base64 値を作成します。
		/// @param s UTF-8 文字列
		void encodeFromUTF8(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	encodeFromString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-8 に変換し、その結果から Base64 値を作成します。
		/// @param s 文字列
		void encodeFromString(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	getBase64
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値を返します。
		/// @return Base64 値
		[[nodiscard]]
		constexpr const std::string& getBase64() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値が空であるかを返します。
		/// @return Base64 値が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値が空でないかを返します。
		/// @return Base64 値が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMaxBinarySize
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値をデコードした結果として想定される、最大のバイナリデータのサイズ（バイト）を返します。
		/// @return バイナリデータのサイズ（バイト）
		/// @remark Base64 値が空白などを含まない場合は、デコード後のバイナリデータのサイズと一致します。
		[[nodiscard]]
		constexpr size_t getMaxBinarySize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	decodeToMemory
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値をデコードした結果をメモリに書き込みます。
		/// @param dst 格納先のメモリの先頭ポインタ
		/// @return デコードに成功した場合は書き込んだデータのサイズ、それ以外の場合は Base64 上のエラーの位置を示すエラー値
		/// @remark dst には少なくとも `.getBinarySize()` バイトの書き込み可能なメモリが確保されている必要があります。
		[[nodiscard]]
		Result<size_t, size_t> decodeToMemory(void* dst) const;

		////////////////////////////////////////////////////////////////
		//
		//	decodeToBlob
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値をデコードした結果を Blob に格納します。
		/// @param dst 格納先の Blob
		/// @return デコードに成功した場合は格納したデータのサイズ、それ以外の場合は Base64 上のエラーの位置を示すエラー値
		/// @return デコードに失敗した場合、dst は空の Blob になります。
		Result<size_t, size_t> decodeToBlob(Blob& dst) const;

		/// @brief Base64 値をデコードした結果を Blob で返します。
		/// @return デコードに成功した場合は Blob, それ以外の場合は空の Blob
		[[nodiscard]]
		Blob decodeToBlob() const;

		////////////////////////////////////////////////////////////////
		//
		//	decodeToFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値をデコードした結果をファイルに保存します。
		/// @param path 保存先のファイルパス
		/// @return デコードに成功した場合は保存したデータのサイズ、それ以外の場合は Base64 上のエラーの位置を示すエラー値
		Result<size_t, size_t> decodeToFile(FilePathView path) const;

		////////////////////////////////////////////////////////////////
		//
		//	decodeToUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値をデコードした結果を UTF-8 文字列に格納します。
		/// @param dst 格納先の UTF-8 文字列
		/// @return デコードに成功した場合は格納したデータのサイズ、それ以外の場合は Base64 上のエラーの位置を示すエラー値
		/// @remark デコードに失敗した場合、dst は空の文字列になります。
		[[nodiscard]]
		Result<size_t, size_t> decodeToUTF8(std::string& dst) const;

		/// @brief Base64 値をデコードした結果を UTF-8 文字列で返します。
		/// @return デコードに成功した場合は UTF-8 文字列、それ以外の場合は空の文字列
		[[nodiscard]]
		std::string decodeToUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	decodeToString
		//
		////////////////////////////////////////////////////////////////

		/// @brief Base64 値をデコードした結果を文字列に格納します。
		/// @param dst 格納先の文字列
		/// @return デコードに成功した場合は格納した文字数、それ以外の場合は Base64 上のエラーの位置を示すエラー値
		/// @remark デコードに失敗した場合、dst は空の文字列になります。
		[[nodiscard]]
		Result<size_t, size_t> decodeToString(String& dst) const;

		/// @brief Base64 値をデコードした結果を文字列で返します。
		/// @return デコードに成功した場合は文字列、それ以外の場合は空の文字列
		[[nodiscard]]
		String decodeToString() const;

		////////////////////////////////////////////////////////////////
		//
		//	EncodeFromMemory
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリ上のデータから Base64 値を作成します。
		/// @param src データの先頭ポインタ
		/// @param size データのサイズ（バイト）
		/// @return 作成された Base64 値
		[[nodiscard]]
		static Base64Value EncodeFromMemory(const void* src, size_t size);

		////////////////////////////////////////////////////////////////
		//
		//	EncodeFromBlob
		//
		////////////////////////////////////////////////////////////////

		/// @brief Blob から Base64 値を作成します。
		/// @param blob Blob
		/// @return 作成された Base64 値
		[[nodiscard]]
		static Base64Value EncodeFromBlob(const Blob& blob);

		////////////////////////////////////////////////////////////////
		//
		//	EncodeFromFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの内容から Base64 値を作成します。
		/// @param path ファイルパス
		/// @return 作成された Base64 値
		[[nodiscard]]
		static Base64Value EncodeFromFile(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	EncodeFromUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 文字列から Base64 値を作成します。
		/// @param s UTF-8 文字列
		/// @return 作成された Base64 値
		[[nodiscard]]
		static Base64Value EncodeFromUTF8(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	EncodeFromString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-8 に変換し、その結果から Base64 値を作成します。
		/// @param s 文字列
		/// @return 作成された Base64 値
		[[nodiscard]]
		static Base64Value EncodeFromString(StringView s);

	private:

		std::string m_base64;
	};
}

# include "detail/Base64Value.ipp"
