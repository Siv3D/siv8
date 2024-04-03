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
# include "IReader.hpp"
# include "Blob.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MemoryReader
	//
	////////////////////////////////////////////////////////////////

	class MemoryReader : public IReader
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		MemoryReader() = default;

		[[nodiscard]]
		constexpr MemoryReader(const void* data, size_t size_bytes) noexcept;

		[[nodiscard]]
		explicit constexpr MemoryReader(const Blob& blob) noexcept;

		[[nodiscard]]
		explicit constexpr MemoryReader(Blob&& blob) noexcept;

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
		/// @return データにアクセス可能な場合 true, それ以外の場合は false | Returns true if the data can be accessed, otherwise false
		[[nodiscard]]
		constexpr bool isOpen() const noexcept override;

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
		/// @param pos 新しい読み込み位置（バイト） | The new read position (bytes)
		/// @return 新しい読み込み位置（バイト） | The new read position (bytes)
		constexpr int64 setPos(int64 pos) override;

		////////////////////////////////////////////////////////////////
		//
		//	skip
		//
		////////////////////////////////////////////////////////////////

		/// @brief データを読み飛ばし、読み込み位置を前進させます。 | Skips the data and advances the read position.
		/// @param offset 読み飛ばすサイズ（バイト） | The size to skip (bytes)
		/// @return 新しい読み込み位置（バイト） | The new read position (bytes)
		constexpr int64 skip(int64 offset) override;


	private:

		Blob m_blob;

		int64 m_pos = 0;
	};
}

# include "detail/MemoryReader.ipp"
