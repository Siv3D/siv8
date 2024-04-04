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
# include "StringView.hpp"
# include "Blob.hpp"

namespace s3d
{
	namespace Compression
	{
		/// @brief 最小の圧縮レベル（最速）
		inline constexpr int32 MinLevel = 1;

		/// @brief デフォルトの圧縮レベル
		inline constexpr int32 DefaultLevel = 11;

		/// @brief 最大の圧縮レベル（最高品質）
		inline constexpr int32 MaxLevel = 22;

		////////////////////////////////////////////////////////////////
		//
		//	Compress
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Blob Compress(const void* data, size_t size, int32 compressionLevel = DefaultLevel);

		bool Compress(const void* data, size_t size, Blob& dst, int32 compressionLevel = DefaultLevel);

		[[nodiscard]]
		Blob Compress(const Blob& blob, int32 compressionLevel = DefaultLevel);

		bool Compress(const Blob& blob, Blob& dst, int32 compressionLevel = DefaultLevel);

		////////////////////////////////////////////////////////////////
		//
		//	CompressFile
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Blob CompressFile(FilePathView path, int32 compressionLevel = DefaultLevel);

		bool CompressFile(FilePathView path, Blob& dst, int32 compressionLevel = DefaultLevel);

		////////////////////////////////////////////////////////////////
		//
		//	CompressToFile
		//
		////////////////////////////////////////////////////////////////

		bool CompressToFile(const void* data, size_t size, FilePathView outputPath, int32 compressionLevel = DefaultLevel);

		bool CompressToFile(const Blob& blob, FilePathView outputPath, int32 compressionLevel = DefaultLevel);

		////////////////////////////////////////////////////////////////
		//
		//	CompressFileToFile
		//
		////////////////////////////////////////////////////////////////

		bool CompressFileToFile(FilePathView inputPath, FilePathView outputPath, int32 compressionLevel = DefaultLevel);

		////////////////////////////////////////////////////////////////
		//
		//	Decompress
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Blob Decompress(const void* data, size_t size);

		bool Decompress(const void* data, size_t size, Blob& dst);

		[[nodiscard]]
		Blob Decompress(const Blob& blob);

		bool Decompress(const Blob& blob, Blob& dst);

		////////////////////////////////////////////////////////////////
		//
		//	DecompressFile
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Blob DecompressFile(FilePathView path);

		bool DecompressFile(FilePathView path, Blob& dst);

		////////////////////////////////////////////////////////////////
		//
		//	DecompressToFile
		//
		////////////////////////////////////////////////////////////////

		bool DecompressToFile(const void* data, size_t size, FilePathView outputPath);

		bool DecompressToFile(const Blob& blob, FilePathView outputPath);

		////////////////////////////////////////////////////////////////
		//
		//	DecompressFileToFile
		//
		////////////////////////////////////////////////////////////////

		bool DecompressFileToFile(FilePathView inputPath, FilePathView outputPath);
	}
}
