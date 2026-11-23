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
# include <limits>

namespace
{
	struct Vec2KDAdapter : KDTreeVec2Adapter<Array<Vec2>>
	{
		static double GetElement(const dataset_type& points, const uint32 index, const size_t dim)
		{
			return points[index].getPointer()[dim];
		}

		static const double* GetPointer(const Vec2& point)
		{
			return point.getPointer();
		}
	};

	struct Float3KDAdapter : KDTreeFloat3Adapter<Array<Float3>, uint64>
	{
		static float GetElement(const dataset_type& points, const uint64 index, const size_t dim)
		{
			return points[index].getPointer()[dim];
		}

		static const float* GetPointer(const Float3& point)
		{
			return point.getPointer();
		}
	};

	using Vec2Matches = Array<std::pair<uint32, double>>;

	Array<Vec2> MakeKDPoints()
	{
		Array<Vec2> points;
		for (uint32 i = 0; i < 24; ++i)
		{
			points.emplace_back((2.0 * i), (0.25 * (i % 5)));
		}
		return points;
	}

	Vec2Matches ExpectedKDMatches(const Array<Vec2>& points, const Vec2 query,
		const size_t k, const double radiusSq = std::numeric_limits<double>::max())
	{
		Vec2Matches results;
		for (uint32 i = 0; i < points.size(); ++i)
		{
			const double distanceSq = points[i].distanceFromSq(query);
			if (distanceSq < radiusSq)
			{
				results.emplace_back(i, distanceSq);
			}
		}
		std::ranges::sort(results, {}, &Vec2Matches::value_type::second);
		results.resize(std::min(k, results.size()));
		return results;
	}

	Array<uint32> KDIndices(const Vec2Matches& matches)
	{
		Array<uint32> indices;
		for (const auto& [index, distanceSq] : matches)
		{
			indices.push_back(index);
		}
		return indices;
	}
}

TEST_CASE("KDTree.knn_overloads")
{
	const Array<Vec2> points = MakeKDPoints();
	const KDTree<Vec2KDAdapter> tree{ points };
	const Vec2 query{ 17.25, 0.125 };
	for (const size_t k : { 0, 1, 7, 24, 40 })
	{
		CAPTURE(k);
		const Vec2Matches expected = ExpectedKDMatches(points, query, k);
		const Array<uint32> expectedIndices = KDIndices(expected);
		CHECK(tree.knnSearch(k, query) == expectedIndices);

		Array<uint32> indices{ 999 };
		Vec2Matches matches{ { 999, -1.0 } };
		tree.knnSearch(k, query, indices);
		tree.knnSearch(k, query, matches);
		CHECK(indices == expectedIndices);
		CHECK(matches == expected);
	}
}

TEST_CASE("KDTree.rknn_overloads")
{
	const Array<Vec2> points = MakeKDPoints();
	const KDTree<Vec2KDAdapter> tree{ points };
	const Vec2 query{ 17.25, 0.125 };
	for (const double radius : { 0.0, 3.0, 12.0, 100.0 })
	{
		for (const size_t k : { 0, 1, 7, 40 })
		{
			CAPTURE(radius, k);
			const Vec2Matches expected = ExpectedKDMatches(points, query, k, (radius * radius));
			const Array<uint32> expectedIndices = KDIndices(expected);
			CHECK(tree.rknnSearch(radius, k, query) == expectedIndices);

			Array<uint32> indices{ 999 };
			Vec2Matches matches{ { 999, -1.0 } };
			tree.rknnSearch(radius, k, query, indices);
			tree.rknnSearch(radius, k, query, matches);
			CHECK(indices == expectedIndices);
			CHECK(matches == expected);
		}
	}
}

TEST_CASE("KDTree.radius_overloads")
{
	const Array<Vec2> points = MakeKDPoints();
	const KDTree<Vec2KDAdapter> tree{ points };
	const Vec2 query{ 17.25, 0.125 };
	for (const double radius : { 0.0, 3.0, 12.0, 100.0 })
	{
		for (const SortByDistance sortByDistance : { SortByDistance::No, SortByDistance::Yes })
		{
			CAPTURE(radius, sortByDistance.getBool());
			Vec2Matches expected = ExpectedKDMatches(points, query, points.size(), (radius * radius));
			Array<uint32> expectedIndices = KDIndices(expected);
			Array<uint32> returned = tree.radiusSearch(query, radius, sortByDistance);
			Array<uint32> indices{ 999 };
			Vec2Matches matches{ { 999, -1.0 } };
			tree.radiusSearch(query, radius, indices, sortByDistance);
			tree.radiusSearch(query, radius, matches, sortByDistance);
			if (not sortByDistance)
			{
				// Unsorted traversal order is not part of the contract.
				expectedIndices.sort();
				returned.sort();
				indices.sort();
				std::ranges::sort(expected);
				std::ranges::sort(matches);
			}
			CHECK(returned == expectedIndices);
			CHECK(indices == expectedIndices);
			CHECK(matches == expected);
		}
	}
}

TEST_CASE("KDTree.float3_boundary_and_ties")
{
	const Array<Float3> points = {
		{ 0, 0, 0 }, { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 },
		{ 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 }, { 2, 0, 0 },
		{ 3, 0, 0 }, { 4, 0, 0 }, { 5, 0, 0 }, { 6, 0, 0 },
	};
	const KDTree<Float3KDAdapter> tree{ points };
	const Float3 query{ 0, 0, 0 };
	// Preserve the existing strict distance < radius behavior.
	CHECK(tree.radiusSearch(query, 0.0f).isEmpty());
	CHECK((tree.radiusSearch(query, 1.0f)) == (Array<uint64>{ 0 }));
	CHECK((tree.rknnSearch(1.0f, 20, query)) == (Array<uint64>{ 0 }));
	CHECK((tree.radiusSearch(query, 2.0f).sort()) == (Array<uint64>{ 0, 1, 2, 3, 4, 5, 6 }));

	Array<std::pair<uint64, float>> nearest;
	tree.knnSearch(3, query, nearest);
	REQUIRE(nearest.size() == 3);
	CHECK((nearest[0]) == (std::pair<uint64, float>{ 0, 0.0f }));
	// Any two unit-distance points are valid; tied index ordering is unspecified.
	for (size_t i = 1; i < nearest.size(); ++i)
	{
		CHECK(nearest[i].second == 1.0f);
		CHECK(((1 <= nearest[i].first) && (nearest[i].first <= 6)));
	}
	CHECK(nearest[1].first != nearest[2].first);
}

TEST_CASE("KDTree.empty_release_and_rebuild")
{
	Array<Vec2> points;
	KDTree<Vec2KDAdapter> tree{ points };
	const Vec2 query{ 17.25, 0.125 };
	CHECK(tree.knnSearch(5, query).isEmpty());
	CHECK(tree.rknnSearch(10.0, 5, query).isEmpty());
	CHECK(tree.radiusSearch(query, 10.0).isEmpty());
	CHECK(tree.usedMemory() == 0);

	points = MakeKDPoints();
	tree.rebuildIndex();
	CHECK(tree.knnSearch(1, query) == KDIndices(ExpectedKDMatches(points, query, 1)));
	const size_t memoryBefore = tree.usedMemory();
	REQUIRE(memoryBefore > 0);
	tree.release();
	CHECK(tree.usedMemory() < memoryBefore);

	points[0] = Vec2{ -1000, -1000 };
	points.push_back(query);
	tree.rebuildIndex();
	CHECK((tree.knnSearch(1, query)) == (Array<uint32>{ 24 }));
	CHECK(tree.usedMemory() > 0);

	points.clear();
	tree.rebuildIndex();
	Array<uint32> indices{ 999 };
	Vec2Matches matches{ { 999, -1.0 } };
	tree.knnSearch(5, query, indices);
	tree.radiusSearch(query, 10.0, matches, SortByDistance::Yes);
	CHECK(indices.isEmpty());
	CHECK(matches.isEmpty());
}
