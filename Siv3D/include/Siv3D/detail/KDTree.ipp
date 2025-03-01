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

# pragma once

namespace s3d
{
	namespace detail
	{
		template <class DatasetAdapter>
		KDAdapter<DatasetAdapter>::KDAdapter(const dataset_type& dataset)
			: m_dataset{ dataset } {}

		template <class DatasetAdapter>
		size_t KDAdapter<DatasetAdapter>::kdtree_get_point_count() const
		{
			return std::size(m_dataset);
		}

		template <class DatasetAdapter>
		typename KDAdapter<DatasetAdapter>::element_type KDAdapter<DatasetAdapter>::kdtree_get_pt(const index_type index, const size_t dim) const
		{
			return DatasetAdapter::GetElement(m_dataset, index, dim);
		}

		template <class DatasetAdapter>
		template <class BBOX>
		bool KDAdapter<DatasetAdapter>::kdtree_get_bbox(BBOX&) const
		{
			return false;
		}

		template <class DatasetAdapter>
		const typename KDAdapter<DatasetAdapter>::distance_type* KDAdapter<DatasetAdapter>::GetPointer(const point_type& point)
		{
			return DatasetAdapter::GetPointer(point);
		}

		template <class DistanceType, class IndexType>
		class RadiusResultsAdapter
		{
		public:

			using distance_type	= DistanceType;

			using index_type	= IndexType;

			const distance_type m_radius;

			Array<index_type>& m_results;

			RadiusResultsAdapter(distance_type radius, Array<index_type>& results)
				: m_radius{ radius }
				, m_results{ results }
			{
				init();
			}

			void init()
			{
				clear();
			}

			void clear()
			{
				m_results.clear();
			}

			size_t size() const
			{
				return m_results.size();
			}

			constexpr bool full() const
			{
				return true;
			}

			bool addPoint(const distance_type, const index_type index)
			{
				m_results.push_back(index);

				return true;
			}

			distance_type worstDist() const
			{
				return m_radius;
			}
		};

		template <class DistanceType, class IndexType>
		class RadiusResultsPairAdapter
		{
		public:

			using distance_type = DistanceType;

			using index_type = IndexType;

			const distance_type m_radius;

			Array<std::pair<index_type, distance_type>>& m_results;

			RadiusResultsPairAdapter(distance_type radius, Array<std::pair<index_type, distance_type>>& results)
				: m_radius{ radius }
				, m_results{ results }
			{
				init();
			}

			void init()
			{
				clear();
			}

			void clear()
			{
				m_results.clear();
			}

			size_t size() const
			{
				return m_results.size();
			}

			constexpr bool full() const
			{
				return true;
			}

			bool addPoint(const distance_type distanceSq, const index_type index)
			{
				m_results.emplace_back(index, distanceSq);

				return true;
			}

			distance_type worstDist() const
			{
				return m_radius;
			}
		};
	}

	template <class DatasetAdapter>
	KDTree<DatasetAdapter>::KDTree(const dataset_type& dataset)
		: m_adapter{ dataset }
		, m_index{ Dimensions, m_adapter, nanoflann::KDTreeSingleIndexAdaptorParams(10) }
	{
		rebuildIndex();
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::rebuildIndex()
	{
		m_index.buildIndex();
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::release()
	{
		m_index.freeIndex();
	}

	template <class DatasetAdapter>
	size_t KDTree<DatasetAdapter>::usedMemory() const
	{
		return m_index.usedMemory();
	}

	template <class DatasetAdapter>
	Array<typename KDTree<DatasetAdapter>::index_type> KDTree<DatasetAdapter>::knnSearch(size_t k, const point_type& point) const
	{
		Array<index_type> results;

		knnSearch(k, point, results);

		return results;
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::knnSearch(size_t k, const point_type& point, Array<index_type>& results) const
	{
		results.resize(k);

		Array<distance_type> distanceSqs(k);

		k = m_index.knnSearch(adapter_type::GetPointer(point), k, results.data(), distanceSqs.data());

		results.resize(k);
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::knnSearch(size_t k, const point_type& point, Array<std::pair<index_type, distance_type>>& results) const
	{
		Array<index_type> indices(k);
		Array<distance_type> distanceSqs(k);

		k = m_index.knnSearch(adapter_type::GetPointer(point), k, indices.data(), distanceSqs.data());

		results.resize(k);

		for (size_t i = 0; i < k; ++i)
		{
			results[i] = { indices[i], distanceSqs[i] };
		}
	}

	template <class DatasetAdapter>
	Array<typename KDTree<DatasetAdapter>::index_type> KDTree<DatasetAdapter>::rknnSearch(const distance_type radius, size_t k, const point_type& point) const
	{
		Array<index_type> results;

		rknnSearch(radius, k, point, results);

		return results;
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::rknnSearch(const distance_type radius, size_t k, const point_type& point, Array<index_type>& results) const
	{
		results.resize(k);

		Array<distance_type> distanceSqs(k);

		k = m_index.rknnSearch(adapter_type::GetPointer(point), k, results.data(), distanceSqs.data(), (radius * radius));

		results.resize(k);
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::rknnSearch(const distance_type radius, size_t k, const point_type& point, Array<std::pair<index_type, distance_type>>& results) const
	{
		Array<index_type> indices(k);
		Array<distance_type> distanceSqs(k);

		k = m_index.rknnSearch(adapter_type::GetPointer(point), k, indices.data(), distanceSqs.data(), (radius * radius));

		results.resize(k);

		for (size_t i = 0; i < k; ++i)
		{
			results[i] = { indices[i], distanceSqs[i] };
		}
	}

	template <class DatasetAdapter>
	Array<typename KDTree<DatasetAdapter>::index_type> KDTree<DatasetAdapter>::radiusSearch(const point_type& point, const distance_type radius, const SortByDistance sortByDistance) const
	{
		Array<index_type> results;

		radiusSearch(point, radius, results, sortByDistance);

		return results;
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::radiusSearch(const point_type& point, const distance_type radius, Array<index_type>& results, const SortByDistance sortByDistance) const
	{
		const nanoflann::SearchParameters searchParams{ 0.0f, sortByDistance.getBool() };

		if (sortByDistance)
		{
			std::vector<nanoflann::ResultItem<index_type, distance_type>> matches;

			nanoflann::RadiusResultSet<distance_type, index_type> resultSet{ (radius * radius), matches };

			const size_t num_matches = m_index.radiusSearchCustomCallback(adapter_type::GetPointer(point), resultSet, searchParams);

			if (searchParams.sorted)
			{
				std::sort(matches.begin(), matches.end(), nanoflann::IndexDist_Sorter());
			}

			results.resize(num_matches);

			index_type* pDst = results.data();
			const index_type* const pDstEnd = (pDst + num_matches);
			const nanoflann::ResultItem<index_type, distance_type>* pSrc = matches.data();

			while (pDst != pDstEnd)
			{
				*pDst++ = (pSrc++)->first;
			}
		}
		else
		{
			detail::RadiusResultsAdapter<distance_type, index_type> resultSet{ (radius * radius), results };

			m_index.radiusSearchCustomCallback(adapter_type::GetPointer(point), resultSet, searchParams);
		}
	}

	template <class DatasetAdapter>
	void KDTree<DatasetAdapter>::radiusSearch(const point_type& point, const distance_type radius, Array<std::pair<index_type, distance_type>>& results, const SortByDistance sortByDistance) const
	{
		const nanoflann::SearchParameters searchParams{ 0.0f, sortByDistance.getBool() };

		if (sortByDistance)
		{
			std::vector<nanoflann::ResultItem<index_type, distance_type>> matches;

			nanoflann::RadiusResultSet<distance_type, index_type> resultSet{ (radius * radius), matches };

			const size_t num_matches = m_index.radiusSearchCustomCallback(adapter_type::GetPointer(point), resultSet, searchParams);

			if (searchParams.sorted)
			{
				std::sort(matches.begin(), matches.end(), nanoflann::IndexDist_Sorter());
			}

			results.resize(num_matches);

			std::pair<index_type, distance_type>* pDst = results.data();
			const std::pair<index_type, distance_type>* const pDstEnd = (pDst + num_matches);
			const nanoflann::ResultItem<index_type, distance_type>* pSrc = matches.data();

			while (pDst != pDstEnd)
			{
				pDst->first = pSrc->first;
				pDst->second = pSrc->second;
				++pDst;
				++pSrc;
			}
		}
		else
		{
			detail::RadiusResultsPairAdapter<distance_type, index_type> resultSet{ (radius * radius), results };

			m_index.radiusSearchCustomCallback(adapter_type::GetPointer(point), resultSet, searchParams);
		}
	}
}
