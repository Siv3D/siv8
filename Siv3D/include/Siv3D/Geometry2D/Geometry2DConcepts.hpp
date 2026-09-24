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
# include "Contains.hpp"
# include "Overlaps.hpp"

namespace s3d::detail
{
	template <class A, class B>
	concept SupportsContains = requires(const A& a, const B& b)
	{
		{ Geometry2D::Contains(a, b) } -> std::same_as<bool>;
	};

	template <class A, class B>
	concept SupportsOverlaps = requires(const A& a, const B& b)
	{
		{ Geometry2D::Overlaps(a, b) } -> std::same_as<bool>;
	};
}
