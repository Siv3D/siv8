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
# include <utility>
# include "Common.hpp"
# include "IReader.hpp"
# include "Blob.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MemoryReader
	//
	////////////////////////////////////////////////////////////////

	/// @brief メモリ上のバイナリデータを読み込む Reader クラス | Reader class to read binary data in memory
	class MemoryReader : public IReader
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の Reader を作成します。 | Creates an empty reader.
		[[nodiscard]]
		MemoryReader() = default;

		/// @brief データと読み込み位置をコピーします。 | Copies the data and read position.
		/// @param other コピー元 | The reader to copy
		[[nodiscard]]
		MemoryReader(const MemoryReader& other) = default;

		/// @brief データと読み込み位置をムーブします。 | Moves the data and read position.
		/// @param other ムーブ元。ムーブ後は空になり、読み込み位置は 0 になります。 | The source, which becomes empty with read position 0.
		[[nodiscard]]
		constexpr MemoryReader(MemoryReader&& other) noexcept;

		/// @brief メモリ上のデータをコピーして所有します。 | Copies and owns the data in memory.
		/// @param data コピー元。size_bytes が 0 の場合は nullptr を指定できます。 | The source; may be nullptr when size_bytes is 0.
		/// @param size_bytes コピーするバイト数。INT64_MAX 以下である必要があります。 | The byte count to copy; must not exceed INT64_MAX.
		/// @pre size_bytes が正の場合、data はそのバイト数の読み取り可能な領域を指す必要があります。 | When size_bytes is positive, data must point to that many readable bytes.
		[[nodiscard]]
		MemoryReader(const void* data, size_t size_bytes);

		/// @brief Blob のデータをコピーして所有します。 | Copies and owns the Blob's data.
		/// @param blob コピー元 | The Blob to copy
		/// @throws std::bad_alloc データ用のメモリを確保できない場合 | If memory allocation for the data fails
		[[nodiscard]]
		explicit constexpr MemoryReader(const Blob& blob);

		/// @brief Blob のデータの所有権を受け取ります。 | Takes ownership of the Blob's data.
		/// @param blob ムーブ元 | The Blob to move
		[[nodiscard]]
		explicit constexpr MemoryReader(Blob&& blob) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief データと読み込み位置をコピーします。 | Copies the data and read position.
		/// @param other コピー元 | The reader to copy
		/// @return *this
		MemoryReader& operator =(const MemoryReader& other) = default;

		/// @brief データと読み込み位置をムーブします。 | Moves the data and read position.
		/// @param other ムーブ元。自己代入以外では、ムーブ後は空になり、読み込み位置は 0 になります。 | The source, which becomes empty with read position 0 unless assigning to itself.
		/// @remark 自己ムーブ代入では状態を変更しません。 | Self move assignment leaves the state unchanged.
		/// @return *this
		constexpr MemoryReader& operator =(MemoryReader&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	supportsLookahead
		//
		////////////////////////////////////////////////////////////////

		/// @brief Reader が読み込み位置を前進させないデータ読み込みをサポートしているかを返します。 | Returns whether the Reader supports data reading without advancing the read position.
		/// @return true
		[[nodiscard]]
		constexpr bool supportsLookahead() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief Reader のデータにアクセス可能かを返します。 | Returns whether the Reader can access the data.
		/// @return 所有するデータが空でない場合 true, それ以外の場合は false | true if the owned data is nonempty, otherwise false
		[[nodiscard]]
		constexpr bool isOpen() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief Reader のデータにアクセス可能かを返します。 | Returns whether the Reader can access the data.
		/// @return 所有するデータが空でない場合 true, それ以外の場合は false | true if the owned data is nonempty, otherwise false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief データのサイズを返します。 | Returns the size of the data.
		/// @return データのサイズ（バイト） | The size of the data (bytes)
		[[nodiscard]]
		constexpr int64 size() const override;

		////////////////////////////////////////////////////////////////
		//
		//	getPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief データの現在の読み込み位置を返します。 | Returns the current read position of the data.
		/// @return 現在の読み込み位置（バイト） | The current read position (bytes)
		[[nodiscard]]
		constexpr int64 getPos() const override;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief データの読み込み位置を変更します。 | Changes the read position of the data.
		/// @param pos 新しい読み込み位置（バイト）。[0, size()] の範囲にクランプされます。 | The new read position in bytes, clamped to [0, size()].
		/// @return 新しい読み込み位置（バイト） | The new read position (bytes)
		constexpr int64 setPos(int64 pos) override;

		////////////////////////////////////////////////////////////////
		//
		//	skip
		//
		////////////////////////////////////////////////////////////////

		/// @brief 読み込み位置を相対的に移動します。 | Moves the read position by a relative offset.
		/// @param offset 移動量（バイト）。負の場合は後退します。 | The offset in bytes; negative values move backward.
		/// @return [0, size()] の範囲にクランプされた新しい読み込み位置（バイト） | The new read position in bytes, clamped to [0, size()].
		constexpr int64 skip(int64 offset) override;

		////////////////////////////////////////////////////////////////
		//
		//	read
		//
		////////////////////////////////////////////////////////////////

		/// @brief データを読み込み、その分読み込み位置を前進させます。 | Reads the data and advances the read position.
		/// @param dst 読み込んだデータの格納先。コピー元の領域と重ならない、読み込みバイト数分の書き込み可能領域が必要です。 | Writable storage for the bytes read, which must not overlap the source range.
		/// @param size 読み込むサイズ（バイト）。0 以下の場合は他の引数を検査せず、何もしません。 | The byte count; if nonpositive, does nothing without checking the other arguments.
		/// @return 実際に読み込んだサイズ（バイト）。size が 0 以下、または読み込み開始位置が終端以降の場合は 0。 | The actual byte count; 0 if size is nonpositive or the start position is at or beyond the end.
		/// @throws Error size が正で dst が nullptr の場合。読み込み位置は変わりません。 | If size is positive and dst is nullptr; the read position is unchanged.
		int64 read(void* dst, int64 size) override;

		/// @brief データを読み込み、その分読み込み位置を前進させます。 | Reads the data and advances the read position.
		/// @param dst 読み込んだデータの格納先。コピー元の領域と重ならない、読み込みバイト数分の書き込み可能領域が必要です。 | Writable storage for the bytes read, which must not overlap the source range.
		/// @param pos 先頭から数えた読み込み開始位置（バイト） | The read start position from the beginning (bytes)
		/// @param size 読み込むサイズ（バイト）。0 以下の場合は他の引数を検査せず、何もしません。 | The byte count; if nonpositive, does nothing without checking the other arguments.
		/// @return 実際に読み込んだサイズ（バイト）。size が 0 以下、または読み込み開始位置が終端以降の場合は 0。 | The actual byte count; 0 if size is nonpositive or the start position is at or beyond the end.
		/// @remark 1 バイト以上読み込んだ場合、読み込み位置を pos + 読み込んだサイズに変更します。それ以外は変更しません。 | If any bytes are read, sets the read position to pos plus the bytes read; otherwise leaves it unchanged.
		/// @throws Error size が正で、dst が nullptr または pos が負の場合。読み込み位置は変わりません。 | If size is positive and either dst is nullptr or pos is negative; the read position is unchanged.
		int64 read(void* dst, int64 pos, int64 size) override;

		/// @brief データを読み込み、その分読み込み位置を前進させます。 | Reads the data and advances the read position.
		/// @param dst 読み込んだデータの格納先 | The destination to store the read data
		/// @return sizeof(dst) バイト読み込んだ場合 true, それ以外の場合は false | true if sizeof(dst) bytes were read, otherwise false
		/// @remark データが不足する場合も、読み込めたバイト数だけ dst の先頭を更新し、読み込み位置を進めます。 | A short read still updates the prefix of dst and advances the read position by the bytes read.
		bool read(Concept::TriviallyCopyable auto& dst);

		////////////////////////////////////////////////////////////////
		//
		//	lookahead
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の読み込み位置から、読み込み位置を前進させずにデータを読み込みます。 | Reads the data from the current read position without advancing the read position.
		/// @param dst 読み込んだデータの格納先。コピー元の領域と重ならない、読み込みバイト数分の書き込み可能領域が必要です。 | Writable storage for the bytes read, which must not overlap the source range.
		/// @param size 読み込むサイズ（バイト）。0 以下の場合は他の引数を検査せず、何もしません。 | The byte count; if nonpositive, does nothing without checking the other arguments.
		/// @return 実際に読み込んだサイズ（バイト）。size が 0 以下、または読み込み開始位置が終端以降の場合は 0。 | The actual byte count; 0 if size is nonpositive or the start position is at or beyond the end.
		/// @throws Error size が正で dst が nullptr の場合 | If size is positive and dst is nullptr
		int64 lookahead(void* dst, int64 size) const override;

		/// @brief 現在の読み込み位置は変更せずに、データを読み込みます。 | Reads the data without changing the current read position.
		/// @param dst 読み込んだデータの格納先。コピー元の領域と重ならない、読み込みバイト数分の書き込み可能領域が必要です。 | Writable storage for the bytes read, which must not overlap the source range.
		/// @param pos 先頭から数えた読み込み開始位置（バイト） | The read start position from the beginning (bytes)
		/// @param size 読み込むサイズ（バイト）。0 以下の場合は他の引数を検査せず、何もしません。 | The byte count; if nonpositive, does nothing without checking the other arguments.
		/// @return 実際に読み込んだサイズ（バイト）。size が 0 以下、または読み込み開始位置が終端以降の場合は 0。 | The actual byte count; 0 if size is nonpositive or the start position is at or beyond the end.
		/// @throws Error size が正で、dst が nullptr または pos が負の場合 | If size is positive and either dst is nullptr or pos is negative
		int64 lookahead(void* dst, int64 pos, int64 size) const override;

		/// @brief 現在の読み込み位置から、読み込み位置を前進させずにデータを読み込みます。 | Reads the data from the current read position without advancing the read position.
		/// @param dst 読み込んだデータの格納先 | The destination to store the read data
		/// @return sizeof(dst) バイト読み込んだ場合 true, それ以外の場合は false | true if sizeof(dst) bytes were read, otherwise false
		/// @remark データが不足する場合も、読み込めたバイト数だけ dst の先頭を更新します。 | A short read still updates the prefix of dst with the bytes read.
		bool lookahead(Concept::TriviallyCopyable auto& dst) const;

	private:

		Blob m_blob;

		int64 m_pos = 0;
	};
}

# include "detail/MemoryReader.ipp"
