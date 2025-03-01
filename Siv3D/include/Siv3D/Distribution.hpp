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
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(6294)
# include <ThirdParty/absl/random/distributions.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()
# include "Common.hpp"
# include "Concepts.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	UniformIntDistribution
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IntType = int32>
	using UniformIntDistribution = absl::uniform_int_distribution<IntType>;

	////////////////////////////////////////////////////////////////
	//
	//	UniformRealDistribution
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint RealType = double>
	using UniformRealDistribution = absl::uniform_real_distribution<RealType>;

	////////////////////////////////////////////////////////////////
	//
	//	NormalDistribution
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint RealType = double>
	using NormalDistribution = absl::gaussian_distribution<RealType>;

	////////////////////////////////////////////////////////////////
	//
	//	BernoulliDistribution
	//
	////////////////////////////////////////////////////////////////

	using BernoulliDistribution = absl::bernoulli_distribution;

	namespace detail
	{
		template <class Type>
		struct UniformDistributionHelper
		{
			using DistributionType = UniformIntDistribution<Type>;
		};

		template <>
		struct UniformDistributionHelper<float>
		{
			using DistributionType = UniformRealDistribution<float>;
		};

		template <>
		struct UniformDistributionHelper<double>
		{
			using DistributionType = UniformRealDistribution<double>;
		};

		template <>
		struct UniformDistributionHelper<long double>
		{
			using DistributionType = UniformRealDistribution<long double>;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	UniformDistribution
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Arithmetic Arithmetic>
	using UniformDistribution = typename detail::UniformDistributionHelper<Arithmetic>::DistributionType;
}
