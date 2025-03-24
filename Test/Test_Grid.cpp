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

namespace
{
	const Grid<int32> grid4x1_0 = {
		{ 11 },
		{ 21 },
		{ 31 },
		{ 41 },
	};

	const Grid<int32> grid4x1_90 = {
		{ 41, 31, 21, 11 },
	};

	const Grid<int32> grid4x1_180 = {
		{ 41 },
		{ 31 },
		{ 21 },
		{ 11 },
	};

	const Grid<int32> grid4x1_270 = {
		{ 11, 21, 31, 41 },
	};

	const Grid<int32> grid4x1_transposed = {
		{ 11, 21, 31, 41 },
	};


	const Grid<int32> grid3x5_0 = {
		{ 11, 12, 13, 14, 15 },
		{ 21, 22, 23, 24, 25 },
		{ 31, 32, 33, 34, 35 },
	};

	const Grid<int32> grid3x5_90 = {
		{ 31, 21, 11 },
		{ 32, 22, 12 },
		{ 33, 23, 13 },
		{ 34, 24, 14 },
		{ 35, 25, 15 },
	};

	const Grid<int32> grid3x5_180 = {
		{ 35, 34, 33, 32, 31 },
		{ 25, 24, 23, 22, 21 },
		{ 15, 14, 13, 12, 11 },
	};

	const Grid<int32> grid3x5_270 = {
		{ 15, 25, 35 },
		{ 14, 24, 34 },
		{ 13, 23, 33 },
		{ 12, 22, 32 },
		{ 11, 21, 31 },
	};

	const Grid<int32> grid3x5_transposed = {
		{ 11, 21, 31 },
		{ 12, 22, 32 },
		{ 13, 23, 33 },
		{ 14, 24, 34 },
		{ 15, 25, 35 },
	};


	const Grid<int32> grid8x8_0 = {
		{ 11, 12, 13, 14, 15, 16, 17, 18 },
		{ 21, 22, 23, 24, 25, 26, 27, 28 },
		{ 31, 32, 33, 34, 35, 36, 37, 38 },
		{ 41, 42, 43, 44, 45, 46, 47, 48 },
		{ 51, 52, 53, 54, 55, 56, 57, 58 },
		{ 61, 62, 63, 64, 65, 66, 67, 68 },
		{ 71, 72, 73, 74, 75, 76, 77, 78 },
		{ 81, 82, 83, 84, 85, 86, 87, 88 },
	};

	const Grid<int32> grid8x8_90 = {
		{ 81, 71, 61, 51, 41, 31, 21, 11 },
		{ 82, 72, 62, 52, 42, 32, 22, 12 },
		{ 83, 73, 63, 53, 43, 33, 23, 13 },
		{ 84, 74, 64, 54, 44, 34, 24, 14 },
		{ 85, 75, 65, 55, 45, 35, 25, 15 },
		{ 86, 76, 66, 56, 46, 36, 26, 16 },
		{ 87, 77, 67, 57, 47, 37, 27, 17 },
		{ 88, 78, 68, 58, 48, 38, 28, 18 },
	};

	const Grid<int32> grid8x8_180 = {
		{ 88, 87, 86, 85, 84, 83, 82, 81 },
		{ 78, 77, 76, 75, 74, 73, 72, 71 },
		{ 68, 67, 66, 65, 64, 63, 62, 61 },
		{ 58, 57, 56, 55, 54, 53, 52, 51 },
		{ 48, 47, 46, 45, 44, 43, 42, 41 },
		{ 38, 37, 36, 35, 34, 33, 32, 31 },
		{ 28, 27, 26, 25, 24, 23, 22, 21 },
		{ 18, 17, 16, 15, 14, 13, 12, 11 },
	};

	const Grid<int32> grid8x8_270 = {
		{ 18, 28, 38, 48, 58, 68, 78, 88 },
		{ 17, 27, 37, 47, 57, 67, 77, 87 },
		{ 16, 26, 36, 46, 56, 66, 76, 86 },
		{ 15, 25, 35, 45, 55, 65, 75, 85 },
		{ 14, 24, 34, 44, 54, 64, 74, 84 },
		{ 13, 23, 33, 43, 53, 63, 73, 83 },
		{ 12, 22, 32, 42, 52, 62, 72, 82 },
		{ 11, 21, 31, 41, 51, 61, 71, 81 },
	};

	const Grid<int32> grid8x8_transposed = {
		{ 11, 21, 31, 41, 51, 61, 71, 81 },
		{ 12, 22, 32, 42, 52, 62, 72, 82 },
		{ 13, 23, 33, 43, 53, 63, 73, 83 },
		{ 14, 24, 34, 44, 54, 64, 74, 84 },
		{ 15, 25, 35, 45, 55, 65, 75, 85 },
		{ 16, 26, 36, 46, 56, 66, 76, 86 },
		{ 17, 27, 37, 47, 57, 67, 77, 87 },
		{ 18, 28, 38, 48, 58, 68, 78, 88 },
	};


	const Grid<int32> grid9x9_0 = {
		{ 11, 12, 13, 14, 15, 16, 17, 18, 19 },
		{ 21, 22, 23, 24, 25, 26, 27, 28, 29 },
		{ 31, 32, 33, 34, 35, 36, 37, 38, 39 },
		{ 41, 42, 43, 44, 45, 46, 47, 48, 49 },
		{ 51, 52, 53, 54, 55, 56, 57, 58, 59 },
		{ 61, 62, 63, 64, 65, 66, 67, 68, 69 },
		{ 71, 72, 73, 74, 75, 76, 77, 78, 79 },
		{ 81, 82, 83, 84, 85, 86, 87, 88, 89 },
		{ 91, 92, 93, 94, 95, 96, 97, 98, 99 },
	};

	const Grid<int32> grid9x9_90 = {
		{ 91, 81, 71, 61, 51, 41, 31, 21, 11 },
		{ 92, 82, 72, 62, 52, 42, 32, 22, 12 },
		{ 93, 83, 73, 63, 53, 43, 33, 23, 13 },
		{ 94, 84, 74, 64, 54, 44, 34, 24, 14 },
		{ 95, 85, 75, 65, 55, 45, 35, 25, 15 },
		{ 96, 86, 76, 66, 56, 46, 36, 26, 16 },
		{ 97, 87, 77, 67, 57, 47, 37, 27, 17 },
		{ 98, 88, 78, 68, 58, 48, 38, 28, 18 },
		{ 99, 89, 79, 69, 59, 49, 39, 29, 19 },
	};

	const Grid<int32> grid9x9_180 = {
		{ 99, 98, 97, 96, 95, 94, 93, 92, 91 },
		{ 89, 88, 87, 86, 85, 84, 83, 82, 81 },
		{ 79, 78, 77, 76, 75, 74, 73, 72, 71 },
		{ 69, 68, 67, 66, 65, 64, 63, 62, 61 },
		{ 59, 58, 57, 56, 55, 54, 53, 52, 51 },
		{ 49, 48, 47, 46, 45, 44, 43, 42, 41 },
		{ 39, 38, 37, 36, 35, 34, 33, 32, 31 },
		{ 29, 28, 27, 26, 25, 24, 23, 22, 21 },
		{ 19, 18, 17, 16, 15, 14, 13, 12, 11 },
	};

	const Grid<int32> grid9x9_270 = {
		{ 19, 29, 39, 49, 59, 69, 79, 89, 99 },
		{ 18, 28, 38, 48, 58, 68, 78, 88, 98 },
		{ 17, 27, 37, 47, 57, 67, 77, 87, 97 },
		{ 16, 26, 36, 46, 56, 66, 76, 86, 96 },
		{ 15, 25, 35, 45, 55, 65, 75, 85, 95 },
		{ 14, 24, 34, 44, 54, 64, 74, 84, 94 },
		{ 13, 23, 33, 43, 53, 63, 73, 83, 93 },
		{ 12, 22, 32, 42, 52, 62, 72, 82, 92 },
		{ 11, 21, 31, 41, 51, 61, 71, 81, 91 },
	};

	const Grid<int32> grid9x9_transposed = {
		{ 11, 21, 31, 41, 51, 61, 71, 81, 91 },
		{ 12, 22, 32, 42, 52, 62, 72, 82, 92 },
		{ 13, 23, 33, 43, 53, 63, 73, 83, 93 },
		{ 14, 24, 34, 44, 54, 64, 74, 84, 94 },
		{ 15, 25, 35, 45, 55, 65, 75, 85, 95 },
		{ 16, 26, 36, 46, 56, 66, 76, 86, 96 },
		{ 17, 27, 37, 47, 57, 67, 77, 87, 97 },
		{ 18, 28, 38, 48, 58, 68, 78, 88, 98 },
		{ 19, 29, 39, 49, 59, 69, 79, 89, 99 },
	};
}

TEST_CASE("Grid.rotate90")
{
	{
		Grid<int32> grid = grid4x1_0;
		grid.rotate90();
		CHECK(grid == grid4x1_90);
		grid.rotate90();
		CHECK(grid == grid4x1_180);
		grid.rotate90();
		CHECK(grid == grid4x1_270);
		grid.rotate90();
		CHECK(grid == grid4x1_0);
	}

	{
		Grid<int32> grid = grid3x5_0;
		grid.rotate90();
		CHECK(grid == grid3x5_90);
		grid.rotate90();
		CHECK(grid == grid3x5_180);
		grid.rotate90();
		CHECK(grid == grid3x5_270);
		grid.rotate90();
		CHECK(grid == grid3x5_0);
	}

	{
		Grid<int32> grid = grid8x8_0;
		grid.rotate90();
		CHECK(grid == grid8x8_90);
		grid.rotate90();
		CHECK(grid == grid8x8_180);
		grid.rotate90();
		CHECK(grid == grid8x8_270);
		grid.rotate90();
		CHECK(grid == grid8x8_0);
	}

	{
		Grid<int32> grid = grid9x9_0;
		grid.rotate90();
		CHECK(grid == grid9x9_90);
		grid.rotate90();
		CHECK(grid == grid9x9_180);
		grid.rotate90();
		CHECK(grid == grid9x9_270);
		grid.rotate90();
		CHECK(grid == grid9x9_0);
	}
}

TEST_CASE("Grid.rotate180")
{
	{
		Grid<int32> grid = grid4x1_0;
		grid.rotate180();
		CHECK(grid == grid4x1_180);
		grid.rotate180();
		CHECK(grid == grid4x1_0);
	}

	{
		Grid<int32> grid = grid3x5_0;
		grid.rotate180();
		CHECK(grid == grid3x5_180);
		grid.rotate180();
		CHECK(grid == grid3x5_0);
	}

	{
		Grid<int32> grid = grid8x8_0;
		grid.rotate180();
		CHECK(grid == grid8x8_180);
		grid.rotate180();
		CHECK(grid == grid8x8_0);
	}

	{
		Grid<int32> grid = grid9x9_0;
		grid.rotate180();
		CHECK(grid == grid9x9_180);
		grid.rotate180();
		CHECK(grid == grid9x9_0);
	}
}

TEST_CASE("Grid.rotate270")
{
	{
		Grid<int32> grid = grid4x1_0;
		grid.rotate270();
		CHECK(grid == grid4x1_270);
		grid.rotate270();
		CHECK(grid == grid4x1_180);
		grid.rotate270();
		CHECK(grid == grid4x1_90);
		grid.rotate270();
		CHECK(grid == grid4x1_0);
	}

	{
		Grid<int32> grid = grid3x5_0;
		grid.rotate270();
		CHECK(grid == grid3x5_270);
		grid.rotate270();
		CHECK(grid == grid3x5_180);
		grid.rotate270();
		CHECK(grid == grid3x5_90);
		grid.rotate270();
		CHECK(grid == grid3x5_0);
	}

	{
		Grid<int32> grid = grid8x8_0;
		grid.rotate270();
		CHECK(grid == grid8x8_270);
		grid.rotate270();
		CHECK(grid == grid8x8_180);
		grid.rotate270();
		CHECK(grid == grid8x8_90);
		grid.rotate270();
		CHECK(grid == grid8x8_0);
	}

	{
		Grid<int32> grid = grid9x9_0;
		grid.rotate270();
		CHECK(grid == grid9x9_270);
		grid.rotate270();
		CHECK(grid == grid9x9_180);
		grid.rotate270();
		CHECK(grid == grid9x9_90);
		grid.rotate270();
		CHECK(grid == grid9x9_0);
	}
}

TEST_CASE("Grid.transpose")
{
	{
		Grid<int32> grid = grid4x1_0;
		grid.transpose();
		CHECK(grid == grid4x1_transposed);
		grid.transpose();
		CHECK(grid == grid4x1_0);
	}

	{
		Grid<int32> grid = grid3x5_0;
		grid.transpose();
		CHECK(grid == grid3x5_transposed);
		grid.transpose();
		CHECK(grid == grid3x5_0);
	}

	{
		Grid<int32> grid = grid8x8_0;
		grid.transpose();
		CHECK(grid == grid8x8_transposed);
		grid.transpose();
		CHECK(grid == grid8x8_0);
	}

	{
		Grid<int32> grid = grid9x9_0;
		grid.transpose();
		CHECK(grid == grid9x9_transposed);
		grid.transpose();
		CHECK(grid == grid9x9_0);
	}
}