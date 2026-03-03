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

	constexpr UUIDValue::UUIDValue(const std::span<const value_type, 16> uuid) noexcept
		: m_bytes{ uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7],
				 uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15] } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	isNil
	//
	////////////////////////////////////////////////////////////////

	constexpr bool UUIDValue::isNil() const noexcept
	{
		for (auto& value : m_bytes)
		{
			if (value != 0)
			{
				return false;
			}
		}

		return true;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	getData
	//
	////////////////////////////////////////////////////////////////

	constexpr const std::array<UUIDValue::value_type, 16>& UUIDValue::bytes() const noexcept
	{
		return m_bytes;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void UUIDValue::swap(UUIDValue& other)
	{
		m_bytes.swap(other.m_bytes);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 UUIDValue::hash() const noexcept
	{
		return Hash(m_bytes);
	}
}
