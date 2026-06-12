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
# include <memory>
# include "Common.hpp"
# include "IReader.hpp"
# include "StringView.hpp"
# include "Optional.hpp"

namespace s3d
{
	class String;
	using FilePath = String;
	class Blob;

	////////////////////////////////////////////////////////////////
	//
	//	BinaryFileReader
	//
	////////////////////////////////////////////////////////////////

	/// @brief 読み込み用バイナリファイル
	class BinaryFileReader : public IReader
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		BinaryFileReader();

		/// @brief ファイルを開きます。
		/// @param path ファイルパス
		[[nodiscard]]
		explicit BinaryFileReader(FilePathView path);

		BinaryFileReader(const BinaryFileReader& other) = delete;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする BinaryFileReader
		BinaryFileReader(BinaryFileReader&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		~BinaryFileReader() override;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		BinaryFileReader& operator =(const BinaryFileReader& other) = delete;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする BinaryFileReader
		/// @return *this
		BinaryFileReader& operator =(BinaryFileReader&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	supportsLookahead
		//
		////////////////////////////////////////////////////////////////

		/// @brief lookahead をサポートしているかを返します。
		/// @return true
		[[nodiscard]]
		constexpr bool supportsLookahead() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを開きます。
		/// @param path ファイルパス
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		bool open(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	close
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを閉じます。
		/// @remark ファイルが開いていない場合は何もしません。
		void close();

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルが開いているかを返します。
		/// @return ファイルが開いている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルが開いているかを返します。
		/// @return ファイルが開いている場合 true, それ以外の場合は false	
		[[nodiscard]]
		explicit operator bool() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルのサイズを返します。
		/// @return ファイルのサイズ（バイト）
		/// @remark ファイルサイズは open() 時点で取得されます。
		/// @remark open() 後に外部からファイルが変更された場合の読み込み結果は保証されません。
		[[nodiscard]]
		int64 size() const override;

		////////////////////////////////////////////////////////////////
		//
		//	getPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の読み込み位置を返します。
		/// @return 現在の読み込み位置（バイト）		
		[[nodiscard]]
		int64 getPos() const override;

		////////////////////////////////////////////////////////////////
		//
		//	remaining
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の読み込み位置からファイル終端までの残りサイズを返します。
		/// @return 残りサイズ（バイト）
		[[nodiscard]]
		int64 remaining() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEOF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の読み込み位置がファイル終端に達しているかを返します。
		/// @return ファイル終端に達している場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEOF() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 読み込み位置を変更します。
		/// @param pos 新しい読み込み位置（バイト）
		/// @return 新しい読み込み位置
		int64 setPos(int64 pos) override;

		////////////////////////////////////////////////////////////////
		//
		//	skip
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを読み飛ばし、読み込み位置を変更します。
		/// @param offset 読み飛ばすサイズ（バイト）
		/// @return 新しい読み込み位置
		int64 skip(int64 offset) override;

		////////////////////////////////////////////////////////////////
		//
		//	read
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルから指定したサイズまでのデータを読み込みます。
		/// @param dst 読み込み先の先頭ポインタ
		/// @param size 読み込む最大サイズ（バイト）
		/// @return 実際に読み込んだサイズ（バイト）
		/// @remark size が 0 以下の場合は 0 を返し、読み込み位置は変更されません。
		/// @remark size が 0 より大きく、dst が nullptr の場合は Error を投げます。
		/// @remark 実際に読み込んだサイズは、ファイル終端や読み込みエラーにより size より小さくなる場合があります。
		/// @remark 読み込み位置は実際に読み込んだサイズ分だけ進みます。
		int64 read(void* dst, int64 size) override;

		/// @brief ファイルの指定した位置から、指定したサイズまでのデータを読み込みます。
		/// @param dst 読み込み先の先頭ポインタ
		/// @param pos 読み込み開始位置（バイト）
		/// @param size 読み込む最大サイズ（バイト）
		/// @return 実際に読み込んだサイズ（バイト）
		/// @remark size が 0 以下の場合は 0 を返し、読み込み位置は変更されません。この場合、dst および pos は検証されません。
		/// @remark size が 0 より大きく、dst が nullptr の場合は Error を投げます。
		/// @remark size が 0 より大きく、pos が 0 以上ファイルサイズ以下の範囲にない場合は Error を投げます。
		/// @remark 実際に読み込んだサイズは、ファイル終端や読み込みエラーにより size より小さくなる場合があります。
		/// @remark 読み込み位置は pos + 実際に読み込んだサイズに移動します。
		int64 read(void* dst, int64 pos, int64 size) override;

		/// @brief ファイルから値を読み込みます。
		/// @param dst 読み込み先の値
		/// @return sizeof(dst) バイトをすべて読み込めた場合 true, それ以外の場合は false
		/// @remark 読み込みに成功すると、読み込み位置は sizeof(dst) バイト進みます。
		/// @remark 読み込みに失敗した場合でも、部分的に読み込まれたバイト数だけ読み込み位置が進むことがあります。
		/// @remark 読み込みに失敗した場合、dst の内容は未規定です。
		[[nodiscard]]
		bool read(Concept::TriviallyCopyable auto& dst);

		/// @brief ファイルから値を読み込みます。
		/// @tparam Type 読み込む値の型
		/// @return sizeof(Type) バイトをすべて読み込めた場合は読み込んだ値。それ以外の場合は none
		/// @remark 読み込みに成功すると、読み込み位置は sizeof(Type) バイト進みます。
		/// @remark 読み込みに失敗した場合でも、部分的に読み込まれたバイト数だけ読み込み位置が進むことがあります。
		template <Concept::TriviallyCopyable Type>
		[[nodiscard]]
		Optional<Type> read();

		////////////////////////////////////////////////////////////////
		//
		//	readExact
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルから指定したサイズのデータを読み込みます。
		/// @param dst 読み込み先の先頭ポインタ
		/// @param size 読み込むサイズ（バイト）
		/// @return 指定したサイズのデータをすべて読み込めた場合 true, それ以外の場合は false
		/// @remark size が 0 以下の場合は true を返し、読み込み位置は変更されません。
		/// @remark 読み込みに成功すると、読み込み位置は現在位置 + size に移動します。
		/// @remark 読み込みに失敗した場合、読み込み位置は変更されません。
		/// @remark 読み込みに失敗した場合、dst が指すメモリの内容は未規定です。
		[[nodiscard]]
		bool readExact(void* dst, int64 size);

		/// @brief ファイルの指定した位置から、指定したサイズのデータを読み込みます。
		/// @param dst 読み込み先の先頭ポインタ
		/// @param pos 読み込み開始位置（バイト）
		/// @param size 読み込むサイズ（バイト）
		/// @return 指定したサイズのデータをすべて読み込めた場合 true, それ以外の場合は false
		/// @remark size が 0 以下の場合は true を返し、読み込み位置は変更されません。
		/// @remark 読み込みに成功すると、読み込み位置は pos + size に移動します。
		/// @remark 読み込みに失敗した場合、読み込み位置は変更されません。
		/// @remark 読み込みに失敗した場合、dst が指すメモリの内容は未規定です。
		[[nodiscard]]
		bool readExact(void* dst, int64 pos, int64 size);

		/// @brief ファイルから値を読み込みます。
		/// @param dst 読み込み先の値
		/// @return sizeof(dst) バイトをすべて読み込めた場合 true, それ以外の場合は false
		/// @remark 読み込みに成功すると、読み込み位置は sizeof(dst) バイト進みます。
		/// @remark 読み込みに失敗した場合、読み込み位置は変更されません。
		/// @remark 読み込みに失敗した場合、dst の内容は未規定です。
		[[nodiscard]]
		bool readExact(Concept::TriviallyCopyable auto& dst);

		/// @brief ファイルから値を読み込みます。
		/// @tparam Type 読み込む値の型
		/// @return sizeof(Type) バイトをすべて読み込めた場合は読み込んだ値。それ以外の場合は none
		/// @remark 読み込みに成功すると、読み込み位置は sizeof(Type) バイト進みます。
		/// @remark 読み込みに失敗した場合、読み込み位置は変更されません。
		template <Concept::TriviallyCopyable Type>
		[[nodiscard]]
		Optional<Type> readExact();

		////////////////////////////////////////////////////////////////
		//
		//	readToEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの終端までのデータを読み込み、Blob として返します。
		/// @return 読み込んだデータ
		[[nodiscard]]
		Blob readToEnd();

		////////////////////////////////////////////////////////////////
		//
		//	readBytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の読み込み位置から指定したサイズのデータを読み込み、Blob として返します。
		/// @param size 読み込むサイズ（バイト）
		/// @return 読み込んだデータ
		/// @remark 読み込み位置は戻り値の Blob のサイズ分だけ進みます。
		[[nodiscard]]
		Blob readBytes(int64 size);

		/// @brief ファイルの指定した位置から指定したサイズのデータを読み込み、Blob として返します。
		/// @param pos 先頭から数えた読み込み開始位置（バイト）
		/// @param size 読み込むサイズ（バイト）
		/// @return 読み込んだデータ
		/// @remark 読み込み位置は pos + 戻り値の Blob のサイズに移動します。
		/// @remark 読み込み位置を変更せずに読み込むには lookaheadBytes(pos, size) を使用してください。
		/// @remark size が 0 以下の場合は空の Blob を返し、読み込み位置は変更されません。この場合、pos は検証されません。
		/// @remark size が 0 より大きく、pos が 0 以上ファイルサイズ以下の範囲にない場合は Error を投げます。
		[[nodiscard]]
		Blob readBytes(int64 pos, int64 size);

		////////////////////////////////////////////////////////////////
		//
		//	lookahead
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルからデータを読み込みます。読み込み位置は変更されません。
		/// @param dst 読み込み先
		/// @param size 読み込むサイズ（バイト）
		/// @return 実際に読み込んだサイズ（バイト）
		int64 lookahead(void* dst, int64 size) const override;

		/// @brief ファイルの指定した位置から指定したサイズのデータを読み込みます。読み込み位置は変更されません。
		/// @param dst 読み込み先
		/// @param pos 先頭から数えた読み込み開始位置（バイト）
		/// @param size 読み込むサイズ（バイト）
		/// @return 実際に読み込んだサイズ（バイト）
		int64 lookahead(void* dst, int64 pos, int64 size) const override;

		/// @brief ファイルからデータを読み込みます。読み込み位置は変更されません。
		/// @tparam TriviallyCopyable 読み込む値の型
		/// @param dst 読み込み先
		/// @return 読み込みに成功したら true, それ以外の場合は false
		[[nodiscard]]
		bool lookahead(Concept::TriviallyCopyable auto& dst) const;

		/// @brief ファイルからデータを読み込みます。読み込み位置は変更されません。
		/// @tparam Type 読み込む値の型
		/// @return 読み込みに成功した場合は読み込んだ値。それ以外の場合は none
		template <Concept::TriviallyCopyable Type>
		[[nodiscard]]
		Optional<Type> lookahead() const;

		////////////////////////////////////////////////////////////////
		//
		//	lookaheadExact
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルからデータを読み込みます。読み込み位置は変更されません。
		/// @param dst 読み込み先
		/// @param size 読み込むサイズ（バイト）
		/// @return 読み込みに成功したら true, それ以外の場合は false
		/// @remark size が 0 以下の場合は true を返します。この場合、dst は検証されません。
		/// @remark size が 0 より大きく、dst が nullptr の場合は Error を投げます。
		/// @remark 読み込み位置は変更されません。
		[[nodiscard]]
		bool lookaheadExact(void* dst, int64 size) const;

		/// @brief ファイルの指定した位置から指定したサイズのデータを過不足なく読み込みます。読み込み位置は変更されません。
		/// @param dst 読み込み先の先頭ポインタ
		/// @param pos 読み込み開始位置（バイト）
		/// @param size 読み込むサイズ（バイト）
		/// @return 指定したサイズのデータをすべて読み込めた場合 true, それ以外の場合は false
		/// @remark size が 0 以下の場合は true を返します。この場合、dst および pos は検証されません。
		/// @remark size が 0 より大きく、dst が nullptr の場合は Error を投げます。
		/// @remark size が 0 より大きく、pos が 0 以上ファイルサイズ以下の範囲にない場合は Error を投げます。
		/// @remark 読み込み位置は変更されません。
		[[nodiscard]]
		bool lookaheadExact(void* dst, int64 pos, int64 size) const;

		////////////////////////////////////////////////////////////////
		//
		//	lookaheadBytes
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 現在の読み込み位置から指定したサイズのデータを読み込み、Blob として返します。読み込み位置は変更されません。
		/// @param size 読み込むサイズ（バイト）
		/// @return 読み込んだデータ
		[[nodiscard]]
		Blob lookaheadBytes(int64 size) const;
		
		/// @brief ファイルの指定した位置から指定したサイズのデータを読み込み、Blob として返します。読み込み位置は変更されません。
		/// @param pos 先頭から数えた読み込み開始位置（バイト）
		/// @param size 読み込むサイズ（バイト）
		/// @return 読み込んだデータ
		/// @remark size が 0 以下の場合は空の Blob を返します。この場合、pos は検証されません。
		/// @remark size が 0 より大きく、pos が 0 以上ファイルサイズ以下の範囲にない場合は Error を投げます。
		[[nodiscard]]
		Blob lookaheadBytes(int64 pos, int64 size) const;

		////////////////////////////////////////////////////////////////
		//
		//	path
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開いているファイルのパスを返します。
		/// @return 開いている通常ファイルのフルパス、またはリソースパス。開いていない場合は空文字列
		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		class BinaryFileReaderDetail;

		std::unique_ptr<BinaryFileReaderDetail> pImpl;
	};
}

# include "detail/BinaryFileReader.ipp"
