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

TEST_CASE("VertexNormalWeighting")
{
	static_assert(FromEnum(VertexNormalWeighting::Angle) == 0);
	static_assert(FromEnum(VertexNormalWeighting::Area) == 1);
	static_assert(FromEnum(VertexNormalWeighting::Uniform) == 2);
	static_assert(std::is_same_v<std::underlying_type_t<VertexNormalWeighting>, uint8>);

	CHECK((Format(VertexNormalWeighting::Angle)) == (U"Angle"));
	CHECK((Format(VertexNormalWeighting::Area)) == (U"Area"));
	CHECK((Format(VertexNormalWeighting::Uniform)) == (U"Uniform"));
}
