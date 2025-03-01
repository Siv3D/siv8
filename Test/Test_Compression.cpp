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

# if SIV3D_RUN_HEAVY_TEST

TEST_CASE("Compression")
{
	constexpr int32 nThreads = 8;

	const Blob original_empty = MakeRandomBlob(0); // 0 B
	const Blob original_tiny = MakeRandomBlob(7); // 7 B
	const Blob original_small = MakeRandomBlob(1024 + 111); // 1 KB
	const Blob original_medium = MakeRandomBlob(1024 * 320 + 333); // 320 KB
	const Blob original_large = MakeRandomBlob(1024 * 1024 * 2 + 555); // 2 MB

	const Blob compressed_empty = Compression::Compress(original_empty);
	const Blob compressed_tiny = Compression::Compress(original_tiny);
	const Blob compressed_small = Compression::Compress(original_small);
	const Blob compressed_medium = Compression::Compress(original_medium);
	const Blob compressed_large = Compression::Compress(original_large);

	original_empty.save(U"../../Test/output/compression/original_empty.bin");
	original_tiny.save(U"../../Test/output/compression/original_tiny.bin");
	original_small.save(U"../../Test/output/compression/original_small.bin");
	original_medium.save(U"../../Test/output/compression/original_medium.bin");
	original_large.save(U"../../Test/output/compression/original_large.bin");

	SUBCASE("Compress")
	{
		CHECK_EQ(original_empty, Compression::Decompress(compressed_empty));
		CHECK_EQ(original_tiny, Compression::Decompress(compressed_tiny));
		CHECK_EQ(original_small, Compression::Decompress(compressed_small));
		CHECK_EQ(original_medium, Compression::Decompress(compressed_medium));
		CHECK_EQ(original_large, Compression::Decompress(compressed_large));
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

	SUBCASE("DecompressFile")
	{
		compressed_empty.save(U"../../Test/output/compression/original_empty.bin.zstd");
		compressed_tiny.save(U"../../Test/output/compression/original_tiny.bin.zstd");
		compressed_small.save(U"../../Test/output/compression/original_small.bin.zstd");
		compressed_medium.save(U"../../Test/output/compression/original_medium.bin.zstd");
		compressed_large.save(U"../../Test/output/compression/original_large.bin.zstd");

		{
			Blob decompressed;
			CHECK(Compression::DecompressFile(U"../../Test/output/compression/original_empty.bin.zstd", decompressed));
			CHECK_EQ(original_empty, decompressed);
		}

		{
			Blob decompressed;
			CHECK(Compression::DecompressFile(U"../../Test/output/compression/original_tiny.bin.zstd", decompressed));
			CHECK_EQ(original_tiny, decompressed);
		}

		{
			Blob decompressed;
			CHECK(Compression::DecompressFile(U"../../Test/output/compression/original_small.bin.zstd", decompressed));
			CHECK_EQ(original_small, decompressed);
		}

		{
			Blob decompressed;
			CHECK(Compression::DecompressFile(U"../../Test/output/compression/original_medium.bin.zstd", decompressed));
			CHECK_EQ(original_medium, decompressed);
		}

		{
			Blob decompressed;
			CHECK(Compression::DecompressFile(U"../../Test/output/compression/original_large.bin.zstd", decompressed));
			CHECK_EQ(original_large, decompressed);
		}
	}

	SUBCASE("DecompressToFile")
	{
		{
			CHECK(Compression::DecompressToFile(compressed_empty, U"../../Test/output/compression/original_empty.out"));
			CHECK(original_empty == Blob{ U"../../Test/output/compression/original_empty.out" });
		}

		{
			CHECK(Compression::DecompressToFile(compressed_tiny, U"../../Test/output/compression/original_tiny.out"));
			CHECK(original_tiny == Blob{ U"../../Test/output/compression/original_tiny.out" });
		}

		{
			CHECK(Compression::DecompressToFile(compressed_small, U"../../Test/output/compression/original_small.out"));
			CHECK(original_small == Blob{ U"../../Test/output/compression/original_small.out" });
		}

		{
			CHECK(Compression::DecompressToFile(compressed_medium, U"../../Test/output/compression/original_medium.out"));
			CHECK(original_medium == Blob{ U"../../Test/output/compression/original_medium.out" });
		}

		{
			CHECK(Compression::DecompressToFile(compressed_large, U"../../Test/output/compression/original_large.out"));
			CHECK(original_large == Blob{ U"../../Test/output/compression/original_large.out" });
		}
	}

	SUBCASE("DecompressFileToFile")
	{
		compressed_empty.save(U"../../Test/output/compression/original_empty.bin.zstd");
		compressed_tiny.save(U"../../Test/output/compression/original_tiny.bin.zstd");
		compressed_small.save(U"../../Test/output/compression/original_small.bin.zstd");
		compressed_medium.save(U"../../Test/output/compression/original_medium.bin.zstd");
		compressed_large.save(U"../../Test/output/compression/original_large.bin.zstd");

		{
			CHECK(Compression::DecompressFileToFile(U"../../Test/output/compression/original_empty.bin.zstd", U"../../Test/output/compression/original_empty.out"));
			CHECK(original_empty == Blob{ U"../../Test/output/compression/original_empty.out" });
		}

		{
			CHECK(Compression::DecompressFileToFile(U"../../Test/output/compression/original_tiny.bin.zstd", U"../../Test/output/compression/original_tiny.out"));
			CHECK(original_tiny == Blob{ U"../../Test/output/compression/original_tiny.out" });
		}

		{
			CHECK(Compression::DecompressFileToFile(U"../../Test/output/compression/original_small.bin.zstd", U"../../Test/output/compression/original_small.out"));
			CHECK(original_small == Blob{ U"../../Test/output/compression/original_small.out" });
		}

		{
			CHECK(Compression::DecompressFileToFile(U"../../Test/output/compression/original_medium.bin.zstd", U"../../Test/output/compression/original_medium.out"));
			CHECK(original_medium == Blob{ U"../../Test/output/compression/original_medium.out" });
		}

		{
			CHECK(Compression::DecompressFileToFile(U"../../Test/output/compression/original_large.bin.zstd", U"../../Test/output/compression/original_large.out"));
			CHECK(original_large == Blob{ U"../../Test/output/compression/original_large.out" });
		}
	}
}

# endif

# if SIV3D_RUN_BENCHMARK && SIV3D_RUN_HEAVY_TEST

TEST_CASE("Compression.Benchmark")
{
	const Blob original = MakeRandomBlob(1024 * 1024 * 64); // 64 MB
	const ScopedLogSilencer logSilencer;

	Console << U"\n----------------";
	{
		Console << U"Compression::CompressToFile(nThreads = 0)";
		MillisecClock clock;
		Compression::CompressToFile(original, U"../../Test/output/compression/original_large.out");
		Console << U"| {} ms"_fmt(clock.ms());
	}

	{
		Console << U"Compression::CompressToFile(nThreads = 2)";
		MillisecClock clock;
		Compression::CompressToFile(original, U"../../Test/output/compression/original_large.out", Compression::DefaultLevel, 2);
		Console << U"| {} ms"_fmt(clock.ms());
	}

	{
		Console << U"Compression::CompressToFile(nThreads = 4)";
		MillisecClock clock;
		Compression::CompressToFile(original, U"../../Test/output/compression/original_large.out", Compression::DefaultLevel, 4);
		Console << U"| {} ms"_fmt(clock.ms());
	}
	Console << U"----------------\n";
}

# endif
