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
# include "Concepts.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FloatRect
	//
	////////////////////////////////////////////////////////////////

	struct FloatRect
	{
		float left;
		
		float top;
			
		float right;
		
		float bottom;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		FloatRect() = default;

		[[nodiscard]]
		constexpr FloatRect(float l, float t, float r, float b) noexcept;

		[[nodiscard]]
		constexpr FloatRect(Concept::Arithmetic auto l, Concept::Arithmetic auto t, Concept::Arithmetic auto r, Concept::Arithmetic auto b) noexcept;
	};
}

# include "detail/FloatRect.ipp"

