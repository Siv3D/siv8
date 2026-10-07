// Standalone development tool. All generated files live beneath out-dir.
// --prepare out-dir repo-root | --verify out-dir | --measure out-dir csv [filter]
// Relative CSV paths are relative to out-dir; filter is a scenario-name substring.
// CSV seconds cover the entire fixed batch; checksum is per iteration (not batch).
# include <Siv3D.hpp>
# include <algorithm>
# include <array>
# include <chrono>
# include <cmath>
# include <cstdint>
# include <cstring>
# include <filesystem>
# include <fstream>
# include <iomanip>
# include <iostream>
# include <locale>
# include <stdexcept>
# include <string>
# include <vector>
# ifdef _WIN32
# include <Siv3D/Windows/Windows.hpp>
# include <winioctl.h>
# endif

void Main() {} // Satisfy library link references without initializing the engine.

namespace
{
	namespace fs = std::filesystem;
	using Bytes = std::vector<uint8_t>;
	using Reader = s3d::BinaryFileReader;
	using I64 = s3d::int64;
	constexpr size_t KiB = 1024, MiB = KiB * KiB;
	constexpr I64 FourGiB = I64{ 1 } << 32, SparseSize = FourGiB + 64 * KiB + 3;

	void Require(bool condition, const char* message)
	{
		if (!condition) throw std::runtime_error(message);
	}
	std::string UTF8(const fs::path& path)
	{
		const auto s = path.generic_u8string();
		return { reinterpret_cast<const char*>(s.data()), s.size() };
	}
	s3d::String EnginePath(const fs::path& path)
	{
		return s3d::Unicode::FromUTF8(UTF8(path));
	}
	bool Within(const fs::path& root, const fs::path& path)
	{
		const auto relative = fs::weakly_canonical(path).lexically_relative(fs::weakly_canonical(root));
		return !relative.empty() && !relative.is_absolute() && *relative.begin() != "..";
	}
	fs::path Output(const fs::path& root, const fs::path& relative)
	{
		const auto path = fs::weakly_canonical(root / relative);
		Require(Within(root, path), "Output path escapes out-dir");
		return path;
	}
	uint64_t Next(uint64_t& state)
	{
		uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
		z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
		z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
		return z ^ (z >> 31);
	}
	Bytes Pattern(size_t size, uint64_t seed)
	{
		Bytes bytes(size);
		for (size_t i = 0; i < size;)
		{
			const uint64_t word = Next(seed);
			for (unsigned j = 0; j < 8 && i < size; ++j, ++i)
				bytes[i] = static_cast<uint8_t>(word >> (j * 8));
		}
		return bytes;
	}
	Bytes Records()
	{
		auto bytes = Pattern(MiB + 3, 2);
		for (size_t pos = 0; pos < MiB; pos += 256)
			for (unsigned j = 0; j < 4; ++j)
			{
				bytes[pos + j] = static_cast<uint8_t>(uint32_t{ 240 } >> (j * 8));
				bytes[pos + 4 + j] = static_cast<uint8_t>((pos / 256) >> (j * 8));
			}
		return bytes;
	}
	struct Dialogue { Bytes utf8, utf16{ 0xFF, 0xFE }; std::vector<s3d::String> lines; };
	Dialogue MakeDialogue()
	{
		Dialogue text;
		for (size_t i = 0; i < 1024; ++i)
		{
			s3d::String line = i % 2 ? U"Guide: \u6b21\u306e\u6751\u3078\u884c\u3053\u3046! \U0001F30D " : U"Player: Ready for the next quest? \u306f\u3044! ";
			line.append(63 - line.toUTF8().size(), U'.');
			text.lines.push_back(line);
			line.push_back(U'\n');
			const auto utf8 = line.toUTF8();
			text.utf8.insert(text.utf8.end(), utf8.begin(), utf8.end());
			for (const auto unit : line.toUTF16()) { text.utf16.push_back(static_cast<uint8_t>(unit)); text.utf16.push_back(static_cast<uint8_t>(unit >> 8)); }
		}
		return text;
	}
	template <bool Check> uint64_t ReadDialogue(const s3d::String& path, const Dialogue& text, s3d::String& line)
	{
		s3d::TextFileReader reader{ path }; // Includes encoding detection and open/close overhead.
		Require(reader.isOpen(), "Dialogue open failed");
		uint64_t sum = 0;
		for (const auto& expected : text.lines)
		{
			Require(reader.readLine(line) && line.size() == expected.size(), "Short dialogue line");
			if constexpr (Check) Require(line == expected, "Decoded dialogue line mismatch");
			sum += line.size() + line.front() + line.back();
		}
		Require(!reader.readLine(line), "Unexpected extra dialogue line");
		return sum;
	}
	uint64_t Hash(const void* data, size_t size)
	{
		uint64_t hash = 14695981039346656037ULL;
		const auto* bytes = static_cast<const uint8_t*>(data);
		for (size_t i = 0; i < size; ++i) hash = (hash ^ bytes[i]) * 1099511628211ULL;
		return hash;
	}
	// Cheap consumption in timings; full comparisons and hashes run outside them.
	uint64_t Sample(const void* data, size_t size)
	{
		if (!size) return 0;
		const auto* p = static_cast<const uint8_t*>(data);
		return p[0] + (uint64_t{ p[size / 2] } << 8) + (uint64_t{ p[size - 1] } << 16);
	}
	Bytes Load(const fs::path& path)
	{
		std::ifstream in(path, std::ios::binary);
		Require(in.is_open(), "Cannot open input with std::ifstream");
		Bytes bytes(static_cast<size_t>(fs::file_size(path)));
		if (!bytes.empty()) in.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
		Require(bool(in), "Short std::ifstream read");
		Require(in.peek() == std::char_traits<char>::eof(), "Input size changed");
		return bytes;
	}
	void Write(const fs::path& path, const Bytes& bytes)
	{
		std::ofstream out(path, std::ios::binary | std::ios::trunc);
		if (!bytes.empty()) out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		out.close();
		Require(bool(out), "Fixture write failed");
	}

	uint8_t SparseByte(I64 pos)
	{
		if (pos >= FourGiB - 32 && pos < FourGiB + 96)
			return static_cast<uint8_t>((pos - FourGiB + 32) * 73 + 19);
		return pos >= SparseSize - 3 ? static_cast<uint8_t>(pos - SparseSize + 131) : 0;
	}

	bool PrepareSparse(const fs::path& path)
	{
# ifdef _WIN32
		struct Handle { HANDLE value; ~Handle() { if (value != INVALID_HANDLE_VALUE) CloseHandle(value); } };
		Handle file{ CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr) };
		if (file.value == INVALID_HANDLE_VALUE) return false;
		DWORD returned = 0;
		if (!DeviceIoControl(file.value, FSCTL_SET_SPARSE, nullptr, 0, nullptr, 0, &returned, nullptr)) return false;
		LARGE_INTEGER end{};
		end.QuadPart = SparseSize;
		if (!SetFilePointerEx(file.value, end, nullptr, FILE_BEGIN) || !SetEndOfFile(file.value)) return false;
		for (const I64 pos : { FourGiB - 32, SparseSize - 3 })
		{
			std::array<uint8_t, 128> bytes{};
			const DWORD count = (pos == FourGiB - 32) ? 128 : 3;
			for (DWORD i = 0; i < count; ++i) bytes[i] = SparseByte(pos + i);
			end.QuadPart = pos;
			if (!SetFilePointerEx(file.value, end, nullptr, FILE_BEGIN)
				|| !WriteFile(file.value, bytes.data(), count, &returned, nullptr) || returned != count) return false;
		}
		return FlushFileBuffers(file.value) != 0;
# else
		(void)path;
		return false;
# endif
	}

	struct Asset { std::string group, relative; uint64_t size = 0, hash = 0; };
	struct Config
	{
		fs::path repo;
		bool sparse = false;
		std::string font;
		uint64_t decodedSize = 0, decodedHash = 0;
		std::vector<Asset> assets;
	};

	void Prepare(const fs::path& root, const fs::path& repo)
	{
		Require(fs::is_directory(repo / "Siv3D/include"), "repo-root does not contain Siv3D/include");
		fs::create_directories(Output(root, "fixtures/small-files"));
		Write(Output(root, "fixtures/big.bin"), Pattern(64 * MiB, 1));
		Write(Output(root, "fixtures/records.bin"), Records());
		Write(Output(root, "fixtures/empty.bin"), {});
		const auto dialogue = MakeDialogue();
		Write(Output(root, "fixtures/dialogue-utf8.txt"), dialogue.utf8);
		Write(Output(root, "fixtures/dialogue-utf16.txt"), dialogue.utf16);
		for (size_t i = 0; i < 128; ++i)
			Write(Output(root, "fixtures/small-files/" + std::to_string(i) + ".bin"), Pattern((1 + i * 37 % 64) * KiB, 1000 + i));
		Config config;
		config.repo = fs::canonical(repo);
		config.sparse = PrepareSparse(Output(root, "fixtures/sparse.bin"));
		if (!config.sparse) std::cerr << "UNVERIFIED: sparse >4GiB fixture unavailable on this filesystem/host\n";
		const std::string base = fs::is_directory(repo / "WindowsDesktop/App") ? "WindowsDesktop/App/" : "macOS/App/";
		const auto add = [&](const char* group, const std::string& relative)
		{
			const auto path = repo / fs::u8path(relative);
			if (!fs::is_regular_file(path)) { std::cerr << "UNVERIFIED: missing asset " << relative << '\n'; return; }
			Require(Within(repo, path), "Asset escapes repo-root");
			const auto bytes = Load(path);
			config.assets.push_back({ group, relative, bytes.size(), Hash(bytes.data(), bytes.size()) });
		};
		for (const char* name : { "2d.vs", "fullscreen_triangle.vs", "2d_texture.ps", "2d_shape.ps" })
			add("shaders", base + "engine/shader/d3d11/" + name);
		for (const char* name : { "example/windmill.png", "example/siv3d-kun.png", "engine/texture/box-shadow/64.png" }) add("png", base + name);
		add("fonts", base + "engine/font/min/siv3d-min.otf.zstdcmp");
		const auto font = base + "engine/font/noto-cjk/NotoSansJP-Medium.otf.zstdcmp";
		add("fonts", font);
		if (fs::is_regular_file(repo / fs::u8path(font)))
		{
			const auto compressed = Load(repo / fs::u8path(font));
			s3d::Blob decoded;
			Require(s3d::Compression::Decompress(compressed.data(), compressed.size(), decoded) && !decoded.isEmpty(), "Reference font decode failed");
			config.font = font;
			config.decodedSize = decoded.size();
			config.decodedHash = Hash(decoded.data(), decoded.size());
		}
		std::ofstream manifest(Output(root, "fixtures/manifest.txt"), std::ios::binary | std::ios::trunc);
		manifest << "BFR_BENCH_2\n" << std::quoted(UTF8(config.repo)) << '\n' << config.sparse << '\n'
			<< std::quoted(config.font) << ' ' << config.decodedSize << ' ' << config.decodedHash << '\n' << config.assets.size() << '\n';
		for (const auto& a : config.assets) manifest << a.group << ' ' << std::quoted(a.relative) << ' ' << a.size << ' ' << a.hash << '\n';
		manifest.close();
		Require(bool(manifest), "Manifest write failed");
		std::cerr << "Prepared deterministic fixtures and " << config.assets.size() << " read-only repository assets\n";
	}

	Config ReadConfig(const fs::path& root)
	{
		std::ifstream in(Output(root, "fixtures/manifest.txt"), std::ios::binary);
		std::string magic, repo;
		Config c;
		size_t count = 0;
		in >> magic >> std::quoted(repo) >> c.sparse >> std::quoted(c.font) >> c.decodedSize >> c.decodedHash >> count;
		Require(bool(in) && magic == "BFR_BENCH_2" && count <= 1000, "Invalid/missing manifest; run --prepare");
		c.repo = fs::u8path(repo);
		c.assets.resize(count);
		for (auto& a : c.assets)
		{
			in >> a.group >> std::quoted(a.relative) >> a.size >> a.hash;
			Require(bool(in) && Within(c.repo, c.repo / fs::u8path(a.relative)), "Invalid asset manifest entry");
		}
		Require(c.font.empty() || Within(c.repo, c.repo / fs::u8path(c.font)), "Invalid font manifest path");
		return c;
	}

	struct File { s3d::String path; Bytes bytes; std::string group; };
	File Model(const fs::path& path, Bytes bytes, std::string group = {})
	{
		return { EnginePath(path), std::move(bytes), std::move(group) };
	}

	void Equal(const void* data, const Bytes& model, size_t pos, size_t size)
	{
		Require(pos <= model.size() && size <= model.size() - pos, "Invalid model range");
		Require(!size || std::memcmp(data, model.data() + pos, size) == 0, "Byte mismatch against in-memory model");
	}

	void ReadFull(Reader& reader, void* destination, size_t size)
	{
		Require(reader.read(destination, static_cast<I64>(size)) == static_cast<I64>(size), "Short BinaryFileReader read");
	}

	void Seek(Reader& reader, I64 pos)
	{
		Require(reader.setPos(pos) == pos, "Seek failed");
	}

	void VerifyFile(const File& file)
	{
		Reader reader{ file.path };
		Require(reader.isOpen() && reader.size() == static_cast<I64>(file.bytes.size()), "File open/size mismatch");
		Bytes buffer(64 * KiB + 17);
		for (size_t pos = 0; pos < file.bytes.size();)
		{
			const auto count = std::min(buffer.size(), file.bytes.size() - pos);
			ReadFull(reader, buffer.data(), count);
			Equal(buffer.data(), file.bytes, pos, count);
			pos += count;
			Require(reader.getPos() == static_cast<I64>(pos), "Sequential position mismatch");
		}
		Require(reader.isEOF() && reader.remaining() == 0 && reader.read(buffer.data(), 1) == 0, "EOF mismatch");
	}

	void VerifyTrace(const File& file)
	{
		Reader r{ file.path };
		const I64 end = static_cast<I64>(file.bytes.size());
		Bytes buffer(64 * KiB + 33);
		for (const I64 boundary : { I64{ 4 * KiB }, I64{ 16 * KiB }, I64{ 64 * KiB } })
			for (const I64 delta : { -17, -1, 0, 1 })
			{
				const I64 pos = boundary + delta;
				Seek(r, pos);
				Require(r.lookahead(buffer.data(), buffer.size()) == static_cast<I64>(buffer.size()) && r.getPos() == pos, "Boundary lookahead mismatch");
				Equal(buffer.data(), file.bytes, pos, buffer.size());
				ReadFull(r, buffer.data(), buffer.size());
				Equal(buffer.data(), file.bytes, pos, buffer.size());
			}
		uint64_t seed = 42;
		I64 pos = 0;
		Seek(r, pos);
		for (size_t step = 0; step < 12000; ++step)
		{
			const auto op = Next(seed) % 8;
			I64 at = static_cast<I64>(Next(seed) % (end + 1));
			const I64 length = static_cast<I64>(Next(seed) % buffer.size());
			if (step % 11 == 0) at = end - static_cast<I64>(Next(seed) % 4);
			if (op < 2)
			{
				const I64 offset = at - end / 2;
				pos = std::clamp<I64>(op == 0 ? offset : pos + offset, 0, end);
				Require((op == 0 ? r.setPos(offset) : r.skip(offset)) == pos, "Random seek/skip mismatch");
			}
			else
			{
				const bool positioned = (op == 3 || op == 5 || op == 7), exact = op >= 6, peek = (op == 4 || op == 5);
				const I64 start = positioned ? at : pos, count = std::min(length, end - start);
				if (exact)
				{
					const bool ok = positioned ? r.readExact(buffer.data(), at, length) : r.readExact(buffer.data(), length);
					Require(ok == (count == length), "Random readExact result mismatch");
					if (ok && length) { Equal(buffer.data(), file.bytes, start, count); pos = start + count; }
				}
				else
				{
					const I64 actual = peek ? (positioned ? r.lookahead(buffer.data(), at, length) : r.lookahead(buffer.data(), length))
						: (positioned ? r.read(buffer.data(), at, length) : r.read(buffer.data(), length));
					Require(actual == count, "Random read/lookahead length mismatch");
					Equal(buffer.data(), file.bytes, start, count);
					if (!peek && length) pos = start + count;
				}
			}
			Require(r.getPos() == pos && r.remaining() == end - pos && r.isEOF() == (pos == end), "Random sequence position mismatch");
		}
		Seek(r, end - 3);
		Require(!r.readExact(buffer.data(), 4) && r.getPos() == end - 3, "Failed readExact moved position");
		Seek(r, 29);
		Require(!r.readExact(buffer.data(), end - 3, 4) && r.getPos() == 29, "Failed positioned readExact moved position");
		Require(r.lookahead(buffer.data(), end - 3, 16) == 3 && r.getPos() == 29, "EOF lookahead restore failed");
		Equal(buffer.data(), file.bytes, end - 3, 3);
		ReadFull(r, buffer.data(), 16);
		Equal(buffer.data(), file.bytes, 29, 16);
		Seek(r, end - 97);
		for (I64 at = end - 97; at < end; ++at)
		{
			ReadFull(r, buffer.data(), 1);
			Require(buffer[0] == file.bytes[at] && r.getPos() == at + 1, "Tiny reads through final prefetched buffer failed");
		}
		Seek(r, end - 3);
		uint32_t scalar = 0;
		Require(!r.read(scalar) && r.getPos() == end && r.read(buffer.data(), 1) == 0, "Partial scalar/EOF mismatch");
		Require(r.setPos(end + 1) == end && r.setPos(-1) == 0, "Seek clamping mismatch");
		Require(r.skip(INT64_MAX) == end && r.skip(INT64_MIN) == 0, "Skip clamping mismatch");
		Require(r.read(nullptr, -1) == 0 && r.lookahead(nullptr, -1, 0) == 0 && r.readExact(nullptr, -1, 0), "Zero/negative size mismatch");
		Require(r.getPos() == 0, "No-op changed position");
		for (int op = 0; op < 6; ++op)
		{
			bool threw = false;
			try
			{
				if (op == 0) r.read(nullptr, 1);
				if (op == 1) r.lookahead(nullptr, 1);
				if (op == 2) (void)r.readExact(nullptr, 1);
				if (op == 3) r.read(buffer.data(), -1, 1);
				if (op == 4) r.lookahead(buffer.data(), end + 1, 1);
				if (op == 5) (void)r.readExact(buffer.data(), end + 1, 1);
			}
			catch (const s3d::Error&) { threw = true; }
			Require(threw && r.getPos() == 0, "Invalid argument did not throw/preserve position");
		}
		Seek(r, 65533);
		Reader moved{ std::move(r) };
		Require(!r.isOpen() && moved.getPos() == 65533, "Move construction mismatch");
		Require(r.open(file.path) && r.getPos() == 0, "Moved-from reopen failed");
		r = std::move(moved);
		Require(!moved.isOpen() && r.getPos() == 65533, "Move assignment mismatch");
		ReadFull(r, buffer.data(), 16);
		Equal(buffer.data(), file.bytes, 65533, 16);
		r.close(); r.close();
		Require(!r.isOpen() && r.size() == 0 && r.getPos() == 0 && !r.isEOF(), "Closed reader state mismatch");
		Require(r.open(file.path) && r.getPos() == 0, "Reopen failed");
		ReadFull(r, buffer.data(), 16);
		Equal(buffer.data(), file.bytes, 0, 16);
	}

	void VerifySparse(const fs::path& root, bool available)
	{
		if (!available) { std::cerr << "UNVERIFIED: sparse >4GiB offsets\n"; return; }
		Reader r{ EnginePath(Output(root, "fixtures/sparse.bin")) };
		Require(r.isOpen() && r.size() == SparseSize, "Sparse open/size mismatch");
		std::array<uint8_t, 128> buffer{};
		for (const I64 at : { I64{ 0 }, FourGiB - 65536, FourGiB - 33, FourGiB - 1, FourGiB, FourGiB + 17, SparseSize - 19, SparseSize })
		{
			Seek(r, FourGiB + 7);
			const auto count = std::min<I64>(buffer.size(), SparseSize - at);
			Require(r.lookahead(buffer.data(), at, buffer.size()) == count && r.getPos() == FourGiB + 7, "Sparse lookahead restore mismatch");
			for (I64 i = 0; i < count; ++i) Require(buffer[i] == SparseByte(at + i), "Sparse lookahead byte mismatch");
			Require(r.read(buffer.data(), at, buffer.size()) == count && r.getPos() == at + count, "Sparse read/position mismatch");
			for (I64 i = 0; i < count; ++i) Require(buffer[i] == SparseByte(at + i), "Sparse read byte mismatch");
		}
		Require(r.isEOF(), "Sparse EOF mismatch");
		Seek(r, SparseSize - 3);
		Require(!r.readExact(buffer.data(), 4) && r.getPos() == SparseSize - 3, "Sparse readExact failure mismatch");
		Require(r.skip(-65536) == FourGiB, "Sparse backward skip mismatch");
	}

	struct Data
	{
		Config config;
		File big, records;
		std::vector<File> files;
		s3d::String font;
		s3d::Blob decoded;
	};

	Data Verify(const fs::path& root)
	{
		std::cerr << "Verifying all fixture/asset bytes and reader state transitions...\n";
		Data d;
		d.config = ReadConfig(root);
		d.big = Model(Output(root, "fixtures/big.bin"), Pattern(64 * MiB, 1));
		d.records = Model(Output(root, "fixtures/records.bin"), Records());
		VerifyFile(d.big); VerifyFile(d.records);
		VerifyFile(Model(Output(root, "fixtures/empty.bin"), {}));
		const auto dialogue = MakeDialogue();
		s3d::String line;
		line.reserve(64);
		for (const bool utf16 : { false, true })
		{
			const auto file = Model(Output(root, utf16 ? "fixtures/dialogue-utf16.txt" : "fixtures/dialogue-utf8.txt"), utf16 ? dialogue.utf16 : dialogue.utf8);
			VerifyFile(file);
			(void)ReadDialogue<true>(file.path, dialogue, line);
		}
		VerifyTrace(d.records);
		Reader missing;
		Require(!missing.open(EnginePath(Output(root, "fixtures/small-files/absent.bin"))), "Missing-file open unexpectedly succeeded");
		Require(missing.open(d.records.path), "Reopen after failed open failed");
		for (size_t i = 0; i < 128; ++i)
			d.files.push_back(Model(Output(root, "fixtures/small-files/" + std::to_string(i) + ".bin"), Pattern((1 + i * 37 % 64) * KiB, 1000 + i), "small"));
		for (const auto& a : d.config.assets)
		{
			const auto path = d.config.repo / fs::u8path(a.relative);
			auto bytes = Load(path);
			Require(bytes.size() == a.size && Hash(bytes.data(), bytes.size()) == a.hash, "Repository asset changed since --prepare");
			d.files.push_back(Model(path, std::move(bytes), a.group));
		}
		for (const auto& file : d.files) VerifyFile(file);
		VerifySparse(root, d.config.sparse);
		if (!d.config.font.empty())
		{
			const auto path = d.config.repo / fs::u8path(d.config.font);
			const auto compressed = Load(path);
			Require(s3d::Compression::Decompress(compressed.data(), compressed.size(), d.decoded), "In-memory reference decode failed");
			Require(d.decoded.size() == d.config.decodedSize && Hash(d.decoded.data(), d.decoded.size()) == d.config.decodedHash, "Complete decoded reference hash mismatch");
			d.font = EnginePath(path);
			s3d::Blob actual;
			Require(s3d::Compression::DecompressFile(d.font, actual), "DecompressFile failed");
			Require(actual.size() == d.decoded.size() && Hash(actual.data(), actual.size()) == d.config.decodedHash
				&& std::memcmp(actual.data(), d.decoded.data(), actual.size()) == 0, "DecompressFile complete decoded content mismatch");
		}
		else std::cerr << "UNVERIFIED: compressed font decompression (asset unavailable)\n";
		std::cerr << "Verification passed. UNVERIFIED: embedded Windows resources (no resource bundle linked)\n";
		return d;
	}

	class Measurements
	{
		std::ofstream csv;
		std::string filter;
		size_t rows = 0;
	public:
		Measurements(const fs::path& path, std::string selected) : csv(path, std::ios::binary | std::ios::trunc), filter(std::move(selected))
		{
			Require(csv.is_open(), "Cannot open CSV");
			csv.imbue(std::locale::classic());
			csv << "scenario,iterations,bytes_per_iteration,seconds,checksum\n" << std::setprecision(10);
		}
		template <class Function> void Run(const char* name, uint64_t bytes, Function function)
		{
			if (!filter.empty() && std::string(name).find(filter) == std::string::npos) return;
			std::cerr << "Measuring " << name << "...\n";
			const uint64_t expected = function.template operator()<true>(); // One fully checked warmup.
			std::array<uint64_t, 256> checksums{};
			const auto batch = [&](size_t iterations)
			{
				const auto start = std::chrono::steady_clock::now();
				for (size_t i = 0; i < iterations; ++i) checksums[i] = function.template operator()<false>();
				const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
				for (size_t i = 0; i < iterations; ++i) Require(checksums[i] == expected, "Timed checksum differs from validated warmup");
				return seconds;
			};
			size_t calibration = 1;
			double seconds = batch(calibration);
			while (seconds < 0.01 && calibration < 256) { calibration *= 2; seconds = batch(calibration); }
			const size_t iterations = static_cast<size_t>(std::clamp(std::ceil(0.1 * calibration / std::max(seconds, 1e-9)), 3.0, 256.0));
			seconds = batch(iterations);
			csv << name << ',' << iterations << ',' << bytes << ',' << seconds << ',' << expected << '\n';
			csv.flush();
			Require(bool(csv), "CSV write failed");
			++rows;
		}
		void Finish() { Require(rows != 0, "Scenario filter matched no available scenarios"); csv.close(); Require(bool(csv), "CSV close failed"); }
	};

	struct Request { I64 pos; size_t size; };
	template <bool Positioned>
	void ReadScenario(Measurements& measurements, const char* name, const File& file, const std::vector<Request>& trace)
	{
		Reader r{ file.path };
		Require(r.isOpen(), "Scenario open failed");
		Bytes buffer(MiB);
		uint64_t bytes = 0;
		for (const auto& q : trace) bytes += q.size;
		measurements.Run(name, bytes, [&]<bool Check>()
		{
			Seek(r, 0);
			uint64_t sum = 0;
			for (const auto& q : trace)
			{
				if constexpr (Positioned) Seek(r, q.pos);
				ReadFull(r, buffer.data(), q.size);
				if constexpr (Check) { Equal(buffer.data(), file.bytes, q.pos, q.size); Require(r.getPos() == q.pos + static_cast<I64>(q.size), "Trace position mismatch"); }
				sum += Sample(buffer.data(), q.size);
			}
			return sum;
		});
	}

	void Measure(const fs::path& root, const fs::path& csvPath, const std::string& filter)
	{
		const auto path = Output(root, csvPath);
		Require(!Within(Output(root, "fixtures"), path), "CSV must be outside fixtures directory");
		auto data = Verify(root); // Verification and model/trace/destination construction are never timed.
		fs::create_directories(path.parent_path());
		Measurements measurements(path, filter);
		for (const size_t block : { MiB, 4 * KiB })
		{
			std::vector<Request> trace;
			for (size_t pos = 0; pos < data.big.bytes.size(); pos += block) trace.push_back({ static_cast<I64>(pos), block });
			ReadScenario<false>(measurements, block == MiB ? "sequential_1m" : "stream_4k", data.big, trace);
		}
		Reader records{ data.records.path };
		Require(records.isOpen(), "Records open failed");
		measurements.Run("scalar_4b", MiB, [&]<bool Check>()
		{
			Seek(records, 0);
			uint64_t sum = 0;
			uint32_t value = 0;
			for (size_t pos = 0; pos < MiB; pos += 4)
			{
				Require(records.read(value), "Short scalar read");
				if constexpr (Check) { Equal(&value, data.records.bytes, pos, 4); Require(records.getPos() == static_cast<I64>(pos + 4), "Scalar position mismatch"); }
				sum += value;
			}
			return sum;
		});
		std::vector<I64> recordEnds;
		for (I64 pos = 256; pos <= MiB; pos += 256) recordEnds.push_back(pos);
		std::array<uint8_t, 16> header{}, lookahead{};
		measurements.Run("mixed_records", recordEnds.size() * 32, [&]<bool Check>()
		{
			Seek(records, 0);
			uint64_t sum = 0;
			for (const I64 end : recordEnds)
			{
				Require(records.lookahead(lookahead.data(), 16) == 16, "Short header lookahead");
				if constexpr (Check) { Equal(lookahead.data(), data.records.bytes, end - 256, 16); Require(records.getPos() == end - 256, "Header lookahead moved position"); }
				ReadFull(records, header.data(), 16);
				if constexpr (Check) Equal(header.data(), data.records.bytes, end - 256, 16);
				Require(records.skip(240) == end, "Payload skip failed");
				sum += Sample(header.data(), 16) + Sample(lookahead.data(), 16);
			}
			return sum;
		});
		for (const bool clustered : { false, true })
		{
			std::vector<Request> trace;
			uint64_t seed = 12345;
			size_t base = 0;
			for (size_t i = 0; i < 4096; ++i)
			{
				if (i % 32 == 0) base = (Next(seed) % (data.big.bytes.size() - 64 * KiB)) & ~size_t{ 4095 };
				const size_t pos = clustered ? base + Next(seed) % (60 * KiB + 1) : Next(seed) % (data.big.bytes.size() - 4 * KiB + 1);
				trace.push_back({ static_cast<I64>(pos), 4 * KiB });
			}
			ReadScenario<true>(measurements, clustered ? "clustered_4k" : "random_4k", data.big, trace);
		}
		for (const auto& [group, name] : std::array<std::pair<const char*, const char*>, 4>{ { { "small", "small_files" }, { "shaders", "real_shaders" }, { "png", "real_png" }, { "fonts", "real_fonts" } } })
		{
			std::vector<const File*> files;
			uint64_t bytes = 0;
			size_t maximum = 1;
			for (const auto& file : data.files) if (file.group == group) { files.push_back(&file); bytes += file.bytes.size(); maximum = std::max(maximum, file.bytes.size()); }
			if (files.empty()) { std::cerr << "UNVERIFIED: " << name << " (no assets)\n"; continue; }
			Bytes buffer(maximum);
			measurements.Run(name, bytes, [&]<bool Check>()
			{
				uint64_t sum = 0;
				for (const auto* file : files)
				{
					Reader r{ file->path }; // Deliberately include construction, open, close, and destruction.
					Require(r.isOpen(), "Open/read/close scenario open failed");
					ReadFull(r, buffer.data(), file->bytes.size());
					if constexpr (Check) { Equal(buffer.data(), file->bytes, 0, file->bytes.size()); Require(r.isEOF(), "Asset EOF mismatch"); }
					sum += Sample(buffer.data(), file->bytes.size());
					r.close();
				}
				return sum;
			});
		}
		const auto dialogue = MakeDialogue();
		s3d::String line;
		line.reserve(64);
		for (const bool utf16 : { false, true })
		{
			const auto textPath = EnginePath(Output(root, utf16 ? "fixtures/dialogue-utf16.txt" : "fixtures/dialogue-utf8.txt"));
			measurements.Run(utf16 ? "text_utf16_lines" : "text_utf8_lines", utf16 ? dialogue.utf16.size() : dialogue.utf8.size(),
				[&]<bool Check>() { return ReadDialogue<Check>(textPath, dialogue, line); });
		}
		if (!data.font.empty())
		{
			const auto font = std::find_if(data.files.begin(), data.files.end(), [&](const File& f) { return f.path == data.font; });
			Require(font != data.files.end(), "Font missing from validated assets");
			// Current bundled ZSTD_CStreamInSize() is 131072, also used by DecompressFile.
			std::vector<Request> trace;
			for (size_t pos = 0; pos < font->bytes.size(); pos += 128 * KiB)
				trace.push_back({ static_cast<I64>(pos), std::min(128 * KiB, font->bytes.size() - pos) });
			Bytes buffer(128 * KiB);
			measurements.Run("font_stream_128k", font->bytes.size(), [&]<bool Check>()
			{
				Reader reader{ data.font };
				Require(reader.isOpen(), "Font stream open failed");
				uint64_t sum = 0;
				for (const auto& q : trace)
				{
					ReadFull(reader, buffer.data(), q.size);
					if constexpr (Check) Equal(buffer.data(), font->bytes, q.pos, q.size);
					sum += Sample(buffer.data(), q.size);
				}
				Require(reader.read(buffer.data(), buffer.size()) == 0, "Font stream EOF mismatch");
				return sum;
			});
			s3d::Blob decoded;
			decoded.reserve(data.decoded.size());
			// bytes_per_iteration is compressed input size; API context/scratch allocations remain timed.
			measurements.Run("decompress_font", fs::file_size(data.config.repo / fs::u8path(data.config.font)), [&]<bool Check>()
			{
				decoded.clear(); // DecompressFile appends; clear retains the reserved destination storage.
				Require(s3d::Compression::DecompressFile(data.font, decoded) && decoded.size() == data.decoded.size(), "Timed font decode failed/short");
				if constexpr (Check) Require(Hash(decoded.data(), decoded.size()) == data.config.decodedHash
					&& std::memcmp(decoded.data(), data.decoded.data(), decoded.size()) == 0, "Warmup decoded font mismatch");
				return Sample(decoded.data(), decoded.size());
			});
			Require(decoded.isEmpty() || Hash(decoded.data(), decoded.size()) == data.config.decodedHash, "Post-timing complete decoded hash mismatch");
		}
		measurements.Finish();
		std::cerr << "CSV written: " << UTF8(path) << '\n';
	}

	template <class Char> int Run(int argc, Char** argv)
	{
		try
		{
			Require(argc >= 3, "Usage: benchmark.exe --prepare <out-dir> <repo-root> | --verify <out-dir> | --measure <out-dir> <csv-path> [scenario-filter]");
			const auto mode = fs::path(argv[1]).string();
			const auto root = fs::weakly_canonical(fs::absolute(fs::path(argv[2])));
			if (mode == "--prepare" && argc == 4) Prepare(root, fs::absolute(fs::path(argv[3])));
			else if (mode == "--verify" && argc == 3) (void)Verify(root);
			else if (mode == "--measure" && (argc == 4 || argc == 5)) Measure(root, fs::path(argv[3]), argc == 5 ? fs::path(argv[4]).string() : "");
			else throw std::runtime_error("Invalid command or argument count; use --prepare, --verify, or --measure");
			return 0;
		}
		catch (const std::exception& e) { std::cerr << "ERROR: " << e.what() << '\n'; return 1; }
		catch (...) { std::cerr << "ERROR: unknown exception\n"; return 1; }
	}
}

# ifdef _WIN32
int wmain(int argc, wchar_t** argv) { return Run(argc, argv); }
# else
int main(int argc, char** argv) { return Run(argc, argv); }
# endif
