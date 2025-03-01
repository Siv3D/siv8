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

	inline DiscreteDistribution::DiscreteDistribution(std::initializer_list<double> weights)
		: m_distribution(weights) {}

	inline DiscreteDistribution::DiscreteDistribution(std::span<const double> weights)
		: m_distribution(weights.begin(), weights.end()) {}

	template <class Iterator>
	DiscreteDistribution::DiscreteDistribution(Iterator begin, Iterator end)
		: m_distribution(begin, end) {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	inline DiscreteDistribution& DiscreteDistribution::operator =(std::initializer_list<double> weights)
	{
		m_distribution = distribution_type(weights);
		return *this;
	}

	inline DiscreteDistribution& DiscreteDistribution::operator =(std::span<const double> weights)
	{
		m_distribution = distribution_type(weights.begin(), weights.end());
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	probabilities
	//
	////////////////////////////////////////////////////////////////

	inline Array<double> DiscreteDistribution::probabilities() const
	{
		return Array<double>(m_distribution.probabilities());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ()
	//
	////////////////////////////////////////////////////////////////

	DiscreteDistribution::result_type DiscreteDistribution::operator ()(Concept::UniformRandomBitGenerator auto&& rbg)
	{
		return m_distribution(std::forward<decltype(rbg)>(rbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	min, max
	//
	////////////////////////////////////////////////////////////////

	inline size_t DiscreteDistribution::min() const
	{
		return m_distribution.min();
	}

	inline size_t DiscreteDistribution::max() const
	{
		return m_distribution.max();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	inline size_t DiscreteDistribution::size() const
	{
		return m_distribution.probabilities().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	DiscreteSample
	//
	////////////////////////////////////////////////////////////////

	template <class Iterator>
	decltype(auto) DiscreteSample(Iterator begin, Iterator end, DiscreteDistribution& weight)
	{
		return DiscreteSample(begin, end, weight, GetDefaultRNG());
	}

	template <class Iterator>
	decltype(auto) DiscreteSample(Iterator begin, [[maybe_unused]] Iterator end, DiscreteDistribution& weight, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		assert(begin != end);
		assert(std::distance(begin, end) == static_cast<int64>(weight.size()));

		std::advance(begin, weight(std::forward<decltype(rbg)>(rbg)));
		return *begin;
	}

	template <class Container>
	decltype(auto) DiscreteSample(const Container& c SIV3D_LIFETIMEBOUND, DiscreteDistribution& weight)
	{
		return DiscreteSample(c, weight, GetDefaultRNG());
	}

	template <class Container>
	decltype(auto) DiscreteSample(const Container& c SIV3D_LIFETIMEBOUND, DiscreteDistribution& weight, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		assert(std::size(c) != 0);
		assert(std::size(c) == weight.size());

		auto it = std::begin(c);
		std::advance(it, weight(std::forward<decltype(rbg)>(rbg)));
		return *it;
	}

	template <class Type>
	auto DiscreteSample(std::initializer_list<Type> ilist, DiscreteDistribution& weight)
	{
		return DiscreteSample(ilist, weight, GetDefaultRNG());
	}

	template <class Type>
	auto DiscreteSample(std::initializer_list<Type> ilist, DiscreteDistribution& weight, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		assert(ilist.size() != 0);
		return *(ilist.begin() + weight(std::forward<decltype(rbg)>(rbg)));
	}
}
