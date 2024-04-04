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

		Blob CompressFile(const FilePathView path, const int32 compressionLevel)
		{
			Blob blob;

			if (not CompressFile(path, blob, compressionLevel))
			{
				return{};
			}

			return blob;
		}

		bool CompressFile(const FilePathView path, Blob& dst, const int32 compressionLevel)
		{
			dst.clear();

			// ファイルのオープン
			BinaryReader reader{ path };
			{
				if (not reader)
				{
					return false;
				}
			}

			// ストリームの作成
			ZSTD_CStream* const cStream = ZSTD_createCStream();
			{
				if (not cStream)
				{
					return false;
				}

				if (ZSTD_isError(ZSTD_initCStream(cStream, compressionLevel)))
				{
					ZSTD_freeCStream(cStream);
					return false;
				}
			}

			// バッファの作成
			const size_t InputBufferSize = ZSTD_CStreamInSize();
			const size_t OutputBufferSize = ZSTD_CStreamOutSize();
			const auto inputBuffer = std::make_unique_for_overwrite<Byte[]>(InputBufferSize);
			const auto outputBuffer = std::make_unique_for_overwrite<Byte[]>(OutputBufferSize);

			{
				size_t toRead = InputBufferSize;

				while (const size_t read = static_cast<size_t>(reader.read(inputBuffer.get(), toRead)))
				{
					ZSTD_inBuffer input = { inputBuffer.get(), read, 0 };

					while (input.pos < input.size)
					{
						ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };

						toRead = ZSTD_compressStream(cStream, &output, &input);

						if (ZSTD_isError(toRead))
						{
							ZSTD_freeCStream(cStream);
							dst.clear();
							return false;
						}

						if (InputBufferSize < toRead)
						{
							toRead = InputBufferSize;
						}

						dst.append(outputBuffer.get(), output.pos);
					}
				}

				ZSTD_outBuffer output = { outputBuffer.get(), OutputBufferSize, 0 };

				const size_t remainingToFlush = ZSTD_endStream(cStream, &output);

				ZSTD_freeCStream(cStream);

				if (remainingToFlush)
				{
					dst.clear();
					return false;
				}

				dst.append(outputBuffer.get(), output.pos);
			}

			return true;
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	CompressToFile
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	CompressFileToFile
		//
		////////////////////////////////////////////////////////////////

	}
}
