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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr IPv4Address::IPv4Address(const value_type a, const value_type b, const value_type c, const value_type d) noexcept
		: m_octets{ a, b, c, d } {}

	constexpr IPv4Address::IPv4Address(const std::span<const value_type, 4> ipv4)
		: m_octets{ ipv4[0], ipv4[1], ipv4[2], ipv4[3] } {}

	////////////////////////////////////////////////////////////////
	//
	//	isUnspecified, isAny
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isUnspecified() const noexcept
	{
		return (m_octets == std::array<value_type, 4>{ 0, 0, 0, 0 });
	}

	constexpr bool IPv4Address::isAny() const noexcept
	{
		return isUnspecified();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isLoopback
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isLoopback() const noexcept
	{
		return (m_octets[0] == 127);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isPrivate
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isPrivate() const noexcept
	{
		// 10.0.0.0/8
		if (m_octets[0] == 10)
		{
			return true;
		}

		// 172.16.0.0/12  (172.16.0.0 - 172.31.255.255)
		if ((m_octets[0] == 172) && (16 <= m_octets[1]) && (m_octets[1] <= 31))
		{
			return true;
		}

		// 192.168.0.0/16
		if ((m_octets[0] == 192) && (m_octets[1] == 168))
		{
			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isCarrierGradeNAT
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isCarrierGradeNAT() const noexcept
	{
		// 100.64.0.0/10  (100.64.0.0 - 100.127.255.255)
		return ((m_octets[0] == 100) && (64 <= m_octets[1]) && (m_octets[1] <= 127));
	}

	////////////////////////////////////////////////////////////////
	//
	//  isLinkLocal
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isLinkLocal() const noexcept
	{
		// 169.254.0.0/16
		return ((m_octets[0] == 169) && (m_octets[1] == 254));
	}

	////////////////////////////////////////////////////////////////
	//
	//  isMulticast
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isMulticast() const noexcept
	{
		// 224.0.0.0/4  => 224..239
		return (224 <= m_octets[0]) && (m_octets[0] <= 239);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBroadcast
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IPv4Address::isBroadcast() const noexcept
	{
		return (m_octets == std::array<value_type, 4>{ 255, 255, 255, 255 });
	}

	////////////////////////////////////////////////////////////////
	//
	//  toNetworkUint32
	//
	////////////////////////////////////////////////////////////////

	constexpr uint32 IPv4Address::toNetworkUint32() const noexcept
	{
		return (static_cast<uint32>(m_octets[0]) << 24)
			| (static_cast<uint32>(m_octets[1]) << 16)
			| (static_cast<uint32>(m_octets[2]) << 8)
			| (static_cast<uint32>(m_octets[3]));
	}

	////////////////////////////////////////////////////////////////
	//
	//	octets
	//
	////////////////////////////////////////////////////////////////

	constexpr const std::array<IPv4Address::value_type, 4>& IPv4Address::octets() const noexcept
	{
		return m_octets;
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void IPv4Address::swap(IPv4Address& other)
	{
		m_octets.swap(other.m_octets);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 IPv4Address::hash() const noexcept
	{
		return Hash(m_octets);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Unspecified, Any
	//
	////////////////////////////////////////////////////////////////

	constexpr IPv4Address IPv4Address::Unspecified() noexcept
	{
		return{ 0, 0, 0, 0 };
	}

	constexpr IPv4Address IPv4Address::Any() noexcept
	{
		return Unspecified();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Localhost
	//
	////////////////////////////////////////////////////////////////

	constexpr IPv4Address IPv4Address::Localhost() noexcept
	{
		return{ 127, 0, 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//  FromNetworkUint32
	//
	////////////////////////////////////////////////////////////////

	constexpr IPv4Address IPv4Address::FromNetworkUint32(const uint32 value) noexcept
	{
		return{
			static_cast<value_type>((value >> 24) & 0xFF),
			static_cast<value_type>((value >> 16) & 0xFF),
			static_cast<value_type>((value >> 8) & 0xFF),
			static_cast<value_type>((value >> 0) & 0xFF),
		};
	}
}
