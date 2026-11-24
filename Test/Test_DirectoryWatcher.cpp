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
# include <chrono>
# include <thread>

TEST_CASE("DirectoryWatcher.empty_and_invalid")
{
	DirectoryWatcher empty;
	CHECK_FALSE(empty.isOpen());
	CHECK(empty.directory().isEmpty());
	CHECK(empty.applicableExtensions().isEmpty());
	Array<FileChange> changes{ { U"old.txt", FileChangeAction::Added } };
	CHECK_FALSE(empty.extractChanges(changes));
	CHECK(changes.isEmpty());
	CHECK_NOTHROW(empty.clearChanges());
	CHECK_FALSE(DirectoryWatcher{ U"" });
	CHECK_FALSE(DirectoryWatcher{ Test::OutputPath(U"directory-watcher/missing/") });
}

# if SIV3D_PLATFORM(MACOS)

namespace
{
	bool WaitForChange(const DirectoryWatcher& watcher, const FilePath& path, const FileChangeAction action)
	{
		const auto deadline = (std::chrono::steady_clock::now() + std::chrono::seconds{ 5 });
		do
		{
			bool found = false;
			for (const auto& change : watcher.extractChanges())
			{
				if (not change.path.ends_with(U'/'))
				{
					CHECK(FileSystem::Extension(change.path) == U"txt");
				}
				if ((change.path == path) && (change.action == action))
				{
					found = true;
				}
			}
			if (found)
			{
				return true;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
		} while (std::chrono::steady_clock::now() < deadline);
		return false;
	}
}

TEST_CASE("DirectoryWatcher.macOS.dispatch_delivery_and_shutdown")
{
	const FilePath root = Test::OutputPath(U"directory-watcher/events/");
	REQUIRE(FileSystem::CreateDirectories(root));
	const FilePath watched = (root + U"日本語.txt");
	const FilePath ignored = (root + U"ignored.bin");
	// Deliberately do not pump a run loop or call System::Update() while waiting.
	for (int32 round = 0; round < 3; ++round)
	{
		CAPTURE(round);
		DirectoryWatcher watcher{ root, { U"txt" } };
		REQUIRE(watcher.isOpen());
		CHECK(watcher.directory() == FileSystem::FullPath(root));
		CHECK(watcher.applicableExtensions() == Array<String>{ U"txt" });
		{
			BinaryFileWriter writer{ watched };
			REQUIRE(writer.isOpen());
		}
		{
			BinaryFileWriter writer{ ignored };
			REQUIRE(writer.isOpen());
		}
		REQUIRE(WaitForChange(watcher, watched, FileChangeAction::Added));
		{
			BinaryFileWriter writer{ watched };
			REQUIRE(writer.isOpen());
			REQUIRE(writer.write(uint32{ 42 }));
		}
		REQUIRE(WaitForChange(watcher, watched, FileChangeAction::Modified));
		REQUIRE(FileSystem::Remove(watched));
		REQUIRE(WaitForChange(watcher, watched, FileChangeAction::Removed));
		REQUIRE(FileSystem::Remove(ignored));
		watcher.clearChanges();
	}
	// Destroy watchers with pending events, then open a fresh watcher on the same path.
	for (int32 round = 0; round < 16; ++round)
	{
		DirectoryWatcher watcher{ root };
		REQUIRE(watcher.isOpen());
		{
			BinaryFileWriter writer{ watched };
			REQUIRE(writer.isOpen());
		}
		REQUIRE(FileSystem::Remove(watched));
	}
}

# endif
