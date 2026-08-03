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

	struct NonDefaultConstructible
	{
		int32 value;

		NonDefaultConstructible() = delete;

		constexpr explicit NonDefaultConstructible(const int32 value)
			: value{ value } {}
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

	template <class GridType>
	concept HasRows = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).rows();
	};

# if defined(__cpp_lib_ranges_stride)

	template <class GridType>
	concept HasColumn = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).column(0);
	};

	template <class GridType>
	concept HasColumns = requires(GridType&& grid)
	{
		std::forward<GridType>(grid).columns();
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

TEST_CASE("Grid.assign")
{
	Grid<int32> grid = { { 1, 2 }, { 3, 4 } };

	CHECK(&grid.assign(3, 2, 7) == &grid);
	CHECK(grid == Grid<int32>(3, 2, 7));

	CHECK(&grid.assign(Size{ 2, 3 }, 8) == &grid);
	CHECK(grid == Grid<int32>(2, 3, 8));

	CHECK(&grid.assign({ { 1, 2 }, { 3 } }) == &grid);
	CHECK(grid == Grid<int32>{ { 1, 2 }, { 3, 0 } });

	CHECK(&grid.assign({}) == &grid);
	CHECK(grid.size() == Size{ 0, 0 });
	CHECK(grid.empty());
}

TEST_CASE("Grid.Generate")
{
	static_assert(not std::default_initializable<NonDefaultConstructible>);

	int32 value = 0;
	const auto generated = Grid<NonDefaultConstructible>::Generate(Size{ 2, 2 },
		[&value]() { return NonDefaultConstructible{ value++ }; });

	CHECK(generated.size() == Size{ 2, 2 });
	CHECK(generated[0, 0].value == 0);
	CHECK(generated[0, 1].value == 1);
	CHECK(generated[1, 0].value == 2);
	CHECK(generated[1, 1].value == 3);

	const Grid<NonDefaultConstructible> constructed(Size{ 2, 1 },
		Arg::generator = []() { return NonDefaultConstructible{ 42 }; });
	CHECK(constructed.size() == Size{ 2, 1 });
	CHECK(constructed[0, 0].value == 42);
	CHECK(constructed[0, 1].value == 42);

	int32 calls = 0;
	const auto empty = Grid<NonDefaultConstructible>::Generate(Size{ 3, 0 },
		[&calls]() { ++calls; return NonDefaultConstructible{ 0 }; });
	CHECK(empty.size() == Size{ 3, 0 });
	CHECK(empty.empty());
	CHECK(calls == 0);

	const auto invalid = Grid<NonDefaultConstructible>::Generate(Size{ -1, 2 },
		[&calls]() { ++calls; return NonDefaultConstructible{ 0 }; });
	CHECK(invalid.size() == Size{ 0, 0 });
	CHECK(invalid.empty());
	CHECK(calls == 0);
}

TEST_CASE("Grid.IndexedGenerate")
{
	const auto generatedXY = Grid<NonDefaultConstructible>::IndexedGenerate(Size{ 2, 2 },
		[](const int32 x, const int32 y) { return NonDefaultConstructible{ (y * 10 + x) }; });
	CHECK(generatedXY[0, 0].value == 0);
	CHECK(generatedXY[0, 1].value == 1);
	CHECK(generatedXY[1, 0].value == 10);
	CHECK(generatedXY[1, 1].value == 11);

	const auto generatedPoint = Grid<NonDefaultConstructible>::IndexedGenerate(Size{ 2, 2 },
		[](const Point pos) { return NonDefaultConstructible{ (pos.y * 10 + pos.x) }; });
	CHECK(generatedPoint[0, 0].value == 0);
	CHECK(generatedPoint[0, 1].value == 1);
	CHECK(generatedPoint[1, 0].value == 10);
	CHECK(generatedPoint[1, 1].value == 11);

	const Grid<NonDefaultConstructible> constructedXY(Size{ 2, 1 },
		Arg::generator = [](const int32 x, const int32 y) { return NonDefaultConstructible{ (y * 10 + x) }; });
	CHECK(constructedXY[0, 0].value == 0);
	CHECK(constructedXY[0, 1].value == 1);

	const Grid<NonDefaultConstructible> constructedPoint(Size{ 1, 2 },
		Arg::generator = [](const Point pos) { return NonDefaultConstructible{ (pos.y * 10 + pos.x) }; });
	CHECK(constructedPoint[0, 0].value == 0);
	CHECK(constructedPoint[1, 0].value == 10);

	int32 calls = 0;
	const auto empty = Grid<NonDefaultConstructible>::IndexedGenerate(Size{ 0, 3 },
		[&calls](const int32, const int32) { ++calls; return NonDefaultConstructible{ 0 }; });
	CHECK(empty.size() == Size{ 0, 3 });
	CHECK(empty.empty());
	CHECK(calls == 0);

	const auto invalid = Grid<NonDefaultConstructible>::IndexedGenerate(Size{ 2, -1 },
		[&calls](const Point) { ++calls; return NonDefaultConstructible{ 0 }; });
	CHECK(invalid.size() == Size{ 0, 0 });
	CHECK(invalid.empty());
	CHECK(calls == 0);
}

TEST_CASE("Grid.map non-default-constructible result")
{
	const Grid<int32> source = {
		{ 1, 2 },
		{ 3, 4 },
	};

	const auto mapped = source.map([](const int32 value)
		{
			return NonDefaultConstructible{ (value * 2) };
		});

	static_assert(std::same_as<std::remove_cvref_t<decltype(mapped)>, Grid<NonDefaultConstructible>>);
	CHECK(mapped.size() == source.size());
	CHECK(mapped[0, 0].value == 2);
	CHECK(mapped[0, 1].value == 4);
	CHECK(mapped[1, 0].value == 6);
	CHECK(mapped[1, 1].value == 8);
	CHECK(mapped.map(&NonDefaultConstructible::value) == Grid<int32>{ { 2, 4 }, { 6, 8 } });

	int32 calls = 0;
	const auto empty = Grid<int32>{ Size{ 3, 0 } }.map([&calls](const int32)
		{
			++calls;
			return NonDefaultConstructible{ 0 };
		});
	CHECK(empty.size() == Size{ 3, 0 });
	CHECK(empty.empty());
	CHECK(calls == 0);
}

TEST_CASE("Grid.each_index")
{
	Grid<int32> grid = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};

	Array<Point> positions;
	grid.each_index([&positions](const Point pos, int32& value)
		{
			positions.push_back(pos);
			value += (pos.y * 10 + pos.x);
		});

	CHECK(positions == Array<Point>{
		Point{ 0, 0 }, Point{ 1, 0 }, Point{ 2, 0 },
		Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 } });
	CHECK(grid == Grid<int32>{ { 1, 3, 5 }, { 14, 16, 18 } });

	Array<int32> constValues;
	const Grid<int32>& constGrid = grid;
	constGrid.each_index([&constValues](const Point pos, const int32& value)
		{
			constValues.push_back(value + pos.x + pos.y);
		});
	CHECK(constValues == Array<int32>{ 1, 4, 7, 15, 18, 21 });

	int32 calls = 0;
	Grid<int32>{ Size{ 0, 3 } }.each_index([&calls](const Point, int32&) { ++calls; });
	CHECK(calls == 0);

	CHECK_THROWS(grid.each_index([](const Point, int32&) { throw 42; }));
}

TEST_CASE("Grid.map_indexed")
{
	const Grid<int32> source = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};

	Array<Point> positions;
	const auto mapped = source.map_indexed([&positions](const Point pos, const int32& value)
		{
			positions.push_back(pos);
			return NonDefaultConstructible{ (value + pos.x * 10 + pos.y * 100) };
		});

	static_assert(std::same_as<std::remove_cvref_t<decltype(mapped)>, Grid<NonDefaultConstructible>>);
	CHECK(positions == Array<Point>{
		Point{ 0, 0 }, Point{ 1, 0 }, Point{ 2, 0 },
		Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 } });
	CHECK(mapped.map(&NonDefaultConstructible::value) == Grid<int32>{ { 1, 12, 23 }, { 104, 115, 126 } });
	CHECK(source == Grid<int32>{ { 1, 2, 3 }, { 4, 5, 6 } });

	int32 calls = 0;
	const auto empty = Grid<int32>{ Size{ 0, 3 } }.map_indexed(
		[&calls](const Point, const int32) { ++calls; return NonDefaultConstructible{ 0 }; });
	CHECK(empty.size() == Size{ 0, 3 });
	CHECK(empty.empty());
	CHECK(calls == 0);

	Grid<std::unique_ptr<int32>> moveOnly(2, 1);
	moveOnly.each_index([](const Point pos, std::unique_ptr<int32>& value)
		{
			value = std::make_unique<int32>(pos.x + 10);
		});
	const auto mappedMoveOnly = moveOnly.map_indexed(
		[](const Point pos, const std::unique_ptr<int32>& value)
		{
			return NonDefaultConstructible{ (*value + pos.x) };
		});
	CHECK(mappedMoveOnly.map(&NonDefaultConstructible::value) == Grid<int32>{ { 10, 12 } });

	Grid<bool> boolGrid = {
		{ false, false },
		{ false, false },
	};
	boolGrid.each_index([](const Point pos, bool& value)
		{
			value = ((pos.x + pos.y) % 2 == 0);
		});
	CHECK(boolGrid == Grid<bool>{ { true, false }, { false, true } });
	CHECK(boolGrid.map_indexed([](const Point pos, const bool& value)
		{
			return (value ? (pos.x + pos.y * 10) : -1);
		}) == Grid<int32>{ { 0, -1 }, { -1, 11 } });

	CHECK_THROWS(static_cast<void>(source.map_indexed(
		[](const Point, const int32&) -> int32 { throw 42; })));
}

TEST_CASE("Grid.subgrid")
{
	const Grid<int32> source = {
		{ 11, 12, 13, 14 },
		{ 21, 22, 23, 24 },
		{ 31, 32, 33, 34 },
	};

	CHECK(source.subgrid(Point{ 1, 1 }, Size{ 2, 2 }) == Grid<int32>{ { 22, 23 }, { 32, 33 } });
	CHECK(source.subgrid(1, 0, 3, 2) == Grid<int32>{ { 12, 13, 14 }, { 22, 23, 24 } });
	CHECK(source.subgrid(Point{ 0, 0 }, source.size()) == source);

	const auto zeroWidth = source.subgrid(Point{ 4, 1 }, Size{ 0, 2 });
	CHECK(zeroWidth.size() == Size{ 0, 2 });
	CHECK(zeroWidth.empty());

	const auto zeroHeight = source.subgrid(Point{ 2, 3 }, Size{ 2, 0 });
	CHECK(zeroHeight.size() == Size{ 2, 0 });
	CHECK(zeroHeight.empty());

	const auto zeroSize = source.subgrid(Point{ 4, 3 }, Size{ 0, 0 });
	CHECK(zeroSize.size() == Size{ 0, 0 });
	CHECK(zeroSize.empty());

	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ 0, 0 }, Size{ -1, 1 })), std::invalid_argument);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ 0, 0 }, Size{ 1, -1 })), std::invalid_argument);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ -1, 0 }, Size{ 1, 1 })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ 0, -1 }, Size{ 1, 1 })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ 3, 0 }, Size{ 2, 1 })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ 0, 2 }, Size{ 1, 2 })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ 4, 0 }, Size{ 1, 0 })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(source.subgrid(Point{ std::numeric_limits<int32>::max(), 0 }, Size{ 1, 1 })), std::out_of_range);

	const auto nonDefaultSource = Grid<NonDefaultConstructible>::IndexedGenerate(Size{ 3, 2 },
		[](const Point pos) { return NonDefaultConstructible{ (pos.y * 10 + pos.x) }; });
	const auto nonDefaultSubgrid = nonDefaultSource.subgrid(Point{ 1, 0 }, Size{ 2, 2 });
	CHECK(nonDefaultSubgrid.map(&NonDefaultConstructible::value) == Grid<int32>{ { 1, 2 }, { 11, 12 } });
}

TEST_CASE("Grid.region fill")
{
	Grid<int32> grid(4, 3, 0);
	CHECK(&grid.fill(Point{ 1, 1 }, Size{ 2, 2 }, 5) == &grid);
	CHECK(grid == Grid<int32>{ { 0, 0, 0, 0 }, { 0, 5, 5, 0 }, { 0, 5, 5, 0 } });

	grid.fill(Point{ -2, -1 }, Size{ 3, 3 }, 7);
	CHECK(grid == Grid<int32>{ { 7, 0, 0, 0 }, { 7, 5, 5, 0 }, { 0, 5, 5, 0 } });

	grid.fill(Point{ 3, 2 }, Size{ 3, 2 }, 9);
	CHECK(grid == Grid<int32>{ { 7, 0, 0, 0 }, { 7, 5, 5, 0 }, { 0, 5, 5, 9 } });

	const Grid<int32> beforeNoOverlap = grid;
	grid.fill(Point{ 10, 10 }, Size{ 2, 2 }, 1);
	grid.fill(Point{ std::numeric_limits<int32>::min(), std::numeric_limits<int32>::min() }, Size{ 2, 2 }, 1);
	grid.fill(Point{ std::numeric_limits<int32>::max(), std::numeric_limits<int32>::max() }, Size{ 2, 2 }, 1);
	grid.fill(Point{ 0, 0 }, Size{ 0, 2 }, 1);
	grid.fill(Point{ 0, 0 }, Size{ 2, 0 }, 1);
	CHECK(grid == beforeNoOverlap);

	CHECK_THROWS_AS(grid.fill(Point{ 0, 0 }, Size{ -1, 1 }, 1), std::invalid_argument);
	CHECK_THROWS_AS(grid.fill(Point{ 0, 0 }, Size{ 1, -1 }, 1), std::invalid_argument);
}

TEST_CASE("Grid.floodFill")
{
	const Grid<int32> source = {
		{ 1, 1, 0, 1, 1 },
		{ 1, 0, 0, 1, 0 },
		{ 1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0 },
	};

	Grid<int32> filled = source;
	CHECK(filled.floodFill(Point{ 0, 0 }, 9) == 10);
	CHECK(filled == Grid<int32>{
		{ 9, 9, 0, 9, 9 },
		{ 9, 0, 0, 9, 0 },
		{ 9, 9, 9, 9, 0 },
		{ 0, 0, 0, 0, 0 },
	});

	const Grid<int32> diagonal = {
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 },
	};

	Grid<int32> fourConnected = diagonal;
	CHECK(fourConnected.floodFill(Point{ 0, 0 }, 2, GridConnectivity::Four) == 1);
	CHECK(fourConnected == Grid<int32>{ { 2, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } });

	Grid<int32> eightConnected = diagonal;
	CHECK(eightConnected.floodFill(Point{ 0, 0 }, 2, GridConnectivity::Eight) == 3);
	CHECK(eightConnected == Grid<int32>{ { 2, 0, 0 }, { 0, 2, 0 }, { 0, 0, 2 } });

	Grid<int32> noOp = source;
	CHECK(noOp.floodFill(Point{ -1, 0 }, 9) == 0);
	CHECK(noOp.floodFill(Point{ 0, -1 }, 9) == 0);
	CHECK(noOp.floodFill(Point{ noOp.width(), 0 }, 9) == 0);
	CHECK(noOp.floodFill(Point{ 0, noOp.height() }, 9) == 0);
	CHECK(noOp.floodFill(Point{ std::numeric_limits<int32>::min(), 0 }, 9) == 0);
	CHECK(noOp.floodFill(Point{ std::numeric_limits<int32>::max(), 0 }, 9) == 0);
	CHECK(noOp.floodFill(Point{ 0, 0 }, 1) == 0);
	CHECK(noOp == source);
	CHECK(Grid<int32>{}.floodFill(Point{ 0, 0 }, 1) == 0);

	Grid<int32> aliased = { { 1, 1, 2, 1, 1 } };
	CHECK(aliased.floodFill(Point{ 0, 0 }, aliased[0][2]) == 2);
	CHECK(aliased == Grid<int32>{ { 2, 2, 2, 1, 1 } });

	Grid<bool> boolGrid = {
		{ true, true, false },
		{ false, true, false },
	};
	CHECK(boolGrid.floodFill(Point{ 0, 0 }, false) == 3);
	CHECK(boolGrid == Grid<bool>(3, 2, false));

	Grid<int32> large(512, 512, 1);
	CHECK(large.floodFill(Point{ 256, 256 }, 2) == (512 * 512));
	CHECK(large.count(2) == (512 * 512));
}

TEST_CASE("Grid.paste")
{
	const Grid<int32> source = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};

	Grid<int32> clippedRight(4, 3, 0);
	CHECK(&clippedRight.paste(Point{ 2, 1 }, source) == &clippedRight);
	CHECK(clippedRight == Grid<int32>{ { 0, 0, 0, 0 }, { 0, 0, 1, 2 }, { 0, 0, 4, 5 } });

	Grid<int32> clippedTopLeft(3, 2, 0);
	clippedTopLeft.paste(Point{ -1, -1 }, source);
	CHECK(clippedTopLeft == Grid<int32>{ { 5, 6, 0 }, { 0, 0, 0 } });

	const Grid<int32> beforeNoOverlap = clippedTopLeft;
	clippedTopLeft.paste(Point{ 10, 10 }, source);
	clippedTopLeft.paste(Point{ std::numeric_limits<int32>::min(), 0 }, source);
	clippedTopLeft.paste(Point{ std::numeric_limits<int32>::max(), 0 }, source);
	clippedTopLeft.paste(Point{ 0, 0 }, Grid<int32>{ Size{ 3, 0 } });
	CHECK(clippedTopLeft == beforeNoOverlap);

	Grid<int32> self = source;
	CHECK(&self.paste(Point{ 1, 1 }, self) == &self);
	CHECK(self == source);

	Grid<bool> boolGrid(2, 2, false);
	boolGrid.paste(Point{ 0, 0 }, Grid<bool>{ { true, false }, { false, true } });
	CHECK(boolGrid == Grid<bool>{ { true, false }, { false, true } });
}

TEST_CASE("Grid.Rect region operations")
{
	Grid<int32> grid = {
		{ 1, 2, 3, 4 },
		{ 5, 6, 7, 8 },
		{ 9, 10, 11, 12 },
	};

	CHECK(grid.bounds() == Rect{ 0, 0, 4, 3 });
	CHECK(Grid<int32>{ Size{ 0, 3 } }.bounds() == Rect{ 0, 0, 0, 3 });
	CHECK(grid.subgrid(Rect{ 1, 1, 2, 2 }) == Grid<int32>{ { 6, 7 }, { 10, 11 } });
	CHECK_THROWS_AS(static_cast<void>(grid.subgrid(Rect{ 0, 0, -1, 1 })), std::invalid_argument);

	CHECK(&grid.fill(Rect{ -1, 1, 3, 3 }, 20) == &grid);
	CHECK(grid == Grid<int32>{ { 1, 2, 3, 4 }, { 20, 20, 7, 8 }, { 20, 20, 11, 12 } });
	CHECK_THROWS_AS(grid.fill(Rect{ 0, 0, 1, -1 }, 0), std::invalid_argument);
}

TEST_CASE("Grid.wrappedAt and clampedAt")
{
	Grid<int32> grid = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};
	const Grid<int32>& constGrid = grid;

	static_assert(std::same_as<decltype(grid.wrappedAt(Point{})), int32&>);
	static_assert(std::same_as<decltype(constGrid.wrappedAt(Point{})), const int32&>);
	static_assert(std::same_as<decltype(std::move(grid).wrappedAt(Point{})), int32>);
	static_assert(std::same_as<decltype(grid.clampedAt(Point{})), int32&>);
	static_assert(std::same_as<decltype(constGrid.clampedAt(Point{})), const int32&>);
	static_assert(std::same_as<decltype(std::move(grid).clampedAt(Point{})), int32>);

	CHECK(constGrid.wrappedAt(Point{ 0, 0 }) == 1);
	CHECK(constGrid.wrappedAt(Point{ 3, 2 }) == 1);
	CHECK(constGrid.wrappedAt(Point{ -1, -1 }) == 6);
	CHECK(constGrid.wrappedAt(Point{ INT32_MIN, INT32_MAX }) == 5);
	grid.wrappedAt(Point{ -1, 0 }) = 30;
	CHECK(grid[0][2] == 30);

	CHECK(constGrid.clampedAt(Point{ 1, 1 }) == 5);
	CHECK(constGrid.clampedAt(Point{ -1, -1 }) == 1);
	CHECK(constGrid.clampedAt(Point{ 4, -1 }) == 30);
	CHECK(constGrid.clampedAt(Point{ -1, 4 }) == 4);
	CHECK(constGrid.clampedAt(Point{ 4, 4 }) == 6);
	CHECK(constGrid.clampedAt(Point{ INT32_MIN, INT32_MAX }) == 4);
	grid.clampedAt(Point{ INT32_MAX, INT32_MIN }) = 300;
	CHECK(grid[0][2] == 300);

	CHECK_THROWS_AS(static_cast<void>(Grid<int32>{}.wrappedAt(Point{})), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(Grid<int32>{ Size{ 0, 3 } }.wrappedAt(Point{})), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(Grid<int32>{ Size{ 3, 0 } }.wrappedAt(Point{})), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(Grid<int32>{}.clampedAt(Point{})), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(Grid<int32>{ Size{ 0, 3 } }.clampedAt(Point{})), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(Grid<int32>{ Size{ 3, 0 } }.clampedAt(Point{})), std::out_of_range);

	Grid<std::unique_ptr<int32>> moveGrid = Grid<std::unique_ptr<int32>>::IndexedGenerate(Size{ 2, 1 },
		[](const Point pos) { return std::make_unique<int32>(pos.x + 1); });
	auto moved = std::move(moveGrid).wrappedAt(Point{ -1, 0 });
	REQUIRE(moved);
	CHECK(*moved == 2);

	Grid<std::unique_ptr<int32>> clampedMoveGrid = Grid<std::unique_ptr<int32>>::IndexedGenerate(Size{ 2, 1 },
		[](const Point pos) { return std::make_unique<int32>(pos.x + 1); });
	auto clampedMoved = std::move(clampedMoveGrid).clampedAt(Point{ 10, 0 });
	REQUIRE(clampedMoved);
	CHECK(*clampedMoved == 2);
}

TEST_CASE("Grid.values_at")
{
	const Grid<int32> grid = {
		{ 11, 12, 13 },
		{ 21, 22, 23 },
	};

	CHECK(grid.values_at({ Point{ 2, 1 }, Point{ 0, 0 }, Point{ 1, 1 } }) == Array<int32>{ 23, 11, 22 });
	CHECK(grid.values_at({}).empty());
	CHECK_THROWS_AS(static_cast<void>(grid.values_at({ Point{ -1, 0 } })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(grid.values_at({ Point{ 0, -1 } })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(grid.values_at({ Point{ 3, 0 } })), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(grid.values_at({ Point{ 0, 2 } })), std::out_of_range);
}

TEST_CASE("Grid.each_neighbor4")
{
	Grid<int32> grid = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 9 },
	};
	Array<Point> positions;
	Array<int32> values;

	grid.each_neighbor4(Point{ 1, 1 }, [&positions, &values](const Point pos, int32& value)
	{
		positions.push_back(pos);
		values.push_back(value);
		value *= 10;
	});
	CHECK(positions == Array<Point>{ Point{ 1, 0 }, Point{ 0, 1 }, Point{ 2, 1 }, Point{ 1, 2 } });
	CHECK(values == Array<int32>{ 2, 4, 6, 8 });
	CHECK(grid == Grid<int32>{ { 1, 20, 3 }, { 40, 5, 60 }, { 7, 80, 9 } });

	positions.clear();
	values.clear();
	const Grid<int32>& constGrid = grid;
	constGrid.each_neighbor4(Point{ 0, 0 }, [&positions, &values](const Point pos, const int32& value)
	{
		positions.push_back(pos);
		values.push_back(value);
	});
	CHECK(positions == Array<Point>{ Point{ 1, 0 }, Point{ 0, 1 } });
	CHECK(values == Array<int32>{ 20, 40 });

	positions.clear();
	constGrid.each_neighbor4(Point{ 1, 0 }, [&positions](const Point pos, const int32&)
	{
		positions.push_back(pos);
	});
	CHECK(positions == Array<Point>{ Point{ 0, 0 }, Point{ 2, 0 }, Point{ 1, 1 } });

	CHECK_THROWS(constGrid.each_neighbor4(Point{ 1, 1 }, [](const Point, const int32&) { throw 42; }));

	Grid<bool> boolGrid(3, 3, false);
	boolGrid.each_neighbor4(Point{ 1, 1 }, [](const Point, bool& value) { value = true; });
	CHECK(boolGrid.count(true) == 4);
}

TEST_CASE("Grid.each_neighbor8")
{
	Grid<int32> grid = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 9 },
	};
	Array<Point> positions;
	Array<int32> values;

	grid.each_neighbor8(Point{ 1, 1 }, [&positions, &values](const Point pos, int32& value)
	{
		positions.push_back(pos);
		values.push_back(value);
		value *= 10;
	});
	CHECK(positions == Array<Point>{ Point{ 0, 0 }, Point{ 1, 0 }, Point{ 2, 0 }, Point{ 0, 1 },
		Point{ 2, 1 }, Point{ 0, 2 }, Point{ 1, 2 }, Point{ 2, 2 } });
	CHECK(values == Array<int32>{ 1, 2, 3, 4, 6, 7, 8, 9 });
	CHECK(grid == Grid<int32>{ { 10, 20, 30 }, { 40, 5, 60 }, { 70, 80, 90 } });

	positions.clear();
	values.clear();
	const Grid<int32>& constGrid = grid;
	constGrid.each_neighbor8(Point{ 0, 0 }, [&positions, &values](const Point pos, const int32& value)
	{
		positions.push_back(pos);
		values.push_back(value);
	});
	CHECK(positions == Array<Point>{ Point{ 1, 0 }, Point{ 0, 1 }, Point{ 1, 1 } });
	CHECK(values == Array<int32>{ 20, 40, 5 });

	positions.clear();
	constGrid.each_neighbor8(Point{ 1, 0 }, [&positions](const Point pos, const int32&)
	{
		positions.push_back(pos);
	});
	CHECK(positions == Array<Point>{ Point{ 0, 0 }, Point{ 2, 0 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 } });

	CHECK_THROWS(constGrid.each_neighbor8(Point{ 1, 1 }, [](const Point, const int32&) { throw 42; }));
}

TEST_CASE("Grid.count_neighbors")
{
	const Grid<int32> grid = {
		{ 1, 2, 1 },
		{ 2, 1, 2 },
		{ 1, 2, 1 },
	};

	CHECK(grid.count_neighbors4(Point{ 1, 1 }, 2) == 4);
	CHECK(grid.count_neighbors8(Point{ 1, 1 }, 2) == 4);
	CHECK(grid.count_neighbors4(Point{ 0, 0 }, 2) == 2);
	CHECK(grid.count_neighbors8(Point{ 0, 0 }, 1) == 1);
	CHECK(grid.count_neighbors4_if(Point{ 1, 1 }, [](const int32 value) { return ((value % 2) == 0); }) == 4);
	CHECK(grid.count_neighbors8_if(Point{ 1, 1 }, [](const int32 value) { return ((value % 2) == 0); }) == 4);
	CHECK(grid.count_neighbors4_if(Point{ 0, 0 }, [](const int32 value) { return (value == 2); }) == 2);
	CHECK(grid.count_neighbors8_if(Point{ 0, 0 }, [](const int32 value) { return (value == 1); }) == 1);

	const Grid<bool> boolGrid = {
		{ true, false, true },
		{ false, true, false },
		{ true, false, true },
	};
	CHECK(boolGrid.count_neighbors4(Point{ 1, 1 }, false) == 4);
	CHECK(boolGrid.count_neighbors8(Point{ 0, 0 }, true) == 1);
	CHECK(boolGrid.count_neighbors8_if(Point{ 1, 1 }, Identity) == 4);

	const Grid<int32> single = { { 42 } };
	isize calls = 0;
	single.each_neighbor8(Point{ 0, 0 }, [&calls](const Point, const int32&) { ++calls; });
	CHECK(calls == 0);
	CHECK(single.count_neighbors4(Point{ 0, 0 }, 42) == 0);
	CHECK(single.count_neighbors8_if(Point{ 0, 0 }, [](const int32) { return true; }) == 0);
}

TEST_CASE("Grid.shift and shifted")
{
	const Grid<int32> source = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 9 },
	};

	Grid<int32> downRight = source;
	const auto* const oldData = downRight.data();
	const size_t oldCapacity = downRight.capacity();
	CHECK(&downRight.shift(1, 1, 0) == &downRight);
	CHECK(downRight == Grid<int32>{ { 0, 0, 0 }, { 0, 1, 2 }, { 0, 4, 5 } });
	CHECK(downRight.data() == oldData);
	CHECK(downRight.capacity() == oldCapacity);

	Grid<int32> upLeft = source;
	upLeft.shift(-1, -1, 0);
	CHECK(upLeft == Grid<int32>{ { 5, 6, 0 }, { 8, 9, 0 }, { 0, 0, 0 } });

	Grid<int32> mixed = source;
	mixed.shift(1, -1, -1);
	CHECK(mixed == Grid<int32>{ { -1, 4, 5 }, { -1, 7, 8 }, { -1, -1, -1 } });

	Grid<int32> zero = source;
	zero.shift(0, 0, -1);
	CHECK(zero == source);

	Grid<int32> fullX = source;
	fullX.shift(3, 0, -1);
	CHECK(fullX == Grid<int32>(3, 3, -1));
	Grid<int32> fullY = source;
	fullY.shift(0, -3, -2);
	CHECK(fullY == Grid<int32>(3, 3, -2));
	Grid<int32> minX = source;
	minX.shift(std::numeric_limits<int32>::min(), 0, -3);
	CHECK(minX == Grid<int32>(3, 3, -3));
	Grid<int32> maxY = source;
	maxY.shift(0, std::numeric_limits<int32>::max(), -4);
	CHECK(maxY == Grid<int32>(3, 3, -4));

	Grid<int32> aliasLeft = { { 1, 2, 3 } };
	aliasLeft.shift(-1, 0, aliasLeft[0][0]);
	CHECK(aliasLeft == Grid<int32>{ { 2, 3, 1 } });
	Grid<int32> aliasRight = { { 1, 2, 3 } };
	aliasRight.shift(1, 0, aliasRight[0][2]);
	CHECK(aliasRight == Grid<int32>{ { 3, 1, 2 } });
	Grid<int32> aliasFull = { { 1, 2, 3 } };
	aliasFull.shift(std::numeric_limits<int32>::min(), 0, aliasFull[0][1]);
	CHECK(aliasFull == Grid<int32>{ { 2, 2, 2 } });

	CHECK(source.shifted(-1, 1, 0) == Grid<int32>{ { 0, 0, 0 }, { 2, 3, 0 }, { 5, 6, 0 } });
	CHECK(source == Grid<int32>{ { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } });
	Grid<int32> rvalueSource = source;
	CHECK(std::move(rvalueSource).shifted(0, 1, 0) == Grid<int32>{ { 0, 0, 0 }, { 1, 2, 3 }, { 4, 5, 6 } });
	Grid<int32> rvalueShift = source;
	CHECK(std::move(rvalueShift).shift(-1, 0, 0) == Grid<int32>{ { 2, 3, 0 }, { 5, 6, 0 }, { 8, 9, 0 } });

	Grid<int32> emptyWidth{ Size{ 0, 3 } };
	emptyWidth.shift(std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max(), 0);
	CHECK(emptyWidth.size() == Size{ 0, 3 });
	Grid<int32> emptyHeight{ Size{ 3, 0 } };
	emptyHeight.shift(std::numeric_limits<int32>::max(), std::numeric_limits<int32>::min(), 0);
	CHECK(emptyHeight.size() == Size{ 3, 0 });

	Grid<bool> boolGrid = { { true, false }, { false, true } };
	boolGrid.shift(1, 0, true);
	CHECK(boolGrid == Grid<bool>{ { true, true }, { true, false } });

	for (int32 dy = -4; dy <= 4; ++dy)
	{
		for (int32 dx = -4; dx <= 4; ++dx)
		{
			Grid<int32> actual = source;
			actual.shift(dx, dy, 0);
			const auto expected = Grid<int32>::IndexedGenerate(source.size(),
				[&source, dx, dy](const Point pos)
				{
					return source.fetch((pos.y - dy), (pos.x - dx), 0);
				});
			CHECK(actual == expected);
		}
	}
}

TEST_CASE("Grid.scaled")
{
	const Grid<int32> source = {
		{ 1, 2 },
		{ 3, 4 },
	};

	CHECK(source.scaled(1) == source);
	CHECK(source.scaled(2) == Grid<int32>{
		{ 1, 1, 2, 2 },
		{ 1, 1, 2, 2 },
		{ 3, 3, 4, 4 },
		{ 3, 3, 4, 4 },
	});
	CHECK(source == Grid<int32>{ { 1, 2 }, { 3, 4 } });

	CHECK(Grid<int32>{ Size{ 0, 3 } }.scaled(2).size() == Size{ 0, 6 });
	CHECK(Grid<int32>{ Size{ 3, 0 } }.scaled(2).size() == Size{ 6, 0 });
	const Grid<int32> zeroWidth{ Size{ 0, 1 } };
	CHECK(zeroWidth.scaled(std::numeric_limits<int32>::max()).size()
		== Size{ 0, std::numeric_limits<int32>::max() });
	const Grid<int32> zeroHeight{ Size{ 1, 0 } };
	CHECK(zeroHeight.scaled(std::numeric_limits<int32>::max()).size()
		== Size{ std::numeric_limits<int32>::max(), 0 });
	CHECK_THROWS_AS(static_cast<void>(source.scaled(0)), std::invalid_argument);
	CHECK_THROWS_AS(static_cast<void>(source.scaled(-1)), std::invalid_argument);
	CHECK_THROWS_AS(static_cast<void>(source.scaled(std::numeric_limits<int32>::min())), std::invalid_argument);

	const Grid<int32> widthOverflow{ Size{ 2, 1 }, 0 };
	CHECK_THROWS_AS(static_cast<void>(widthOverflow.scaled(std::numeric_limits<int32>::max())), std::length_error);
	const Grid<int32> heightOverflow{ Size{ 1, 2 }, 0 };
	CHECK_THROWS_AS(static_cast<void>(heightOverflow.scaled(std::numeric_limits<int32>::max())), std::length_error);

	const auto nonDefault = Grid<NonDefaultConstructible>::IndexedGenerate(Size{ 2, 1 },
		[](const Point pos) { return NonDefaultConstructible{ (pos.x + 1) }; });
	CHECK(nonDefault.scaled(2).map(&NonDefaultConstructible::value)
		== Grid<int32>{ { 1, 1, 2, 2 }, { 1, 1, 2, 2 } });

	const Grid<bool> boolGrid = { { true, false } };
	CHECK(boolGrid.scaled(2) == Grid<bool>{ { true, true, false, false }, { true, true, false, false } });

	Grid<int32> rvalueSource = source;
	CHECK(std::move(rvalueSource).scaled(1) == source);
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

TEST_CASE("Grid.rows")
{
	static_assert(HasRows<Grid<int32>&>);
	static_assert(HasRows<const Grid<int32>&>);
	static_assert(not HasRows<Grid<int32>>);
	static_assert(not HasRows<const Grid<int32>>);

	Grid<int32> grid = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};

	auto rows = grid.rows();
	CHECK(std::ranges::distance(rows) == 2);
	for (auto row : rows)
	{
		CHECK(row.size() == 3);
		row.front() *= 10;
	}
	CHECK(grid == Grid<int32>{ { 10, 2, 3 }, { 40, 5, 6 } });

	auto copiedRows = rows;
	(*copiedRows.begin())[1] = 20;
	CHECK(grid[0, 1] == 20);

	Array<int32> values;
	for (const auto row : std::as_const(grid).rows())
	{
		values.append(row.begin(), row.end());
	}
	CHECK(values == Array<int32>{ 10, 20, 3, 40, 5, 6 });

	const Grid<int32> noRows(Size{ 3, 0 });
	CHECK(std::ranges::empty(noRows.rows()));

	const Grid<int32> emptyRows(Size{ 0, 3 });
	CHECK(std::ranges::distance(emptyRows.rows()) == 3);
	for (const auto row : emptyRows.rows())
	{
		CHECK(row.empty());
	}
}

# if defined(__cpp_lib_ranges_stride)

TEST_CASE("Grid.columns")
{
	static_assert(HasColumns<Grid<int32>&>);
	static_assert(HasColumns<const Grid<int32>&>);
	static_assert(not HasColumns<Grid<int32>>);
	static_assert(not HasColumns<const Grid<int32>>);

	Grid<int32> grid = {
		{ 3, 6, 9 },
		{ 1, 5, 8 },
		{ 2, 4, 7 },
	};

	auto columns = grid.columns();
	CHECK(std::ranges::distance(columns) == 3);
	for (auto column : columns)
	{
		CHECK(std::ranges::distance(column) == 3);
		std::ranges::sort(column);
	}
	CHECK(grid == Grid<int32>{ { 1, 4, 7 }, { 2, 5, 8 }, { 3, 6, 9 } });

	auto copiedColumns = columns;
	(*copiedColumns.begin()).front() = 10;
	CHECK(grid[0, 0] == 10);

	Array<int32> values;
	for (const auto column : std::as_const(grid).columns())
	{
		values.append(column.begin(), column.end());
	}
	CHECK(values == Array<int32>{ 10, 2, 3, 4, 5, 6, 7, 8, 9 });

	const Grid<int32> noColumns(Size{ 0, 3 });
	CHECK(std::ranges::empty(noColumns.columns()));

	const Grid<int32> emptyColumns(Size{ 3, 0 });
	CHECK(std::ranges::distance(emptyColumns.columns()) == 3);
	for (const auto column : emptyColumns.columns())
	{
		CHECK(std::ranges::empty(column));
	}
}

# endif

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

TEST_CASE("Grid.non-mutating geometric transformations")
{
	const Grid<int32> source = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};

	const Grid<int32> rotated90 = {
		{ 4, 1 },
		{ 5, 2 },
		{ 6, 3 },
	};
	const Grid<int32> rotated180 = {
		{ 6, 5, 4 },
		{ 3, 2, 1 },
	};
	const Grid<int32> rotated270 = {
		{ 3, 6 },
		{ 2, 5 },
		{ 1, 4 },
	};
	const Grid<int32> mirrored = {
		{ 3, 2, 1 },
		{ 6, 5, 4 },
	};
	const Grid<int32> flipped = {
		{ 4, 5, 6 },
		{ 1, 2, 3 },
	};
	const Grid<int32> transposed = {
		{ 1, 4 },
		{ 2, 5 },
		{ 3, 6 },
	};

	CHECK(source.rotated90() == rotated90);
	CHECK(source.rotated180() == rotated180);
	CHECK(source.rotated270() == rotated270);
	CHECK(source.mirrored() == mirrored);
	CHECK(source.flipped() == flipped);
	CHECK(source.transposed() == transposed);
	CHECK(source == Grid<int32>{ { 1, 2, 3 }, { 4, 5, 6 } });
	CHECK(source.rotated90().rotated90().rotated90().rotated90() == source);
	CHECK(source.mirrored().mirrored() == source);
	CHECK(source.transposed().transposed() == source);

	CHECK(Grid<int32>{ source }.rotated90() == rotated90);
	CHECK(Grid<int32>{ source }.rotated180() == rotated180);
	CHECK(Grid<int32>{ source }.rotated270() == rotated270);
	CHECK(Grid<int32>{ source }.mirrored() == mirrored);
	CHECK(Grid<int32>{ source }.flipped() == flipped);
	CHECK(Grid<int32>{ source }.transposed() == transposed);

	const Grid<int32> empty;
	CHECK(empty.rotated90().empty());
	CHECK(empty.rotated180().empty());
	CHECK(empty.rotated270().empty());
	CHECK(empty.mirrored().empty());
	CHECK(empty.flipped().empty());
	CHECK(empty.transposed().empty());

	Grid<std::unique_ptr<int32>> moveOnlyGrid(2, 1);
	moveOnlyGrid[0, 0] = std::make_unique<int32>(10);
	moveOnlyGrid[0, 1] = std::make_unique<int32>(20);
	const int32* const first = moveOnlyGrid[0, 0].get();
	const int32* const second = moveOnlyGrid[0, 1].get();

	const auto movedAndRotated = std::move(moveOnlyGrid).rotated90();
	CHECK(movedAndRotated.size() == Size{ 1, 2 });
	CHECK(movedAndRotated[0, 0].get() == first);
	CHECK(movedAndRotated[1, 0].get() == second);

	const Grid<ThrowingSwappable> horizontal(2, 1);
	CHECK_THROWS(static_cast<void>(horizontal.rotated180()));
	CHECK_THROWS(static_cast<void>(horizontal.mirrored()));

	const Grid<ThrowingSwappable> vertical(1, 2);
	CHECK_THROWS(static_cast<void>(vertical.flipped()));
}
