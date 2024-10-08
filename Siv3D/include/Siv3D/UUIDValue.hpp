﻿//-----------------------------------------------
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
# include <array>
# include "Common.hpp"
# include "Utility.hpp"
# include "StringView.hpp"
# include "Optional.hpp"
# include "Random.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	UUIDValue
	//
	////////////////////////////////////////////////////////////////

	class UUIDValue
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	Variant
		//
		////////////////////////////////////////////////////////////////

		enum class Variant : uint8
		{
			NCS,
			RFC,
			Microsoft,
			Reserved,
		};

		////////////////////////////////////////////////////////////////
		//
		//	Version
		//
		////////////////////////////////////////////////////////////////

		enum class Version : uint8
		{
			Nil,
			TimeBased,
			DCE,
			NameBasedMD5,
			Random,
			NameBasedSHA1,
		};

		using value_type = uint8;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		UUIDValue() = default;

		[[nodiscard]]
		explicit UUIDValue(const value_type(&uuid)[16]) noexcept;

		[[nodiscard]]
		explicit constexpr UUIDValue(const std::array<value_type, 16>& uuid) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNil
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool isNil() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	variant
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Variant variant() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	version
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Version version() const noexcept;

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
		//	getData
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const std::array<value_type, 16>& getData() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		constexpr void swap(UUIDValue& other);

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const UUIDValue&, const UUIDValue&) = default;

		[[nodiscard]]
		friend constexpr auto operator <=>(const UUIDValue& lhs, const UUIDValue& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(UUIDValue& lhs, UUIDValue& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Generate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue Generate();

		////////////////////////////////////////////////////////////////
		//
		//	GenerateFromRNG
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue GenerateFromRNG(DefaultRNG& rng = GetDefaultRNG());

		////////////////////////////////////////////////////////////////
		//
		//	GenerateFromName
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue GenerateFromName(const UUIDValue& namespaceUUIDValue, std::string_view name);

		[[nodiscard]]
		static UUIDValue GenerateFromName(const UUIDValue& namespaceUUIDValue, StringView name);

		////////////////////////////////////////////////////////////////
		//
		//	Nil
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue Nil();

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceDNS
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue NamespaceDNS() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceURL
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue NamespaceURL() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceOID
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue NamespaceOID() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceX500
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static UUIDValue NamespaceX500() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	IsValid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static bool IsValid(std::string_view uuid);

		[[nodiscard]]
		static bool IsValid(StringView uuid);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Optional<UUIDValue> Parse(std::string_view uuid);

		[[nodiscard]]
		static Optional<UUIDValue> Parse(StringView uuid);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const UUIDValue& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const UUIDValue& value);

	private:

		std::array<value_type, 16> m_data{};
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::UUIDValue>
{
	[[nodiscard]]
	size_t operator ()(const s3d::UUIDValue& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/UUIDValue.ipp"
