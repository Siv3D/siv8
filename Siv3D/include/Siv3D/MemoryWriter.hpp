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
# include "IWriter.hpp"
# include "Blob.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MemoryWriter
	//
	////////////////////////////////////////////////////////////////

	/// @brief メモリへのバイナリデータ書き出し
	class MemoryWriter : public IWriter
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		MemoryWriter() = default;

		[[nodiscard]]
		explicit MemoryWriter(Arg::reserve_<size_t> reserveSizeBytes);

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isOpen() const noexcept override;

		[[nodiscard]]
		int64 size() const override;

		[[nodiscard]]
		int64 getPos() const override;

		bool setPos(int64 pos) override;

		int64 seekToEnd();

		int64 write(const void* src, int64 sizeBytes) override;

		bool write(const Concept::TriviallyCopyable auto& src);

		[[nodiscard]]
		Blob retrieve();

		const Blob& getBlob() const noexcept;

		void reserve(size_t sizeBytes);

		void clear();

		void release();

	private:

		Blob m_blob;

		int64 m_writePos = 0;
	};
}

# include "detail/MemoryWriter.ipp"
