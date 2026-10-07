# RemoveContents: moving children to Trash on macOS

This manual check exercises `MoveToTrash::Yes` with a directory alias, nested
contents, and child symbolic links. Automated tests cover permanent deletion;
this check creates retained items in the user's Trash.

## Execution

1. Use a separate macOS Siv3D application built with this revision and paste the
   complete code below into its Main.cpp. Keep this repository's test entry point intact.
2. Run the application. It creates a uniquely named `removecontents-trash-*`
   directory in the application's current directory and displays its absolute path.
3. Press Space once to move the generated target's children to Trash.
4. Inspect the displayed results, the generated directory, and Finder's Trash.
5. After inspection, remove the generated directory and only this run's generated
   Trash items. Each top-level item moved to Trash contains the run's UUID in its name.

## Expected results

- All displayed checks are true. The target directory remains empty, retains its
  identity and permissions, and its alias remains a symbolic link.
- Trash contains the generated file, nested directory, directory link, and broken
  link. The nested directory retains its contained file.
- `outside/keep.txt` remains in the generated directory, with its original content.
  Moving the child directory link does not move its target.
- Calling `RemoveContents` again on the empty target succeeds without moving the
  target itself to Trash.

The fixture is intentionally retained for inspection. No test files are placed in
`Test/data/`, and this program does not empty Trash.

## Complete code

```cpp
# include <Siv3D.hpp>
# include <filesystem>
# include <sys/stat.h>
# include <unistd.h>

void Main()
{
    const String id = UUIDValue::Generate().str();
    const FilePath root = FileSystem::PathAppend(FileSystem::CurrentDirectory(),
        U"removecontents-trash-" + id + U'/');
    const FilePath target = (root + U"target/");
    const FilePath outside = (root + U"outside/");
    const FilePath alias = (root + U"alias");
    const FilePath nested = (target + U"nested-" + id + U'/');
    if (not FileSystem::CreateDirectories(nested)
        || not FileSystem::CreateDirectories(outside))
    {
        throw Error{ U"Could not create fixture directories" };
    }
    for (const FilePath& path : { target + U"file-" + id,
        nested + U"inside.txt", outside + U"keep.txt" })
    {
        BinaryFileWriter writer{ path };
        if (not writer.isOpen() || (writer.write("keep", 4) != 4))
        {
            throw Error{ U"Could not create fixture file" };
        }
    }
    const std::string nativeTarget = Unicode::ToUTF8(target);
    if ((::chmod(nativeTarget.c_str(), 0751) != 0)
        || (::symlink(nativeTarget.c_str(), Unicode::ToUTF8(alias).c_str()) != 0)
        || (::symlink(Unicode::ToUTF8(outside).c_str(),
            Unicode::ToUTF8(target + U"directory-link-" + id).c_str()) != 0)
        || (::symlink("missing-target",
            Unicode::ToUTF8(target + U"broken-link-" + id).c_str()) != 0))
    {
        throw Error{ U"Could not configure fixture permissions or links" };
    }
    struct stat before{};
    if (::stat(nativeTarget.c_str(), &before) != 0)
    {
        throw Error{ U"Could not read target metadata" };
    }
    Print << U"Fixture: " << root;
    Print << U"Press Space once to move the generated children to Trash.";
    bool ran = false;
    while (System::Update())
    {
        if (KeySpace.down() && not ran)
        {
            ran = true;
            const bool removed = FileSystem::RemoveContents(alias, MoveToTrash::Yes);
            struct stat after{};
            const bool metadata = ((::stat(nativeTarget.c_str(), &after) == 0)
                && (before.st_dev == after.st_dev) && (before.st_ino == after.st_ino)
                && ((before.st_mode & 07777) == (after.st_mode & 07777)));
            Print << U"Removal succeeded: " << removed;
            Print << U"Target is empty: " << FileSystem::IsEmptyDirectory(target);
            Print << U"Target identity and permissions retained: " << metadata;
            Print << U"Alias retained: " << std::filesystem::is_symlink(Unicode::ToUTF8(alias));
            Print << U"Outside file retained: " << (FileSystem::FileSize(outside + U"keep.txt") == 4);
            Print << U"Empty target succeeds: " << FileSystem::RemoveContents(target, MoveToTrash::Yes);
        }
    }
}
```
