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

# include "Siv3DTest.hpp"

[[nodiscard]]
static Blob MakeRandomBlob(size_t size)
{
	Array<Byte> bytes(size);

	for (auto& byte : bytes)
	{
		byte = Byte(Random('A', 'Z'));
	}

	return Blob{ bytes };
}

TEST_CASE("Compression")
{
	constexpr int32 nThreads = 8;

	const Blob original_empty = MakeRandomBlob(0); // 0 B
	const Blob original_tiny = MakeRandomBlob(7); // 7 B
	const Blob original_small = MakeRandomBlob(1024 + 111); // 1 KB
	const Blob original_medium = MakeRandomBlob(1024 * 320 + 333); // 320 KB
	const Blob original_large = MakeRandomBlob(1024 * 1024 * 2 + 555); // 2 MB

	original_empty.save(U"../../Test/output/compression/original_empty.bin");
	original_tiny.save(U"../../Test/output/compression/original_tiny.bin");
	original_small.save(U"../../Test/output/compression/original_small.bin");
	original_medium.save(U"../../Test/output/compression/original_medium.bin");
	original_large.save(U"../../Test/output/compression/original_large.bin");

	SUBCASE("Compress")
	{
		{
			const Blob compressed = Compression::Compress(original_empty);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_empty, decompressed);
		}

		{
			const Blob compressed = Compression::Compress(original_tiny);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_tiny, decompressed);
		}

		{
			const Blob compressed = Compression::Compress(original_small);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_small, decompressed);
		}

		{
			const Blob compressed = Compression::Compress(original_medium);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_medium, decompressed);
		}

		{
			const Blob compressed = Compression::Compress(original_large);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("CompressFile")
	{
		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_empty.bin");
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_empty, decompressed);
		}

		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_tiny.bin");
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_tiny, decompressed);
		}

		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_small.bin");
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_small, decompressed);
		}

		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_medium.bin");
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_medium, decompressed);
		}

		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_large.bin");
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("CompressToFile")
	{
		{
			CHECK(Compression::CompressToFile(original_empty, U"../../Test/output/compression/original_empty.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_empty.bin.zstd" });
			CHECK_EQ(original_empty, decompressed);
		}

		{
			CHECK(Compression::CompressToFile(original_tiny, U"../../Test/output/compression/original_tiny.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_tiny.bin.zstd" });
			CHECK_EQ(original_tiny, decompressed);
		}

		{
			CHECK(Compression::CompressToFile(original_small, U"../../Test/output/compression/original_small.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_small.bin.zstd" });
			CHECK_EQ(original_small, decompressed);
		}

		{
			CHECK(Compression::CompressToFile(original_medium, U"../../Test/output/compression/original_medium.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_medium.bin.zstd" });
			CHECK_EQ(original_medium, decompressed);
		}

		{
			CHECK(Compression::CompressToFile(original_large, U"../../Test/output/compression/original_large.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_large.bin.zstd" });
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("CompressFileToFile")
	{
		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_empty.bin", U"../../Test/output/compression/original_empty.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_empty.bin.zstd" });
			CHECK_EQ(original_empty, decompressed);
		}

		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_tiny.bin", U"../../Test/output/compression/original_tiny.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_tiny.bin.zstd" });
			CHECK_EQ(original_tiny, decompressed);
		}

		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_small.bin", U"../../Test/output/compression/original_small.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_small.bin.zstd" });
			CHECK_EQ(original_small, decompressed);
		}

		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_medium.bin", U"../../Test/output/compression/original_medium.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_medium.bin.zstd" });
			CHECK_EQ(original_medium, decompressed);
		}

		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_large.bin", U"../../Test/output/compression/original_large.bin.zstd"));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_large.bin.zstd" });
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("CompressFile nThreads = 8")
	{
		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_empty.bin", Compression::DefaultLevel, nThreads);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_empty, decompressed);
		}

		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_small.bin", Compression::DefaultLevel, nThreads);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_small, decompressed);
		}

		{
			const Blob compressed = Compression::CompressFile(U"../../Test/output/compression/original_large.bin", Compression::DefaultLevel, nThreads);
			const Blob decompressed = Compression::Decompress(compressed);
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("CompressToFile nThreads = 8")
	{
		{
			CHECK(Compression::CompressToFile(original_empty, U"../../Test/output/compression/original_empty.bin.zstd", Compression::DefaultLevel, nThreads));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_empty.bin.zstd" });
			CHECK_EQ(original_empty, decompressed);
		}

		{
			CHECK(Compression::CompressToFile(original_small, U"../../Test/output/compression/original_small.bin.zstd", Compression::DefaultLevel, nThreads));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_small.bin.zstd" });
			CHECK_EQ(original_small, decompressed);
		}

		{
			CHECK(Compression::CompressToFile(original_large, U"../../Test/output/compression/original_large.bin.zstd", Compression::DefaultLevel, nThreads));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_large.bin.zstd" });
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("CompressFileToFile nThreads = 8")
	{
		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_empty.bin", U"../../Test/output/compression/original_empty.bin.zstd", Compression::DefaultLevel, nThreads));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_empty.bin.zstd" });
			CHECK_EQ(original_empty, decompressed);
		}

		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_small.bin", U"../../Test/output/compression/original_small.bin.zstd", Compression::DefaultLevel, nThreads));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_small.bin.zstd" });
			CHECK_EQ(original_small, decompressed);
		}

		{
			CHECK(Compression::CompressFileToFile(U"../../Test/output/compression/original_large.bin", U"../../Test/output/compression/original_large.bin.zstd", Compression::DefaultLevel, nThreads));
			const Blob decompressed = Compression::Decompress(Blob{ U"../../Test/output/compression/original_large.bin.zstd" });
			CHECK_EQ(original_large, decompressed);
		}
	}

	FileSystem::Remove(U"../../Test/output/compression/");
}
