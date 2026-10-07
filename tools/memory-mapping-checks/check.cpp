// Standalone fault injection against the real public API and Windows backends.
# include <Siv3D/MemoryMappedFile.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Windows/Libraries.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <array>
# include <cstring>
# include <filesystem>
# include <fstream>
# include <iostream>
# include <stdexcept>
# include <type_traits>

void Main() {} // Link the engine library without starting the engine.

namespace
{
	struct Probe
	{
		bool failNext = false;
		bool handlesValid = true;
		bool closesSucceeded = true;
		size_t mapCalls = 0;
		size_t mappingCloses = 0;
		HANDLE lastMapping = nullptr;
	};

	Probe probe;

	void Require(const bool condition, const char* message)
	{
		if (not condition)
		{
			throw std::runtime_error(message);
		}
	}

	template <class File>
	void CheckFailure(const std::filesystem::path& directory, const bool extend)
	{
		constexpr bool writable = std::is_same_v<File, s3d::MemoryMappedFile>;
		const char* name = writable ? (extend ? "writable-extension.bin" : "writable.bin") : "readonly.bin";
		const auto nativePath = (directory / name);
		const auto path = s3d::Unicode::FromWstring(nativePath.wstring());
		std::array<char, 32> original{};
		for (size_t i = 0; i < original.size(); ++i)
		{
			original[i] = static_cast<char>(i + 1);
		}
		{
			std::ofstream stream{ nativePath, std::ios::binary | std::ios::trunc };
			stream.write(original.data(), original.size());
			stream.close();
			Require(not stream.fail(), "Could not create fixture");
		}

		File file;
		if constexpr (writable)
		{
			Require(file.open(path, File::ExistingFilePolicy::JustOpen, File::MissingFilePolicy::Fail), "Could not open fixture");
		}
		else
		{
			Require(file.open(path), "Could not open fixture");
		}
		const auto openedPath = file.path();
		size_t expectedSize = original.size();
		constexpr size_t offset = 5;
		size_t count = 7;
		// Consecutive failures must each release their handle and keep the slot free.
		for (int attempt = 0; attempt < 3; ++attempt)
		{
			count = extend ? (expectedSize + 17) : 7;
			if (extend)
			{
				expectedSize = (offset + count);
			}
			probe = { .failNext = true };
			const auto failed = file.map(offset, count);
			Require((not failed) && (failed.data == nullptr) && (failed.size == 0), "Failure must return an empty range");
			Require((probe.mapCalls == 1) && (not probe.failNext) && probe.handlesValid,
				"Injection must receive a valid CreateFileMappingW handle");
			Require((probe.mappingCloses == 1) && probe.closesSucceeded, "Failed view leaked its mapping handle");
			Require(file.isOpen() && (file.path() == openedPath), "Failure changed the open file");
			Require(file.size() == static_cast<s3d::int64>(expectedSize), "Cached file size did not reflect extension");
			Require(std::filesystem::file_size(nativePath) == expectedSize, "OS file size did not reflect extension");
			if constexpr (writable)
			{
				Require(file.flush(), "Flush after a failed view failed");
			}
			// Do not unmap between failures or before the retry: that could hide stale state.
		}

		probe = {};
		const auto mapped = file.map(offset, count);
		Require(mapped && (mapped.size == count), "Mapping retry failed without reopening");
		Require((probe.mapCalls == 1) && probe.handlesValid && (probe.mappingCloses == 0),
			"Retry did not keep a live mapping handle");
		Require(not file.mapAll(), "Retry broke the single-mapping constraint");
		Require(probe.mapCalls == 1, "An occupied mapping slot reached Win32");
		if constexpr (writable)
		{
			std::memset(mapped.data, 0x5A, mapped.size);
			Require(file.flush(), "Flush after retry failed");
		}
		else
		{
			Require(std::memcmp(mapped.data, original.data() + offset, count) == 0, "Retry returned wrong bytes");
		}
		file.unmap();
		Require((probe.mappingCloses == 1) && probe.closesSucceeded, "Unmap did not release the retry handle");
		file.unmap();
		Require(probe.mappingCloses == 1, "Repeated unmap closed the handle twice");

		probe = {};
		const auto all = file.mapAll();
		Require(all && (all.size == expectedSize), "Remap after unmap failed");
		Require(std::memcmp(all.data, original.data(), offset) == 0, "Failure changed the original prefix");
		file.close();
		Require((probe.mappingCloses == 1) && probe.closesSucceeded, "Close did not release the active mapping");

		// Check persisted bytes independently of the mapping API.
		std::ifstream stream{ nativePath, std::ios::binary };
		for (size_t i = 0; i < expectedSize; ++i)
		{
			const int expected = (writable && (offset <= i) && (i < (offset + count))) ? 0x5A : original.at(i);
			Require(stream.get() == expected, "Persisted file bytes differ after retry");
		}
		Require(stream.get() == std::char_traits<char>::eof(), "Unexpected trailing bytes");
		stream.close();
		Require(std::filesystem::remove(nativePath), "Could not remove closed fixture");
		std::cout << "PASS " << name << ": 3 injected failures, immediate handle release, retry and round trip\n";
	}
}

LPVOID WINAPI MappingCheckMapViewOfFile(HANDLE mapping, DWORD access,
	DWORD offsetHigh, DWORD offsetLow, SIZE_T bytes)
{
	++probe.mapCalls;
	probe.lastMapping = mapping;
	DWORD flags = 0;
	probe.handlesValid &= (::GetHandleInformation(mapping, &flags) != 0);
	if (probe.failNext)
	{
		probe.failNext = false;
		::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return nullptr;
	}
	return ::MapViewOfFile(mapping, access, offsetHigh, offsetLow, bytes);
}

BOOL WINAPI MappingCheckCloseHandle(HANDLE handle)
{
	const BOOL closed = ::CloseHandle(handle);
	if (handle == probe.lastMapping)
	{
		++probe.mappingCloses;
		probe.closesSucceeded &= (closed != 0);
	}
	return closed;
}

int wmain(const int argc, wchar_t** argv)
{
	try
	{
		Require(argc == 2, "Usage: memory-mapping-checks.exe <fixture-directory>");
		const auto directory = std::filesystem::absolute(argv[1]);
		std::filesystem::create_directories(directory);
		CheckFailure<s3d::MemoryMappedFileView>(directory, false);
		CheckFailure<s3d::MemoryMappedFile>(directory, false);
		CheckFailure<s3d::MemoryMappedFile>(directory, true);
		return 0;
	}
	catch (const std::exception& error)
	{
		std::cerr << "FAIL: " << error.what() << '\n';
		return 1;
	}
}
