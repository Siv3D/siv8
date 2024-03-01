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
# include <memory>
# include "Common.hpp"
# include "Concepts.hpp"
# include "String.hpp"
# include "FormatData.hpp"

namespace s3d
{
	/// @brief 多倍長整数型
	class BigInt
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt();

		[[nodiscard]]
		BigInt(const BigInt& other);

		[[nodiscard]]
		BigInt(BigInt&& other) noexcept = default;

		[[nodiscard]]
		BigInt(Concept::SignedIntegral auto i);

		[[nodiscard]]
		BigInt(Concept::UnsignedIntegral auto i);

		[[nodiscard]]
		BigInt(int64 i);

		[[nodiscard]]
		BigInt(uint64 i);

		[[nodiscard]]
		explicit BigInt(std::string_view number);
		
		[[nodiscard]]
		explicit BigInt(StringView number);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~BigInt();







		////////////////////////////////////////////////////////////////
		//
		//	to_string
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		std::string to_string() const;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String str() const;






		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const BigInt& value);

	private:

		struct BigIntDetail;

		std::unique_ptr<BigIntDetail> pImpl;
	};
}

# include "detail/BigInt.ipp"
