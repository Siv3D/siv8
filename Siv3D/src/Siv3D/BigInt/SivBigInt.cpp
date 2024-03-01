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

# include <Siv3D/BigInt.hpp>
# include <Siv3D/Unicode.hpp>
# include "BigIntDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BigInt::BigInt()
		: pImpl{ std::make_unique<BigIntDetail>() } {}

	BigInt::BigInt(const BigInt& other)
		: pImpl{ std::make_unique<BigIntDetail>(*other.pImpl) } {}

	BigInt::BigInt(const int64 i)
		: pImpl{ std::make_unique<BigIntDetail>(i) } {}

	BigInt::BigInt(const uint64 i)
		: pImpl{ std::make_unique<BigIntDetail>(i) } {}

	BigInt::BigInt(const std::string_view number)
		: pImpl{ std::make_unique<BigIntDetail>(number) } {}

	BigInt::BigInt(const StringView number)
		: BigInt{ Unicode::ToAscii(number) } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	BigInt::~BigInt() = default;









	////////////////////////////////////////////////////////////////
	//
	//	to_string
	//
	////////////////////////////////////////////////////////////////

	std::string BigInt::to_string() const
	{
		return pImpl->value.str();
	}

	////////////////////////////////////////////////////////////////
	//
	//	str
	//
	////////////////////////////////////////////////////////////////

	String BigInt::str() const
	{
		return Unicode::FromAscii(to_string());
	}










	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const BigInt& value)
	{
		formatData.string.append(value.str());
	}
}
