//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/Array.hpp>
# include <ThirdParty/doctest/doctest.h>
# include <random>

using namespace s3d;

namespace
{
	template <class T>
	void CheckValues(const Array<T>& actual, const std::vector<T>& expected)
	{
		REQUIRE_EQ(actual.size(), expected.size());
		CHECK_GE(actual.capacity(), actual.size());
		CHECK_EQ(actual.get_if(actual.size()), nullptr);
		for (size_t i = 0; i < expected.size(); ++i)
		{
			CHECK_EQ(actual[i], static_cast<T>(expected[i]));
			CHECK_EQ(actual.get_if(i), actual.data() + i);
		}
	}

	template <class T>
	void RunArrayModel()
	{
		// Use raw mt19937 output so the sequence does not depend on distribution implementations.
		for (const uint32_t seed : { 0u, 1u, 0xC0FFEEu, 0xFFFFFFFFu })
		{
			CAPTURE(seed);
			std::mt19937 random(seed);
			Array<T> actual;
			std::vector<T> expected;
			for (int step = 0; step < 1000; ++step)
			{
				const auto operation = random() % 20;
				const T value = static_cast<T>(random() % 7);
				const size_t count = random() % 130;
				const size_t index = random() % (expected.size() + 1);
				CAPTURE(step); CAPTURE(operation); CAPTURE(index); CAPTURE(count);
				switch (operation)
				{
				case 0:
					actual.push_back(value); expected.push_back(value); break;
				case 1:
					actual.push_front(value); expected.insert(expected.begin(), value); break;
				case 2:
					actual.insert(actual.begin() + index, value);
					expected.insert(expected.begin() + index, value); break;
				case 3:
					if (index < expected.size())
					{
						actual.erase_at(index); expected.erase(expected.begin() + index);
					}
					else { CHECK_THROWS_AS(actual.erase_at(index), std::out_of_range); }
					break;
				case 4:
					if (index < expected.size())
					{
						actual.erase_at_unstable(index);
						expected[index] = expected.back(); expected.pop_back();
					}
					else { CHECK_THROWS_AS(actual.erase_at_unstable(index), std::out_of_range); }
					break;
				case 5:
					actual.resize(count, value); expected.resize(count, value); break;
				case 6:
					actual = std::move(actual).drop(count);
					expected.erase(expected.begin(), expected.begin() + std::min(count, expected.size())); break;
				case 7:
					actual = std::move(actual).take(count);
					expected.resize(std::min(count, expected.size())); break;
				case 8:
					actual = std::move(actual).filter([=](T v) { return v != value; });
					std::erase(expected, value); break;
				case 9:
					actual.sort(); std::sort(expected.begin(), expected.end()); break;
				case 10:
					actual.reverse(); std::reverse(expected.begin(), expected.end()); break;
				case 11:
					actual.rotate(index); std::rotate(expected.begin(), expected.begin() + index, expected.end()); break;
				case 12:
					if (count <= expected.size() - index)
					{
						actual = std::move(actual).slice(index, count);
						expected = std::vector<T>(expected.begin() + index, expected.begin() + index + count);
					}
					else { CHECK_THROWS_AS((void)std::move(actual).slice(index, count), std::out_of_range); }
					break;
				case 13:
					actual.clear(); expected.clear(); break;
				case 14:
					actual.release(); expected.clear(); break;
				case 15:
					actual.reserve(count); actual.shrink_to_fit(); break;
				case 16:
					if (expected.size() <= 64)
					{
						actual.append(actual);
						const auto copy = expected; expected.insert(expected.end(), copy.begin(), copy.end());
					}
					break;
				case 17:
					actual.append(std::move(actual)); break;
				case 18:
				{
					Array<T> copy = actual;
					Array<T> moved = std::move(copy);
					actual.swap(moved); break;
				}
				default:
					actual.fill(value); std::fill(expected.begin(), expected.end(), value); break;
				}
				CheckValues(actual, expected);
			}
		}
	}

	template <class T>
	void CheckArrayIdentities()
	{
		for (const size_t size : { 0u, 1u, 2u, 23u, 24u, 65u, 129u })
		{
			CAPTURE(size);
			std::vector<T> expected;
			for (size_t i = 0; i < size; ++i) { expected.push_back(static_cast<T>(i % 3)); }
			const Array<T> source(expected.begin(), expected.end());
			CheckValues(source.reversed().reverse(), expected);
			for (const size_t count : { size_t{ 0 }, size_t{ 1 }, size, size + 1, std::numeric_limits<size_t>::max() })
			{
				CAPTURE(count);
				CheckValues(source.take(count).append(source.drop(count)), expected);
			}
			for (const size_t width : { 1u, 2u, 7u, 130u })
			{
				CheckValues(source.chunk(width).flatten(), expected);
			}
			for (size_t middle = 0; middle <= size; ++middle)
			{
				CheckValues(source.rotated(middle).rotate(size - middle), expected);
			}
			CHECK_THROWS_AS((void)source.slice(size + 1), std::out_of_range);
			CHECK_THROWS_AS((void)source.slice(0, std::numeric_limits<size_t>::max()), std::out_of_range);
			CheckValues(source, expected);
		}
	}
}

TEST_CASE("Array.model.int") { RunArrayModel<int>(); }
TEST_CASE("Array.model.bool") { RunArrayModel<bool>(); }
TEST_CASE("Array.model.identities")
{
	CheckArrayIdentities<int>();
	CheckArrayIdentities<bool>();
}

TEST_CASE("Array.model.bool_storage_boundaries")
{
	const size_t capacity = Array<bool>{}.capacity();
	const std::vector<size_t> sizes{ 0, 1, capacity ? capacity - 1 : 0, capacity, capacity + 1, capacity * 2 + 1, 129 };
	for (const auto from : sizes)
	{
		for (const auto to : sizes)
		{
			CAPTURE(from); CAPTURE(to);
			Array<bool> flags;
			std::vector<bool> expected;
			for (size_t i = 0; i < from; ++i)
			{
				flags.push_back(i % 3 == 1); expected.push_back(i % 3 == 1);
			}
			flags.resize(to, true); expected.resize(to, true);
			CheckValues(flags, expected);
			flags.shrink_to_fit();
			CheckValues(flags, expected);
			Array<bool> moved = std::move(flags);
			CheckValues(moved, expected);
			Array<bool> other(from, false);
			moved.swap(other);
			CheckValues(other, expected);
			CheckValues(moved, std::vector<bool>(from, false));
			flags.clear(); flags.push_back(true); CHECK(flags.front());
			other.release(); CHECK(other.empty());
			other.append(moved); CheckValues(other, std::vector<bool>(from, false));
		}
	}
}
