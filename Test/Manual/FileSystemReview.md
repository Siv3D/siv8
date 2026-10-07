# FileSystem behavior and timing review

This diagnostic records existing behavior before changing path semantics or the
native filesystem boundary. It is not a specification: differences and surprising
results should be reviewed, not automatically adopted as regression expectations.

## Execution

1. Build a separate macOS or Windows Siv3D application with the revision being
   evaluated. Paste the complete code below into that application's Main.cpp.
   Keep this repository's platform Main.cpp unchanged.
2. Run it once. The program creates a uniquely named `filesystem-review-*`
   directory in the application's current directory and displays its path.
3. Read `report.tsv` in that directory. For performance comparisons, use Release
   builds, the same filesystem and fixture location, and several independent runs.
4. Remove only the displayed fixture directory when finished. It contains all
   files produced by this run; symbolic links point inside this directory.

## Interpreting results

- Ordinary paths should refer to the generated seven-byte `directory/data.bin`.
- Compare backslash input across existence, size, absolute path, and copy. On
  macOS, the current implementation interprets backslashes differently in these
  operations. The program also creates a literal backslash filename there.
- The link rows compare the spelling of an alias with its target. Link creation
  may be unavailable on Windows; an error is recorded and only those rows are
  omitted. The junction tests in Test_FileSystem.cpp cover Windows separately.
- The missing-CWD case is intentionally excluded because changing/removing the
  process directory would interfere with a running application's other threads.
- Timings are medians of five batches after one warm-up call. They include the
  public API and result construction, but exclude fixture creation and report
  writing. They do not measure cold storage, allocation counts, or system-call
  counts. Debug timings are diagnostic only; no pass/fail threshold is applied.
- Reports are produced only by this explicit diagnostic, not by ordinary tests.

## Complete code

```cpp
# include <Siv3D.hpp>
# include <array>
# include <chrono>
# include <filesystem>
# include <fstream>
# include <stdexcept>

namespace FileSystemReview
{
    std::filesystem::path Native(const FilePathView path)
    {
    # if SIV3D_PLATFORM(WINDOWS)
        return std::filesystem::path{ Unicode::ToWstring(path) };
    # else
        return std::filesystem::path{ Unicode::ToUTF8(path) };
    # endif
    }

    String Run(const FilePath& root)
    {
        if (not std::filesystem::create_directory(Native(root)))
        {
            throw std::runtime_error("The diagnostic requires a new fixture directory");
        }
        std::filesystem::create_directory(Native(root + U"directory/"));
        const auto write = [](const std::filesystem::path& path, const std::string_view data)
        {
            std::ofstream file{ path, std::ios::binary };
            file.write(data.data(), static_cast<std::streamsize>(data.size()));
            file.close();
            if (not file) throw std::runtime_error("Cannot write fixture file");
        };
        const FilePath file = (root + U"directory/data.bin");
        write(Native(file), "1234567");
        for (int32 i = 0; i < 127; ++i)
        {
            write(Native(root + U"directory/entry-" + Format(i)), "1234567");
        }

        String report = U"case\tvalue\n";
        const auto row = [&report](const StringView name, const auto& value)
        {
            report += (name + U"\t" + Format(value) + U"\n");
        };
        row(U"platform", SIV3D_PLATFORM_NAME);
        row(U"version", SIV3D_VERSION_STRING);
    # ifdef NDEBUG
        row(U"build", U"Release");
    # else
        row(U"build", U"Debug");
    # endif
        row(U"root", root);
        row(U"ordinary.exists", FileSystem::Exists(file));
        row(U"ordinary.file_size", FileSystem::FileSize(file));
        row(U"ordinary.full_path", FileSystem::FullPath(file));
        row(U"missing.full_path", FileSystem::FullPath(root + U"missing/child"));
        row(U"append.absolute_rhs", FileSystem::PathAppend(root, U"/other"));
        row(U"volume.empty", FileSystem::VolumePath(U""));

        const FilePath backslash = (root + U"directory\\data.bin");
        row(U"backslash.exists", FileSystem::Exists(backslash));
        row(U"backslash.file_size", FileSystem::FileSize(backslash));
        row(U"backslash.full_path", FileSystem::FullPath(backslash));
        row(U"backslash.copy", FileSystem::Copy(backslash, root + U"copied.bin"));
    # if !SIV3D_PLATFORM(WINDOWS)
        const FilePath literal = (root + U"literal\\name.bin");
        write(Native(literal), "abc");
        row(U"literal_backslash.native_exists", std::filesystem::exists(Native(literal)));
        row(U"literal_backslash.exists", FileSystem::Exists(literal));
        row(U"literal_backslash.file_size", FileSystem::FileSize(literal));
    # endif

        std::error_code error;
        const FilePath alias = (root + U"alias");
        std::filesystem::create_directory_symlink(Native(root + U"directory"), Native(alias), error);
        row(U"directory_link.error", Unicode::FromUTF8(error.message()));
        if (not error)
        {
            row(U"directory_link.full_path", FileSystem::FullPath(alias));
            row(U"directory_link.relative_path", FileSystem::RelativePath(alias + U"/data.bin", root));
            row(U"root.contents", FileSystem::DirectoryContents(root, Recursive::No).sorted());
        }
        const FilePath fileAlias = (root + U"file-alias");
        std::filesystem::create_symlink(Native(file), Native(fileAlias), error);
        row(U"file_link.error", Unicode::FromUTF8(error.message()));
        if (not error)
        {
            row(U"file_link.file_size", FileSystem::FileSize(fileAlias));
            row(U"file_link.size", FileSystem::Size(fileAlias));
        }

        uint64 checksum = 0;
        const auto measure = [&row, &checksum](const StringView name, const int32 repeats, const auto& operation)
        {
            checksum += operation();
            std::array<double, 5> samples;
            for (double& sample : samples)
            {
                const auto start = std::chrono::steady_clock::now();
                for (int32 i = 0; i < repeats; ++i) checksum += operation();
                sample = (std::chrono::duration<double, std::micro>(
                    std::chrono::steady_clock::now() - start).count() / repeats);
            }
            std::sort(samples.begin(), samples.end());
            row(name, samples[2]);
        };
        measure(U"median_us.FileSize", 1000, [&] { return FileSystem::FileSize(file); });
        measure(U"median_us.Size_file", 1000, [&] { return FileSystem::Size(file); });
        measure(U"median_us.FullPath", 1000, [&] { return FileSystem::FullPath(file).size(); });
        measure(U"median_us.RelativePath", 100, [&] { return FileSystem::RelativePath(file, root).size(); });
        measure(U"median_us.DirectoryContents_128", 20, [&]
        {
            return FileSystem::DirectoryContents(root + U"directory/", Recursive::No).size();
        });
        row(U"checksum", checksum);
        return report;
    }
}

void Main()
{
    const FilePath root = (FileSystem::CurrentDirectory()
        + U"filesystem-review-" + UUIDValue::Generate().str() + U'/');
    const String report = FileSystemReview::Run(root);
    std::ofstream output{ FileSystemReview::Native(root + U"report.tsv"), std::ios::binary };
    output << report.toUTF8();
    output.close();
    if (not output) throw Error{ U"Cannot write review report" };
    Print << U"Report and fixture: " << root;
    while (System::Update()) {}
}
```
