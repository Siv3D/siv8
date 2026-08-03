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

	struct ThrowingMove
	{
		ThrowingMove() = default;

		ThrowingMove(const ThrowingMove&) = default;

		ThrowingMove(ThrowingMove&&)
		{
			throw 42;
		}

		ThrowingMove& operator =(const ThrowingMove&) = default;

		ThrowingMove& operator =(ThrowingMove&&) = default;
	};

	struct ThrowingSwappable
	{
		friend void swap(ThrowingSwappable&, ThrowingSwappable&)
		{
			throw 42;
		}
	};

	template <class Type>
	struct StatefulAllocator
	{
		using value_type = Type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::false_type;

		int32 id = 0;

		constexpr StatefulAllocator() noexcept = default;

		template <class Other>
		constexpr StatefulAllocator(const StatefulAllocator<Other>& other) noexcept
			: id{ other.id } {}

		[[nodiscard]]
		Type* allocate(const size_t n)
		{
			return std::allocator<Type>{}.allocate(n);
		}

		void deallocate(Type* const p, const size_t n) noexcept
		{
			std::allocator<Type>{}.deallocate(p, n);
		}

		template <class Other>
		[[nodiscard]]
		constexpr bool operator ==(const StatefulAllocator<Other>& other) const noexcept
		{
			return (id == other.id);
		}
	};

	struct GenericLess
	{
		template <class Type>
		[[nodiscard]]
		constexpr bool operator ()(const Type& lhs, const Type& rhs) const
		{
			return (lhs < rhs);
		}
	};

	struct GenericPredicate
	{
		template <class Type>
		[[nodiscard]]
		constexpr bool operator ()(const Type& value) const
		{
			return static_cast<bool>(value);
		}
	};

	struct GenericEach
	{
		template <class Type>
		constexpr void operator ()(Type&&) const {}
	};

	struct GenericMap
	{
		template <class Type>
		[[nodiscard]]
		constexpr int32 operator ()(const Type&) const
		{
			return 0;
		}
	};

	template <class GridType>
	concept HasRow = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).row(0);
	};

# if defined(__cpp_lib_ranges_stride)

	template <class GridType>
	concept HasColumn = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).column(0);
	};

# endif

	template <class GridType>
	concept HasSortBy = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).sort_by(GenericLess{});
	};

	template <class GridType>
	concept HasSortedBy = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).sorted_by(GenericLess{});
	};

	template <class GridType>
	concept HasStableSortBy = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).stable_sort_by(GenericLess{});
	};

	template <class GridType>
	concept HasStableSortedBy = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).stable_sorted_by(GenericLess{});
	};

	template <class GridType>
	concept HasParallelCountIf = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).parallel_count_if(GenericPredicate{});
	};

	template <class GridType>
	concept HasParallelEach = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).parallel_each(GenericEach{});
	};

	template <class GridType>
	concept HasParallelMap = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).parallel_map(GenericMap{});
	};
}

TEST_CASE("Grid.constructor")
{
	SUBCASE("valid size with Array")
	{
		const Array<int32> data = { 1, 2, 3 };

		const Grid<int32> copied(2, 2, data);
		CHECK(copied == Grid<int32>{ { 1, 2 }, { 3, 0 } });

		const Grid<int32> moved(2, 2, Array<int32>{ 1, 2, 3, 4, 5 });
		CHECK(moved == Grid<int32>{ { 1, 2 }, { 3, 4 } });
	}

	SUBCASE("invalid size with Array")
	{
		const size_t invalidWidth = (static_cast<size_t>(std::numeric_limits<int32>::max()) + 1);
		const Array<int32> data = { 1, 2, 3 };

		const Grid<int32> copied(invalidWidth, 1, data);
		CHECK(copied.size() == Size{ 0, 0 });
		CHECK(copied.empty());

		Grid<int32> moved(invalidWidth, 1, Array<int32>{ 1, 2, 3 });
		CHECK(moved.size() == Size{ 0, 0 });
		CHECK(moved.empty());
	}

	SUBCASE("empty initializer-list")
	{
		const Grid<int32> grid(std::initializer_list<std::initializer_list<int32>>{});
		CHECK(grid.size() == Size{ 0, 0 });
		CHECK(grid.empty());
	}
}

TEST_CASE("Grid.rvalue operator[]")
{
	SUBCASE("Point")
	{
		Grid<std::unique_ptr<int32>> grid(1, 1);
		grid[Point{ 0, 0 }] = std::make_unique<int32>(42);

		auto value = std::move(grid)[Point{ 0, 0 }];
		REQUIRE(value);
		CHECK(*value == 42);
	}

	SUBCASE("y, x")
	{
		Grid<std::unique_ptr<int32>> grid(1, 1);
		grid[0, 0] = std::make_unique<int32>(42);

		auto value = std::move(grid)[0, 0];
		REQUIRE(value);
		CHECK(*value == 42);
	}
}

TEST_CASE("Grid.exception specifications")
{
	using StatefulGrid = Grid<int32, StatefulAllocator<int32>>;

	static_assert(noexcept(std::declval<Grid<int32>&&>().front()));
	static_assert(noexcept(std::declval<Grid<int32>&&>().back()));
	static_assert(not noexcept(std::declval<Grid<ThrowingMove>&&>().front()));
	static_assert(not noexcept(std::declval<Grid<ThrowingMove>&&>().back()));

	static_assert(noexcept(std::declval<Grid<int32>&>().rotate180()));
	static_assert(noexcept(std::declval<Grid<int32>&>().mirror()));
	static_assert(noexcept(std::declval<Grid<int32>&>().flip()));
	static_assert(not noexcept(std::declval<Grid<ThrowingSwappable>&>().rotate180()));
	static_assert(not noexcept(std::declval<Grid<ThrowingSwappable>&>().mirror()));
	static_assert(not noexcept(std::declval<Grid<ThrowingSwappable>&>().flip()));

	static_assert(noexcept(std::declval<Grid<int32>&>().swap(std::declval<Grid<int32>&>())));
	static_assert(not noexcept(std::declval<StatefulGrid&>().swap(std::declval<StatefulGrid&>())));
	static_assert(std::is_nothrow_swappable_v<Grid<int32>>);
	static_assert(not std::is_nothrow_swappable_v<StatefulGrid>);

	SUBCASE("rvalue element access propagates move exceptions")
	{
		Grid<ThrowingMove> frontGrid(1, 1);
		CHECK_THROWS(static_cast<void>(std::move(frontGrid).front()));

		Grid<ThrowingMove> backGrid(1, 1);
		CHECK_THROWS(static_cast<void>(std::move(backGrid).back()));
	}

	SUBCASE("geometric transformations propagate swap exceptions")
	{
		Grid<ThrowingSwappable> rotateGrid(2, 1);
		CHECK_THROWS(rotateGrid.rotate180());

		Grid<ThrowingSwappable> mirrorGrid(2, 1);
		CHECK_THROWS(mirrorGrid.mirror());

		Grid<ThrowingSwappable> flipGrid(1, 2);
		CHECK_THROWS(flipGrid.flip());
	}
}

TEST_CASE("Grid.view lifetime")
{
	static_assert(HasRow<Grid<int32>&>);
	static_assert(HasRow<const Grid<int32>&>);
	static_assert(not HasRow<Grid<int32>>);
	static_assert(not HasRow<const Grid<int32>>);

# if defined(__cpp_lib_ranges_stride)

	static_assert(HasColumn<Grid<int32>&>);
	static_assert(HasColumn<const Grid<int32>&>);
	static_assert(not HasColumn<Grid<int32>>);
	static_assert(not HasColumn<const Grid<int32>>);

# endif

	Grid<int32> grid = {
		{ 1, 2 },
		{ 3, 4 },
	};

	auto secondRow = grid.row(1);
	secondRow[0] = 30;
	CHECK(grid[Point{ 0, 1 }] == 30);

	const Grid<int32>& constGrid = grid;
	CHECK(constGrid.row(0)[1] == 2);

# if defined(__cpp_lib_ranges_stride)

	auto secondColumn = grid.column(1);
	secondColumn.front() = 20;
	CHECK(grid[Point{ 1, 0 }] == 20);

# endif
}

TEST_CASE("Grid.delegated operation constraints")
{
	static_assert(HasSortBy<Grid<int32>&>);
	static_assert(HasSortBy<Grid<int32>>);
	static_assert(HasSortedBy<const Grid<int32>&>);
	static_assert(HasSortedBy<Grid<int32>>);
	static_assert(HasStableSortBy<Grid<int32>&>);
	static_assert(HasStableSortBy<Grid<int32>>);
	static_assert(HasStableSortedBy<const Grid<int32>&>);
	static_assert(HasStableSortedBy<Grid<int32>>);

	static_assert(HasSortBy<Grid<bool>&>);
	static_assert(HasSortBy<Grid<bool>>);
	static_assert(HasSortedBy<const Grid<bool>&>);
	static_assert(HasSortedBy<Grid<bool>>);
	static_assert(HasStableSortBy<Grid<bool>&>);
	static_assert(HasStableSortBy<Grid<bool>>);
	static_assert(HasStableSortedBy<const Grid<bool>&>);
	static_assert(HasStableSortedBy<Grid<bool>>);

	static_assert(HasParallelCountIf<const Grid<int32>&>);
	static_assert(HasParallelEach<Grid<int32>&>);
	static_assert(HasParallelEach<const Grid<int32>&>);
	static_assert(HasParallelMap<const Grid<int32>&>);

	static_assert(not HasParallelCountIf<const Grid<bool>&>);
	static_assert(not HasParallelEach<Grid<bool>&>);
	static_assert(not HasParallelEach<const Grid<bool>&>);
	static_assert(not HasParallelMap<const Grid<bool>&>);

	Grid<int32> grid = { { 3, 1, 2 } };
	grid.sort_by(GenericLess{});
	CHECK(grid == Grid<int32>{ { 1, 2, 3 } });

	const auto stableSorted = grid.stable_sorted_by([](const int32 lhs, const int32 rhs)
		{
			return (rhs < lhs);
		});
	CHECK(stableSorted == Grid<int32>{ { 3, 2, 1 } });

	CHECK(grid.parallel_count_if([](const int32 value) { return (1 < value); }) == 2);
	grid.parallel_each([](int32& value) { ++value; });
	CHECK(grid == Grid<int32>{ { 2, 3, 4 } });
	CHECK(grid.parallel_map([](const int32 value) { return (value * 2); }) == Grid<int32>{ { 4, 6, 8 } });

	Grid<bool> boolGrid = { { true, false, true, false } };
	boolGrid.sort_by(std::greater<>{});
	CHECK(boolGrid == Grid<bool>{ { true, true, false, false } });

	const Grid<bool> constBoolGrid = { { true, false, true, false } };
	CHECK(constBoolGrid.stable_sorted_by(std::less<>{}) == Grid<bool>{ { false, false, true, true } });
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
