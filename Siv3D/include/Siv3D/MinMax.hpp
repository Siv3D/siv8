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
# include <utility>
# include "Common.hpp"

namespace s3d
{
	template <Concept::Scalar Scalar>
	constexpr std::pair<Scalar, Scalar> MinMax(Scalar a, Scalar b);

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr std::pair<const Type&, const Type&> MinMax(const Type& a, const Type& b);
}

# include "detail/MinMax.ipp"
