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

TEST_CASE("BinaryReader")
{
	constexpr int64 FileSize = (32768 * sizeof(uint32));
	const FilePath path{ U"../../Test/output/binaryreader/test.bin" };
	{
		CHECK(not FileSystem::Exists(path));
		{
			BinaryWriter writer{ path };
			CHECK(writer.isOpen());

			for (uint32 i = 0; i < (FileSize / sizeof(uint32)); ++i)
			{
				writer.write(i);
			}
		}
	}

	{
		BinaryReader reader{ path };
		CHECK(reader.isOpen());
		CHECK(reader.size() == FileSize);

		CHECK(reader.getPos() == 0);
		CHECK(reader.setPos(0) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.setPos(FileSize) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.setPos(FileSize / 2) == (FileSize / 2));
		CHECK(reader.getPos() == (FileSize / 2));

		CHECK(reader.setPos(FileSize + 1) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.setPos(-1) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.skip(0) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.skip(1) == 1);
		CHECK(reader.getPos() == 1);

		CHECK(reader.skip(-1) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.skip(FileSize) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.skip(-1) == (FileSize - 1));
		CHECK(reader.getPos() == (FileSize - 1));

		CHECK(reader.skip(2) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.skip(-(FileSize + 1)) == 0);
		CHECK(reader.getPos() == 0);

		uint32 u32 = 123456;
		CHECK(reader.read(u32));
		CHECK(u32 == 0);
		CHECK(reader.getPos() == 4);

		CHECK(reader.read(u32));
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 8);
		
		CHECK(reader.read(&u32, 4) == 4);
		CHECK(u32 == 2);
		CHECK(reader.getPos() == 12);

		CHECK(reader.read(&u32, 4, 4) == 4);
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 8);

		CHECK(reader.read(&u32, (FileSize - 8), 4) == 4);
		CHECK(u32 == 32766);

		CHECK(reader.read(&u32, 4) == 4);
		CHECK(u32 == 32767);

		CHECK(reader.read(&u32, 4) == 0);
		CHECK(u32 == 32767);

		CHECK(not reader.read(u32));
		CHECK(u32 == 32767);

		CHECK(reader.read(&u32, 0, 4) == 4);
		CHECK(u32 == 0);

		CHECK(reader.lookahead(u32));
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 4);

		CHECK(reader.lookahead(u32));
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 4);

		CHECK(reader.skip(4) == 8);
		CHECK(reader.getPos() == 8);

		CHECK(reader.lookahead(u32));
		CHECK(u32 == 2);
		CHECK(reader.getPos() == 8);

		CHECK(reader.setPos(FileSize - 4) == (FileSize - 4));
		CHECK(reader.getPos() == (FileSize - 4));

		uint32 u32s[4]{};
		CHECK(not reader.lookahead(u32s));
		CHECK(u32s[0] == 32767);

		CHECK(reader.lookahead(u32s, (FileSize - 16), 16) == 16);
		CHECK(u32s[0] == 32764);
		CHECK(u32s[1] == 32765);
		CHECK(u32s[2] == 32766);
		CHECK(u32s[3] == 32767);

		std::ranges::fill(u32s, 0);
		CHECK(reader.setPos(FileSize - 16) == (FileSize - 16));
		CHECK(reader.read(u32s));
		CHECK(u32s[0] == 32764);
		CHECK(u32s[1] == 32765);
		CHECK(u32s[2] == 32766);
		CHECK(u32s[3] == 32767);
		CHECK(reader.getPos() == FileSize);
	}
}
