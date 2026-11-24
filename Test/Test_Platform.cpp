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

# include "Siv3DTest.hpp"

namespace
{
	SIV3D_ALWAYS_INLINE
	static inline int AlwaysInlineIncrement(const int value) noexcept
	{
		return (value + 1);
	}

	struct AlwaysInlineMember
	{
		template <class Type>
		SIV3D_ALWAYS_INLINE
		constexpr Type identity(const Type value) const noexcept
		{
			return value;
		}
	};

	static_assert(AlwaysInlineMember{}.identity(0) == 0);
	static_assert(noexcept(AlwaysInlineIncrement(0)));
	static_assert(noexcept(AlwaysInlineMember{}.identity(0)));
}

TEST_CASE("Platform.always_inline")
{
	CHECK(AlwaysInlineIncrement(-1) == 0);
	CHECK(AlwaysInlineIncrement(41) == 42);
	CHECK(AlwaysInlineMember{}.identity(3.5) == 3.5);
}

TEST_CASE("Platform")
{
	Console << U"SIV3D_PLATFORM_NAME: "	SIV3D_PLATFORM_NAME;
	Console << U"SIV3D_CPU_ARCHITECTURE: " SIV3D_CPU_ARCHITECTURE;
	Console << U"SIV3D_INTRINSIC_TYPE: " SIV3D_INTRINSIC_TYPE;
	Console << U"SIV3D_COMPILER_NAME: " SIV3D_COMPILER_NAME;
	Console << U"SIV3D_BUILD_TYPE: " SIV3D_BUILD_TYPE;
	Console << U"SIV3D_USE_MIMALLOC: " << SIV3D_USE_MIMALLOC;
}
