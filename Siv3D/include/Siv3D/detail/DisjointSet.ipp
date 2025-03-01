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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	DisjointSet<IndexType>::DisjointSet(const size_t n)
		: m_parentsOrSize(n, static_cast<signed_index_type>(-1))
	{
		assert(n <= MaxSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	DisjointSet<IndexType>::operator bool() const noexcept
	{
		return (not m_parentsOrSize.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	bool DisjointSet<IndexType>::isEmpty() const noexcept
	{
		return m_parentsOrSize.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	size_t DisjointSet<IndexType>::size() const noexcept
	{
		return m_parentsOrSize.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	isize DisjointSet<IndexType>::ssize() const noexcept
	{
		return static_cast<isize>(m_parentsOrSize.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	find
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	typename DisjointSet<IndexType>::index_type DisjointSet<IndexType>::find(const index_type i) noexcept
	{
		assert(static_cast<size_t>(i) < size());

		if (m_parentsOrSize[i] < 0)
		{
			return i;
		}

		// path compression
		return (m_parentsOrSize[i] = find(static_cast<index_type>(m_parentsOrSize[i])));
	}

	////////////////////////////////////////////////////////////////
	//
	//	connected
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	bool DisjointSet<IndexType>::connected(const index_type i, const index_type k) noexcept
	{
		assert(static_cast<size_t>(i) < size());
		assert(static_cast<size_t>(k) < size());

		return (find(i) == find(k));
	}

	////////////////////////////////////////////////////////////////
	//
	//	isRoot
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	bool DisjointSet<IndexType>::isRoot(const index_type i) const noexcept
	{
		assert(static_cast<size_t>(i) < size());

		return (m_parentsOrSize[i] < 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	merge
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	bool DisjointSet<IndexType>::merge(index_type i, index_type k) noexcept
	{
		i = find(i);
		k = find(k);

		if (i == k) // i and k are already in the same set
		{
			return false;
		}

		// union by size
		if (-m_parentsOrSize[i] < -m_parentsOrSize[k])
		{
			using std::swap;
			swap(i, k);
		}

		m_parentsOrSize[i] += m_parentsOrSize[k];
		m_parentsOrSize[k] = i;

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	size_t DisjointSet<IndexType>::size(const index_type i) noexcept
	{
		assert(static_cast<size_t>(i) < size());

		return static_cast<size_t>(-m_parentsOrSize[find(i)]);
	}

	////////////////////////////////////////////////////////////////
	//
	//	countSets
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	size_t DisjointSet<IndexType>::countSets() const noexcept
	{
		size_t count = 0;

		const size_t size = m_parentsOrSize.size();

		for (size_t i = 0; i < size; ++i)
		{
			if (m_parentsOrSize[i] < 0)
			{
				++count;
			}
		}

		return count;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	void DisjointSet<IndexType>::reset() noexcept
	{
		std::fill(m_parentsOrSize.begin(), m_parentsOrSize.end(), static_cast<signed_index_type>(-1));
	}
}
