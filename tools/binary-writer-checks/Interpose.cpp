// macOS-only fault injection. Loaded only by run-binary-writer-checks.sh.
# include <cerrno>
# include <cstdio>
# include <cstring>
# include <fcntl.h>
# include <limits.h>

namespace
{
	struct Fault
	{
		char path[PATH_MAX]{};
		int kind = 0; // 1: failed write, 2: short write, 3: flush, 4: close
		int hits = 0;
	};

	// Engine worker threads must never see faults armed by the test thread.
	thread_local Fault fault;

	bool Matches(std::FILE* file, const int kind)
	{
		if ((fault.kind != kind) || (file == nullptr)) { return false; }
		char path[PATH_MAX]{};
		return ((::fcntl(::fileno(file), F_GETPATH, path) == 0)
			&& (std::strcmp(path, fault.path) == 0));
	}

	void ConsumeFault()
	{
		fault.kind = 0;
		++fault.hits;
		errno = ENOSPC;
	}

	size_t Write(const void* data, const size_t size, const size_t count, std::FILE* file)
	{
		const auto real = &std::fwrite;
		if (Matches(file, 1))
		{
			ConsumeFault();
			return 0;
		}
		if (Matches(file, 2) && (count > 0))
		{
			const size_t written = real(data, size, (count / 2), file);
			ConsumeFault();
			return written;
		}
		return real(data, size, count, file);
	}

	int Flush(std::FILE* file)
	{
		const auto real = &std::fflush;
		const bool fail = Matches(file, 3);
		const int result = real(file);
		if (fail) { ConsumeFault(); return EOF; }
		return result;
	}

	int Close(std::FILE* file)
	{
		const auto real = &std::fclose;
		const bool fail = Matches(file, 4);
		// Always close the real descriptor, including the injected failure case.
		const int result = real(file);
		if (fail) { ConsumeFault(); return EOF; }
		return result;
	}
}

extern "C" void Siv3DWriterCheckArm(const char* path, const int kind)
{
	fault = {};
	std::strncpy(fault.path, path, (sizeof(fault.path) - 1));
	fault.kind = kind;
}

extern "C" int Siv3DWriterCheckHits()
{
	return fault.hits;
}

// dyld's interpose section contains replacement/original address pairs.
__attribute__((used))
static const struct
{
	const void* replacement;
	const void* original;
} Interpositions[] __attribute__((section("__DATA,__interpose"))) = {
	{ reinterpret_cast<const void*>(&Write), reinterpret_cast<const void*>(&std::fwrite) },
	{ reinterpret_cast<const void*>(&Flush), reinterpret_cast<const void*>(&std::fflush) },
	{ reinterpret_cast<const void*>(&Close), reinterpret_cast<const void*>(&std::fclose) },
};
