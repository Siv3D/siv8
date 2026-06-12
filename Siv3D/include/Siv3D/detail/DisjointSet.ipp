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
		, m_setCount{ n }
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
	//	root
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	typename DisjointSet<IndexType>::index_type DisjointSet<IndexType>::root(const index_type i) noexcept
	{
		assert(static_cast<size_t>(i) < size());

		while (0 <= m_parentsOrSize[i])
		{
			const signed_index_type parent = m_parentsOrSize[i];

			if (0 <= m_parentsOrSize[parent])
			{
				// path halving
				m_parentsOrSize[i] = m_parentsOrSize[parent];
			}

			i = static_cast<index_type>(parent);
		}

		return i;
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

		return (root(i) == root(k));
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
		i = root(i);
		k = root(k);

		if (i == k) // i and k are already in the same set
		{
			return false;
		}

		// union by size
		if (m_parentsOrSize[i] > m_parentsOrSize[k])
		{
			std::ranges::swap(i, k);
		}

		m_parentsOrSize[i] += m_parentsOrSize[k];
		m_parentsOrSize[k] = i;
		--m_setCount;

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

		return (0 - static_cast<size_t>(m_parentsOrSize[root(i)]));
	}

	////////////////////////////////////////////////////////////////
	//
	//	countSets
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	size_t DisjointSet<IndexType>::countSets() const noexcept
	{
		return m_setCount;
	}

	////////////////////////////////////////////////////////////////
	//
	//	groups
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	Array<Array<typename DisjointSet<IndexType>::index_type>> DisjointSet<IndexType>::groups()
	{
		const size_t n = size();

		Array<index_type> roots(n);
		Array<size_t> groupIndices(n);
		size_t groupCount = 0;

		for (size_t i = 0; i < n; ++i)
		{
			roots[i] = root(static_cast<index_type>(i));

			if (static_cast<size_t>(roots[i]) == i)
			{
				groupIndices[i] = groupCount++;
			}
		}

		Array<Array<index_type>> result(groupCount);

		for (size_t i = 0; i < n; ++i)
		{
			result[groupIndices[roots[i]]].push_back(static_cast<index_type>(i));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	void DisjointSet<IndexType>::resize(const size_t n)
	{
		assert(m_parentsOrSize.size() <= n);
		assert(n <= MaxSize);

		m_setCount += (n - m_parentsOrSize.size());
		m_parentsOrSize.resize(n, static_cast<signed_index_type>(-1));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	void DisjointSet<IndexType>::reset() noexcept
	{
		std::ranges::fill(m_parentsOrSize, static_cast<signed_index_type>(-1));
		m_setCount = m_parentsOrSize.size();
	}
}
