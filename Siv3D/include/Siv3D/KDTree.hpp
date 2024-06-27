//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Array.hpp"
# include "PredefinedYesNo.hpp"
# include <ThirdParty/nanoflann/nanoflann.hpp>

namespace s3d
{
	namespace detail
	{
		template <class DatasetAdapter>
		class KDAdapter
		{
		public:

			using point_type	= typename DatasetAdapter::point_type;

			using element_type	= typename DatasetAdapter::element_type;

			using distance_type = typename DatasetAdapter::distance_type;

			using dataset_type	= typename DatasetAdapter::dataset_type;

			using index_type	= typename DatasetAdapter::index_type;

			static constexpr int32 Dimensions = DatasetAdapter::Dimensions;

			explicit KDAdapter(const dataset_type& dataset);

			[[nodiscard]]
			size_t kdtree_get_point_count() const;

			[[nodiscard]]
			element_type kdtree_get_pt(index_type index, size_t dim) const;

			template <class BBOX>
			bool kdtree_get_bbox(BBOX&) const;

			[[nodiscard]]
			static const distance_type* GetPointer(const point_type& point);

		private:

			static_assert(sizeof(point_type) == sizeof(distance_type) * Dimensions);

			const dataset_type& m_dataset;
		};
	}

	/// @brief kd-tree
	/// @tparam DatasetAdapter kd-tree 用のアダプタ型
	template <class DatasetAdapter>
	class KDTree
	{
	public:

		using adapter_type	= detail::KDAdapter<DatasetAdapter>;

		using point_type	= typename adapter_type::point_type;

		using element_type	= typename adapter_type::element_type;

		using distance_type	= typename adapter_type::distance_type;

		using dataset_type	= typename adapter_type::dataset_type;

		using index_type	= typename adapter_type::index_type;

		static constexpr int32 Dimensions = adapter_type::Dimensions;

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		KDTree() = default;

		/// @brief kd-tree を構築します。
		/// @param dataset データセット
		[[nodiscard]]
		explicit KDTree(const dataset_type& dataset);

		/// @brief ツリーを再構築します。
		void rebuildIndex();

		/// @brief kd-tree を消去し、メモリから解放します。
		void release();

		/// @brief kd-tree が消費しているメモリのサイズ（バイト）を返します。
		/// @return kd-tree が消費しているメモリのサイズ（バイト）
		[[nodiscard]]
		size_t usedMemory() const;

		/// @brief 指定した座標から最も近い k 個の要素を検索して返します。
		/// @param k 検索する個数
		/// @param point 座標
		/// @return 見つかった要素一覧
		[[nodiscard]]
		Array<index_type> knnSearch(size_t k, const point_type& point) const;

		/// @brief 指定した座標から最も近い k 個の要素を検索して取得します。
		/// @param results 結果を格納する配列
		/// @param k 検索する個数
		/// @param point 中心座標
		void knnSearch(Array<index_type>& results, size_t k, const point_type& point) const;

		/// @brief 指定した座標から最も近い k 個の要素を検索して取得します。
		/// @param results 結果を格納する配列
		/// @param distanceSqResults それぞれの要素について、中心からの距離を格納する配列
		/// @param k 検索する個数
		/// @param point 中心座標
		void knnSearch(Array<index_type>& results, Array<distance_type>& distanceSqResults, size_t k, const point_type& point) const;

		/// @brief 指定した座標から指定した半径以内にある要素一覧を検索して返します。
		/// @param point 中心座標
		/// @param radius 半径
		/// @param sortByDistance 結果を中心座標から近い順にソートする場合 `SortByDistance::Yes`, それ以外の場合は `SortByDistance::No`
		/// @return 指定した位置から指定した半径以内にある要素一覧
		[[nodiscard]]
		Array<index_type> radiusSearch(const point_type& point, distance_type radius, SortByDistance sortByDistance = SortByDistance::No) const;

		/// @brief 指定した座標から指定した半径以内にある要素一覧を検索して取得します。
		/// @param results 結果を格納する配列
		/// @param point 中心座標
		/// @param radius 半径
		/// @param sortByDistance 結果を中心座標から近い順にソートする場合 `SortByDistance::Yes`, それ以外の場合は `SortByDistance::No`
		void radiusSearch(Array<index_type>& results, const point_type& point, distance_type radius, const SortByDistance sortByDistance = SortByDistance::No) const;

	private:

		adapter_type m_adapter;

		nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<element_type, adapter_type, distance_type, index_type>, adapter_type, Dimensions, index_type> m_index;
	};

	/// @brief KD-Tree 用のアダプタ
	/// @tparam Dataset データセットを表現する配列型
	/// @tparam PointType 座標を表現する型
	/// @tparam ElementType 座標の要素の型
	/// @tparam Dim 座標の次元
	/// @tparam IndexType インデックスの型
	template <class Dataset, class PointType, class ElementType = typename PointType::value_type, int32 Dim = PointType::Dimension, class IndexType = uint32>
	struct KDTreeAdapter
	{
		/// @brief データセットを表現する配列型
		using dataset_type	= Dataset;
		
		/// @brief 座標を表現する型
		using point_type	= PointType;
		
		/// @brief 座標の要素の型
		using element_type	= ElementType;

		/// @brief 距離の型
		using distance_type = element_type;

		/// @brief 座標の次元
		static constexpr int32 Dimensions = Dim;

		/// @brief インデックスの型
		using index_type	= IndexType;
	};

	template <class Dataset, class IndexType = uint32>
	using KDTreeFloat2Adapter = KDTreeAdapter<Dataset, Float2, float, 2, IndexType>;

	template <class Dataset, class IndexType = uint32>
	using KDTreeFloat3Adapter = KDTreeAdapter<Dataset, Float3, float, 3, IndexType>;

	template <class Dataset, class IndexType = uint32>
	using KDTreeFloat4Adapter = KDTreeAdapter<Dataset, Float4, float, 4, IndexType>;

	template <class Dataset, class IndexType = uint32>
	using KDTreeVec2Adapter = KDTreeAdapter<Dataset, Vec2, double, 2, IndexType>;

	template <class Dataset, class IndexType = uint32>
	using KDTreeVec3Adapter = KDTreeAdapter<Dataset, Vec3, double, 3, IndexType>;

	template <class Dataset, class IndexType = uint32>
	using KDTreeVec4Adapter = KDTreeAdapter<Dataset, Vec4, double, 4, IndexType>;
}

# include "detail/KDTree.ipp"
