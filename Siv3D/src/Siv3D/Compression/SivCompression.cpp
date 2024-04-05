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

# include <Siv3D/Compression.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <ThirdParty/zstd/zstd.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static ZSTD_CCtx* CreateContext(const int32 compressionLevel, const int32 nThreads)
		{
			ZSTD_CCtx* const context = ZSTD_createCCtx();

			if (not context)
			{
				return nullptr;
			}

			if (ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_compressionLevel, compressionLevel)))
			{
				ZSTD_freeCCtx(context);
				return nullptr;
			}

		# ifdef ZSTD_MULTITHREAD

			if (nThreads)
			{
				if (ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_nbWorkers, nThreads)))
				{
					ZSTD_freeCCtx(context);
					return nullptr;
				}
			}

		# endif

			return context;
		}
	}

	namespace Compression
	{
		////////////////////////////////////////////////////////////////
		//
		//	Compress
		//
		////////////////////////////////////////////////////////////////

		Blob Compress(const void* data, const size_t size, const int32 compressionLevel)
		{
			Blob blob;

			if (not Compress(data, size, blob, compressionLevel))
			{
				return{};
			}

			return blob;
		}

		bool Compress(const void* data, const size_t size, Blob& dst, const int32 compressionLevel)
		{
			// バッファを圧縮後の最大サイズにリサイズする
			{
				const size_t bufferSize = ZSTD_compressBound(size);
				dst.resize(bufferSize);
			}

			// 圧縮し、結果をバッファに書き込む
			const size_t result = ZSTD_compress(dst.data(), dst.size(), data, size, compressionLevel);

			// エラーが発生した場合、バッファをクリアして false を返す
			if (ZSTD_isError(result))
			{
				dst.clear();
				return false;
			}

			// 結果のサイズにリサイズする
			dst.resize(result);

			return true;
		}

		Blob Compress(const Blob& blob, const int32 compressionLevel)
		{
			return Compress(blob.data(), blob.size(), compressionLevel);
		}

		bool Compress(const Blob& blob, Blob& dst, const int32 compressionLevel)
		{
			return Compress(blob.data(), blob.size(), dst, compressionLevel);
		}

		////////////////////////////////////////////////////////////////
		//
		//	CompressFile
		//
		////////////////////////////////////////////////////////////////

		Blob CompressFile(const FilePathView inputPath, const int32 compressionLevel, const int32 nThreads)
		{
			Blob blob;

			if (not CompressFile(inputPath, blob, compressionLevel, nThreads))
			{
				return{};
			}

			return blob;
		}

		bool CompressFile(const FilePathView inputPath, Blob& dst, const int32 compressionLevel, const int32 nThreads)
		{
			dst.clear();

			// ファイルのオープン
			BinaryReader reader{ inputPath };
			{
				if (not reader)
				{
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto inputBuffer = std::make_unique_for_overwrite<Byte[]>(InputBufferSize);
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_CCtx* const context = CreateContext(compressionLevel, nThreads);
			{
				if (not context)
				{
					return false;
				}
			}

			for (;;) // チャンクごとに圧縮
			{
				const size_t toRead = InputBufferSize;
				const size_t read = static_cast<size_t>(reader.read(inputBuffer.get(), toRead));
				const bool lastChunk = (read < toRead);
				const ZSTD_EndDirective mode = (lastChunk ? ZSTD_e_end : ZSTD_e_continue);
				
				ZSTD_inBuffer input = { inputBuffer.get(), read, 0 };
				bool finished = false;

				do
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };

					const size_t remaining = ZSTD_compressStream2(context, &output, &input, mode);

					if (ZSTD_isError(remaining))
					{
						ZSTD_freeCCtx(context);
						dst.clear();
						return false;
					}

					dst.append(outputBuffer.get(), output.pos);

					finished = (lastChunk ? (remaining == 0) : (input.pos == input.size));
				
				} while (not finished);

				if (lastChunk)
				{
					break;
				}
			}

			ZSTD_freeCCtx(context);

			return true;
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	CompressToFile
		//
		////////////////////////////////////////////////////////////////

		bool CompressToFile(const void* data, const size_t size, const FilePathView outputPath, const int32 compressionLevel, const int32 nThreads)
		{
			// ファイルのオープン
			BinaryWriter writer{ outputPath };
			{
				if (not writer)
				{
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_CCtx* const context = CreateContext(compressionLevel, nThreads);
			{
				if (not context)
				{
					return false;
				}
			}

			size_t readPos = 0;

			for (;;) // チャンクごとに圧縮
			{
				const size_t toRead = InputBufferSize;
				const size_t read = Min(toRead, (size - readPos));
				const bool lastChunk = (read < toRead);
				const ZSTD_EndDirective mode = (lastChunk ? ZSTD_e_end : ZSTD_e_continue);

				ZSTD_inBuffer input = { (static_cast<const Byte*>(data) + readPos), read, 0 };

				readPos += read;

				bool finished = false;

				do
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };

					const size_t remaining = ZSTD_compressStream2(context, &output, &input, mode);

					if (ZSTD_isError(remaining))
					{
						ZSTD_freeCCtx(context);
						writer.clear();
						return false;
					}

					writer.write(outputBuffer.get(), output.pos);

					finished = (lastChunk ? (remaining == 0) : (input.pos == input.size));

				} while (not finished);

				if (lastChunk)
				{
					break;
				}
			}

			ZSTD_freeCCtx(context);

			return true;
		}

		bool CompressToFile(const Blob& blob, const FilePathView outputPath, const int32 compressionLevel, const int32 nThreads)
		{
			return CompressToFile(blob.data(), blob.size(), outputPath, compressionLevel, nThreads);
		}

		////////////////////////////////////////////////////////////////
		//
		//	CompressFileToFile
		//
		////////////////////////////////////////////////////////////////

		bool CompressFileToFile(const FilePathView inputPath, const FilePathView outputPath, const int32 compressionLevel, const int32 nThreads)
		{
			// 入力ファイルのオープン
			BinaryReader reader{ inputPath };
			{
				if (not reader)
				{
					return false;
				}
			}

			// 出力ファイルのオープン
			BinaryWriter writer{ outputPath };
			{
				if (not writer)
				{
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto inputBuffer = std::make_unique_for_overwrite<Byte[]>(InputBufferSize);
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_CCtx* const context = CreateContext(compressionLevel, nThreads);
			{
				if (not context)
				{
					return false;
				}
			}

			for (;;) // チャンクごとに圧縮
			{
				const size_t toRead = InputBufferSize;
				const size_t read = static_cast<size_t>(reader.read(inputBuffer.get(), toRead));
				const bool lastChunk = (read < toRead);
				const ZSTD_EndDirective mode = (lastChunk ? ZSTD_e_end : ZSTD_e_continue);

				ZSTD_inBuffer input = { inputBuffer.get(), read, 0 };
				bool finished = false;

				do
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };

					const size_t remaining = ZSTD_compressStream2(context, &output, &input, mode);

					if (ZSTD_isError(remaining))
					{
						ZSTD_freeCCtx(context);
						writer.clear();
						return false;
					}

					writer.write(outputBuffer.get(), output.pos);

					finished = (lastChunk ? (remaining == 0) : (input.pos == input.size));

				} while (not finished);

				if (lastChunk)
				{
					break;
				}
			}

			ZSTD_freeCCtx(context);

			return true;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Decompress
		//
		////////////////////////////////////////////////////////////////

		Blob Decompress(const void* data, const size_t size)
		{
			Blob blob;

			if (not Decompress(data, size, blob))
			{
				return{};
			}

			return blob;
		}

		bool Decompress(const void* data, const size_t size, Blob& dst)
		{
			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_DCtx* const context = ZSTD_createDCtx();
			{
				if (not context)
				{
					return false;
				}
			}

			size_t readPos = 0;

			while (size_t read = Min(InputBufferSize, (size - readPos)))
			{
				ZSTD_inBuffer input = { (static_cast<const Byte*>(data) + readPos), read, 0 };

				readPos += read;

				while (input.pos < input.size)
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0};
					size_t const result = ZSTD_decompressStream(context, &output, &input);
					
					if (ZSTD_isError(result))
					{
						ZSTD_freeDCtx(context);
						dst.clear();
						return false;
					}

					dst.append(outputBuffer.get(), output.pos);
				}
			}

			ZSTD_freeDCtx(context);

			return true;
		}

		Blob Decompress(const Blob& blob)
		{
			return Decompress(blob.data(), blob.size());
		}

		bool Decompress(const Blob& blob, Blob& dst)
		{
			return Decompress(blob.data(), blob.size(), dst);
		}

		////////////////////////////////////////////////////////////////
		//
		//	DecompressFile
		//
		////////////////////////////////////////////////////////////////

		Blob DecompressFile(const FilePathView path)
		{
			Blob blob;

			if (not DecompressFile(path, blob))
			{
				return{};
			}

			return blob;
		}

		bool DecompressFile(const FilePathView path, Blob& dst)
		{
			// ファイルのオープン
			BinaryReader reader{ path };
			{
				if (not reader)
				{
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto inputBuffer = std::make_unique_for_overwrite<Byte[]>(InputBufferSize);
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_DCtx* const context = ZSTD_createDCtx();
			{
				if (not context)
				{
					return false;
				}
			}

			while (size_t read = static_cast<size_t>(reader.read(inputBuffer.get(), InputBufferSize)))
			{
				ZSTD_inBuffer input = { inputBuffer.get(), read, 0 };

				while (input.pos < input.size)
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };
					size_t const result = ZSTD_decompressStream(context, &output, &input);

					if (ZSTD_isError(result))
					{
						ZSTD_freeDCtx(context);
						dst.clear();
						return false;
					}

					dst.append(outputBuffer.get(), output.pos);
				}
			}

			ZSTD_freeDCtx(context);

			return true;
		}

		////////////////////////////////////////////////////////////////
		//
		//	DecompressToFile
		//
		////////////////////////////////////////////////////////////////

		bool DecompressToFile(const void* data, const size_t size, const FilePathView outputPath)
		{
			// ファイルのオープン
			BinaryWriter writer{ outputPath };
			{
				if (not writer)
				{
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_DCtx* const context = ZSTD_createDCtx();
			{
				if (not context)
				{
					return false;
				}
			}

			size_t readPos = 0;

			while (size_t read = Min(InputBufferSize, (size - readPos)))
			{
				ZSTD_inBuffer input = { (static_cast<const Byte*>(data) + readPos), read, 0 };

				readPos += read;

				while (input.pos < input.size)
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };
					size_t const result = ZSTD_decompressStream(context, &output, &input);

					if (ZSTD_isError(result))
					{
						ZSTD_freeDCtx(context);
						writer.clear();
						return false;
					}

					writer.write(outputBuffer.get(), output.pos);
				}
			}

			ZSTD_freeDCtx(context);

			return true;
		}

		bool DecompressToFile(const Blob& blob, const FilePathView outputPath)
		{
			return DecompressToFile(blob.data(), blob.size(), outputPath);
		}

		////////////////////////////////////////////////////////////////
		//
		//	DecompressFileToFile
		//
		////////////////////////////////////////////////////////////////

		bool DecompressFileToFile(const FilePathView inputPath, const FilePathView outputPath)
		{
			// 入力ファイルのオープン
			BinaryReader reader{ inputPath };
			{
				if (not reader)
				{
					return false;
				}
			}

			// 出力ファイルのオープン
			BinaryWriter writer{ outputPath };
			{
				if (not writer)
				{
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto inputBuffer = std::make_unique_for_overwrite<Byte[]>(InputBufferSize);
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			// コンテキストの作成
			ZSTD_DCtx* const context = ZSTD_createDCtx();
			{
				if (not context)
				{
					return false;
				}
			}

			while (size_t read = static_cast<size_t>(reader.read(inputBuffer.get(), InputBufferSize)))
			{
				ZSTD_inBuffer input = { inputBuffer.get(), read, 0 };

				while (input.pos < input.size)
				{
					ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };
					size_t const result = ZSTD_decompressStream(context, &output, &input);

					if (ZSTD_isError(result))
					{
						ZSTD_freeDCtx(context);
						writer.clear();
						return false;
					}

					writer.write(outputBuffer.get(), output.pos);
				}
			}

			ZSTD_freeDCtx(context);

			return true;
		}
	}
}
