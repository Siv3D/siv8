//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <memory>
# include "Common.hpp"
# include "Concepts.hpp"

namespace s3d
{
	/// @brief Writer インタフェース | Writer interface
	class IWriter
	{
	public:

		/// @brief デストラクタ | Destructor
		virtual ~IWriter() = default;

		/// @brief Writer にデータを書き込み可能であるかを返します。 | Returns whether the Writer is open.
		/// @return 書き込み可能である場合 true, それ以外の場合は false | Returns true if the Writer is open, otherwise false
		[[nodiscard]]
		virtual bool isOpen() const noexcept = 0;

		/// @brief 書き込まれているデータのサイズを返します。 | Returns the size of the data written.
		/// @return データのサイズ（バイト） | The size of the data (bytes)
		[[nodiscard]]
		virtual int64 size() const = 0;

		/// @brief 現在の書き込み位置を返します。 | Returns the current write position.
		/// @return 現在の書き込み位置（バイト） | The current write position (bytes)
		[[nodiscard]]
		virtual int64 getPos() const = 0;

		/// @brief 書き込み位置を変更します。 | Changes the write position.
		/// @param pos 新しい書き込み位置（バイト） | The new write position (bytes)
		/// @return 書き込み位置の変更に成功した場合 true, それ以外の場合は false | Returns true if the write position was changed, otherwise false
		virtual bool setPos(int64 pos) = 0;

		/// @brief 現在の書き込み位置へデータを書き込みます。 | Writes data to the current write position.
		/// @param src 書き込むデータ | Data to write
		/// @param sizeBytes 書き込むサイズ（バイト） | The size to write (bytes)
		/// @return 実際に書き込んだサイズ（バイト） | The actual size written (bytes)
		virtual int64 write(const void* src, int64 sizeBytes) = 0;

		/// @brief 現在の書き込み位置へデータを書き込みます。 | Writes data to the current write position.
		/// @param src 書き込むデータ | Data to write
		/// @return 書き込みに成功した場合 true, それ以外の場合は false | Returns true if the write succeeded, otherwise false
		bool write(const Concept::TriviallyCopyable auto& src);
	};
}

# include "detail/IWriter.ipp"
