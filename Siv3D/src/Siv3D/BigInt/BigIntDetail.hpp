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
# include <boost/multiprecision/cpp_int.hpp>
# include <Siv3D/BigInt.hpp>

namespace s3d
{
	struct BigInt::BigIntDetail
	{
		using value_type = boost::multiprecision::cpp_int;

		value_type value;

		BigIntDetail()
			: value{ 0 } {}

		BigIntDetail(const int64 i)
			: value{ i } {}

		BigIntDetail(const uint64 i)
			: value{ i } {}

		BigIntDetail(const std::string_view number)
			: value{ number } {}
	};
}
