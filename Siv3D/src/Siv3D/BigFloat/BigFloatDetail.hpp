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
		{
			AssignFromBigInt(value, i);
		}

		BigFloatDetail(const std::string_view number)
			: value{ number } {}

		static void AssignFromBigInt(value_type& destination, const BigInt& i);

		[[nodiscard]]
		static value_type FromBigInt(const BigInt& i)
		{
			value_type result;
			AssignFromBigInt(result, i);
			return result;
		}
	};
}
