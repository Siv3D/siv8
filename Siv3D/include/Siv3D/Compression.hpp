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

		/// @brief バイナリデータを圧縮します。
		/// @param data 圧縮するデータの先頭ポインタ
		/// @param size 圧縮するデータのサイズ（バイト）
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @return 圧縮されたデータ
		[[nodiscard]]
		Blob Compress(const void* data, size_t size, int32 compressionLevel = DefaultLevel);

		/// @brief バイナリデータを圧縮します。
		/// @param data 圧縮するデータの先頭ポインタ
		/// @param size 圧縮するデータのサイズ（バイト）
		/// @param dst 圧縮されたデータの格納先
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @return 圧縮に成功した場合 true, それ以外の場合は false
		bool Compress(const void* data, size_t size, Blob& dst, int32 compressionLevel = DefaultLevel);

		/// @brief バイナリデータを圧縮します。
		/// @param blob 圧縮するデータ
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @return 圧縮されたデータ
		[[nodiscard]]
		Blob Compress(const Blob& blob, int32 compressionLevel = DefaultLevel);

		/// @brief バイナリデータを圧縮します。
		/// @param blob 圧縮するデータ
		/// @param dst 圧縮されたデータの格納先
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @return 圧縮に成功した場合 true, それ以外の場合は false
		bool Compress(const Blob& blob, Blob& dst, int32 compressionLevel = DefaultLevel);

		////////////////////////////////////////////////////////////////
		//
		//	CompressFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを圧縮します。
		/// @param inputPath 圧縮するファイルのパス
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @param nThreads ワーカースレッドの数。0 はシングルスレッド
		/// @return 圧縮されたデータ
		[[nodiscard]]
		Blob CompressFile(FilePathView inputPath, int32 compressionLevel = DefaultLevel, int32 nThreads = 0);

		/// @brief ファイルを圧縮します。
		/// @param inputPath 圧縮するファイルのパス
		/// @param dst 圧縮されたデータの格納先
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @param nThreads ワーカースレッドの数。0 はシングルスレッド
		/// @return 圧縮に成功した場合 true, それ以外の場合は false
		bool CompressFile(FilePathView inputPath, Blob& dst, int32 compressionLevel = DefaultLevel, int32 nThreads = 0);

		////////////////////////////////////////////////////////////////
		//
		//	CompressToFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータをファイルに圧縮して保存します。
		/// @param data 圧縮するデータの先頭ポインタ
		/// @param size 圧縮するデータのサイズ（バイト）
		/// @param outputPath 保存先のファイルパス
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @param nThreads ワーカースレッドの数。0 はシングルスレッド
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool CompressToFile(const void* data, size_t size, FilePathView outputPath, int32 compressionLevel = DefaultLevel, int32 nThreads = 0);

		/// @brief バイナリデータをファイルに圧縮して保存します。
		/// @param blob 圧縮するデータ
		/// @param outputPath 保存先のファイルパス
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @param nThreads ワーカースレッドの数。0 はシングルスレッド
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool CompressToFile(const Blob& blob, FilePathView outputPath, int32 compressionLevel = DefaultLevel, int32 nThreads = 0);

		////////////////////////////////////////////////////////////////
		//
		//	CompressFileToFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを圧縮して別のファイルに保存します。
		/// @param inputPath 圧縮するファイルのパス
		/// @param outputPath 保存先のファイルパス
		/// @param compressionLevel 圧縮レベル（1 ～ 22）
		/// @param nThreads ワーカースレッドの数。0 はシングルスレッド
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool CompressFileToFile(FilePathView inputPath, FilePathView outputPath, int32 compressionLevel = DefaultLevel, int32 nThreads = 0);

		////////////////////////////////////////////////////////////////
		//
		//	Decompress
		//
		////////////////////////////////////////////////////////////////

		/// @brief 圧縮されたデータを展開します。
		/// @param data 圧縮されたデータの先頭ポインタ
		/// @param size 圧縮されたデータのサイズ（バイト）
		/// @return 展開されたデータ
		[[nodiscard]]
		Blob Decompress(const void* data, size_t size);

		/// @brief 圧縮されたデータを展開します。
		/// @param data 圧縮されたデータの先頭ポインタ
		/// @param size 圧縮されたデータのサイズ（バイト）
		/// @param dst 展開されたデータの格納先
		/// @return 展開に成功した場合 true, それ以外の場合は false
		bool Decompress(const void* data, size_t size, Blob& dst);

		/// @brief 圧縮されたデータを展開します。
		/// @param blob 圧縮されたデータ
		/// @return 展開されたデータ
		[[nodiscard]]
		Blob Decompress(const Blob& blob);

		/// @brief 圧縮されたデータを展開します。
		/// @param blob 圧縮されたデータ
		/// @param dst 展開されたデータの格納先
		/// @return 展開に成功した場合 true, それ以外の場合は false
		bool Decompress(const Blob& blob, Blob& dst);

		////////////////////////////////////////////////////////////////
		//
		//	DecompressFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを展開します。
		/// @param path 展開するファイルのパス
		/// @return 展開されたデータ
		[[nodiscard]]
		Blob DecompressFile(FilePathView path);

		/// @brief ファイルを展開します。
		/// @param path 展開するファイルのパス
		/// @param dst 展開されたデータの格納先
		/// @return 展開に成功した場合 true, それ以外の場合は false
		bool DecompressFile(FilePathView path, Blob& dst);

		////////////////////////////////////////////////////////////////
		//
		//	DecompressToFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief 圧縮されたデータを展開してファイルに保存します。
		/// @param data 圧縮されたデータの先頭ポインタ
		/// @param size 圧縮されたデータのサイズ（バイト）
		/// @param outputPath 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool DecompressToFile(const void* data, size_t size, FilePathView outputPath);

		/// @brief 圧縮されたデータを展開してファイルに保存します。
		/// @param blob 圧縮されたデータ
		/// @param outputPath 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool DecompressToFile(const Blob& blob, FilePathView outputPath);

		////////////////////////////////////////////////////////////////
		//
		//	DecompressFileToFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを展開して別のファイルに保存します。
		/// @param inputPath 展開するファイルのパス
		/// @param outputPath 保存先のファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool DecompressFileToFile(FilePathView inputPath, FilePathView outputPath);
	}
}
