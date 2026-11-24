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

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include <dlfcn.h>

namespace
{
	struct WriterFaultInjection
	{
		using Arm = void (*)(const char*, int);
		using Hits = int (*)();
		Arm arm = reinterpret_cast<Arm>(::dlsym(RTLD_DEFAULT, "Siv3DWriterCheckArm"));
		Hits hits = reinterpret_cast<Hits>(::dlsym(RTLD_DEFAULT, "Siv3DWriterCheckHits"));

		~WriterFaultInjection()
		{
			if (arm) { arm("", 0); }
		}
	};
}

// Explicitly run with tools/run-binary-writer-checks.sh, not the normal suite.
TEST_CASE("BinaryFileWriter.injected_write_and_flush_errors", "[.writer-errors]")
{
	WriterFaultInjection fault;
	REQUIRE(fault.arm);
	REQUIRE(fault.hits);
	const auto path = Test::OutputPath(U"binarywriter/injected-write.bin");
	const auto utf8Path = path.toUTF8();
	const Blob large{ size_t{ 16384 } };
	for (const int kind : { 1, 2, 3 }) // failed write, short write, failed stdio flush
	{
		for (const bool automatic : { false, true })
		{
			CAPTURE(kind, automatic);
			BinaryFileWriter writer{ path };
			REQUIRE(writer.isOpen());
			REQUIRE(writer.write("abcdef", 6) == 6);
			fault.arm(utf8Path.c_str(), kind);
			if (automatic)
			{
				CHECK(writer.write(large.data(), large.size()) == 0);
			}
			CHECK_FALSE(writer.flush());
			CHECK(fault.hits() == 1);
			CHECK(writer.write("ignored", 7) == 0);
			writer.clear(); // Clearing must not erase the error from this open session.
			CHECK_FALSE(writer.close());
			CHECK_FALSE(writer.isOpen());
			CHECK_FALSE(writer.flush());
			CHECK_FALSE(writer.close());
			CHECK_FALSE(writer.open(U""));
			CHECK_FALSE(writer.close());
			REQUIRE(writer.open(path));
			REQUIRE(writer.write("ok", 2) == 2);
			CHECK(writer.close());
			CHECK((Blob{ path } == Blob{ "ok", 2 }));
		}
	}
	for (const int kind : { 1, 2 })
	{
		CAPTURE(kind);
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
		fault.arm(utf8Path.c_str(), kind);
		CHECK(writer.write(large.data(), large.size()) == ((kind == 1) ? 0 : 8192));
		CHECK_FALSE(writer.close());
		CHECK(fault.hits() == 1);
	}
	BinaryFileWriter direct{ path };
	REQUIRE(direct.isOpen());
	REQUIRE(direct.write(large.data(), large.size()) == 16384);
	fault.arm(utf8Path.c_str(), 3);
	CHECK_FALSE(direct.flush()); // The engine buffer is empty; stdio must still be flushed.
	CHECK(fault.hits() == 1);
	CHECK_FALSE(direct.close());
}

TEST_CASE("BinaryFileWriter.injected_close_errors_and_blob_save", "[.writer-errors]")
{
	WriterFaultInjection fault;
	REQUIRE(fault.arm);
	REQUIRE(fault.hits);
	const auto path = Test::OutputPath(U"binarywriter/injected-save.bin");
	const auto utf8Path = path.toUTF8();
	BinaryFileWriter writer{ path };
	REQUIRE(writer.isOpen());
	REQUIRE(writer.write("abc", 3) == 3);
	fault.arm(utf8Path.c_str(), 4);
	CHECK_FALSE(writer.close());
	CHECK(fault.hits() == 1);
	CHECK_FALSE(writer.isOpen());
	CHECK_FALSE(writer.close());
	CHECK_FALSE(writer.flush());
	REQUIRE(writer.open(path));
	CHECK(writer.close());
	for (const int kind : { 1, 2, 3, 4 })
	{
		for (const size_t size : { 1, 16, 16384 })
		{
			CAPTURE(kind, size);
			const Blob blob{ size };
			fault.arm(utf8Path.c_str(), kind);
			CHECK_FALSE(blob.save(path));
			CHECK(fault.hits() == 1);
			REQUIRE(blob.save(path));
			CHECK(Blob{ path } == blob);
		}
	}
	fault.arm(utf8Path.c_str(), 4);
	CHECK_FALSE(Blob{}.save(path));
	CHECK(fault.hits() == 1);
}

# endif

TEST_CASE("BinaryFileWriter.flush_and_close_results")
{
	BinaryFileWriter writer;
	CHECK(writer.flush());
	CHECK(writer.close());
	const auto path = Test::OutputPath(U"binarywriter/flush-close.bin");
	for (const size_t size : { 0, 1, 8192, 8193, 32768 })
	{
		CAPTURE(size);
		Blob data{ size };
		for (size_t i = 0; i < size; ++i) { data[i] = static_cast<Byte>(i % 251); }
		REQUIRE(writer.open(path));
		REQUIRE(writer.write(data.data(), size) == static_cast<int64>(size));
		CHECK(writer.flush());
		CHECK(writer.flush());
		CHECK(writer.close());
		CHECK_FALSE(writer.isOpen());
		CHECK(writer.path().isEmpty());
		CHECK(writer.close());
		CHECK(writer.flush());
		CHECK(Blob{ path } == data);
	}
}

TEST_CASE("BinaryFileWriter.buffered_overwrite_append_and_clear")
{
	const auto path = Test::OutputPath(U"binarywriter/overwrite.bin");
	BinaryFileWriter writer{ path };
	REQUIRE(writer.isOpen());
	REQUIRE(writer.write("abcdef", 6) == 6);
	REQUIRE(writer.setPos(2));
	REQUIRE(writer.write("XY", 2) == 2);
	REQUIRE(writer.close());
	CHECK((Blob{ path } == Blob{ "abXYef", 6 }));
	REQUIRE(writer.open(path, FileWriteMode::Append));
	REQUIRE(writer.write("gh", 2) == 2);
	REQUIRE(writer.close());
	CHECK((Blob{ path } == Blob{ "abXYefgh", 8 }));
	REQUIRE(writer.open(path));
	REQUIRE(writer.write("discard", 7) == 7);
	writer.clear();
	REQUIRE(writer.write("new", 3) == 3);
	REQUIRE(writer.close());
	CHECK((Blob{ path } == Blob{ "new", 3 }));
}

TEST_CASE("BinaryFileWriter.constructor")
{
	{
		const FilePath path{ Test::OutputPath(U"binarywriter/test.bin") };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ Test::OutputPath(U"binarywriter/.test") };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ Test::OutputPath(U"binarywriter/test/") };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(not writer.isOpen());
		}
		CHECK(not FileSystem::Exists(path));
	}
}

TEST_CASE("BinaryFileWriter.open")
{
	{
		const FilePath path{ Test::OutputPath(U"binarywriter/test.bin") };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer;
			CHECK(writer.open(path));
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ Test::OutputPath(U"binarywriter/.test") };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer;
			CHECK(writer.open(path));
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ Test::OutputPath(U"binarywriter/test/") };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer;
			CHECK(not writer.open(path));
			CHECK(not writer.isOpen());
		}
		CHECK(not FileSystem::Exists(path));
	}
}
