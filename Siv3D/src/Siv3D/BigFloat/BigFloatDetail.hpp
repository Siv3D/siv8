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
# include <boost/multiprecision/cpp_dec_float.hpp>
# include <Siv3D/BigFloat.hpp>
# include <Siv3D/BigInt/BigIntDetail.hpp>

namespace s3d
{
	struct BigFloat::BigFloatDetail
	{
		using value_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>>;

		value_type value;

		BigFloatDetail()
			: value{ 0 } {}

		BigFloatDetail(const int64 i)
			: value{ i } {}

		BigFloatDetail(const uint64 i)
			: value{ i } {}

		BigFloatDetail(const long double f)
			: value{ f } {}

		BigFloatDetail(const BigInt& i)
			: value{ i._detail().value } {}

		BigFloatDetail(const std::string_view number)
			: value{ number } {}
	};
}
