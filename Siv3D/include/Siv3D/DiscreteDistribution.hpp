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
# include <span>
# include "Common.hpp"
# include "Array.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(6294)
# include <ThirdParty/absl/random/discrete_distribution.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DiscreteDistribution
	//
	////////////////////////////////////////////////////////////////

	class DiscreteDistribution
	{
	public:

		using result_type = size_t;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		DiscreteDistribution() = default;

		[[nodiscard]]
		explicit DiscreteDistribution(std::initializer_list<double> weights);

		[[nodiscard]]
		explicit DiscreteDistribution(std::span<const double> weights);

		template <class Iterator>
		[[nodiscard]]
		explicit DiscreteDistribution(Iterator begin, Iterator end);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		DiscreteDistribution& operator =(std::initializer_list<double> weights);

		DiscreteDistribution& operator =(std::span<const double> weights);

		////////////////////////////////////////////////////////////////
		//
		//	probabilities
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<double> probabilities() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		result_type operator ()(Concept::UniformRandomBitGenerator auto&& rbg);

		////////////////////////////////////////////////////////////////
		//
		//	min, max
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		size_t min() const;

		[[nodiscard]]
		size_t max() const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		size_t size() const;

	private:

		using distribution_type = absl::discrete_distribution<size_t>;

		distribution_type m_distribution;
	};

	////////////////////////////////////////////////////////////////
	//
	//	DiscreteSample
	//
	////////////////////////////////////////////////////////////////

	template <class Iterator>
	[[nodiscard]]
	decltype(auto) DiscreteSample(Iterator begin, Iterator end, DiscreteDistribution& weight);

	template <class Iterator>
	[[nodiscard]]
	decltype(auto) DiscreteSample(Iterator begin, Iterator end, DiscreteDistribution& weight, Concept::UniformRandomBitGenerator auto&& rbg);

	template <class Container>
	[[nodiscard]]
	decltype(auto) DiscreteSample(const Container& c SIV3D_LIFETIMEBOUND, DiscreteDistribution& weight);

	template <class Container>
	[[nodiscard]]
	decltype(auto) DiscreteSample(const Container& c SIV3D_LIFETIMEBOUND, DiscreteDistribution& weight, Concept::UniformRandomBitGenerator auto&& rbg);

	template <class Type>
	[[nodiscard]]
	auto DiscreteSample(std::initializer_list<Type> ilist, DiscreteDistribution& weight);
	
	template <class Type>
	[[nodiscard]]
	auto DiscreteSample(std::initializer_list<Type> ilist, DiscreteDistribution& weight, Concept::UniformRandomBitGenerator auto&& rbg);
}

# include "detail/DiscreteDistribution.ipp"
