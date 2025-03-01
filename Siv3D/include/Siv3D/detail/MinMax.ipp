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
	template <Concept::Scalar Scalar>
	constexpr std::pair<Scalar, Scalar> MinMax(const Scalar a, const Scalar b)
	{
		return ((b < a) ? std::pair<Scalar, Scalar>{ b, a } : std::pair<Scalar, Scalar>{ a, b });
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr std::pair<const Type&, const Type&> MinMax(const Type& a, const Type& b)
	{
		return ((b < a) ? std::pair<const Type&, const Type&>{ b, a } : std::pair<const Type&, const Type&>{ a, b });
	}
}
