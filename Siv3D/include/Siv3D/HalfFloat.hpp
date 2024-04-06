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
# include "Common.hpp"
# include "Concepts.hpp"
# include "SIMDMath.hpp"

namespace s3d
{
	class HalfFloat
	{
	public:

		[[nodiscard]]
		HalfFloat() = default;

		[[nodiscard]]
		HalfFloat(float value) noexcept;

		[[nodiscard]]
		HalfFloat(Concept::Arithmetic auto value) noexcept;

		HalfFloat& operator =(float value) noexcept;

		HalfFloat& operator =(Concept::Arithmetic auto value);

		constexpr HalfFloat operator +() const noexcept;

		constexpr HalfFloat operator -() const noexcept;

		[[nodiscard]]
		float asFloat() const noexcept;

		[[nodiscard]]
		operator float() const noexcept;

		[[nodiscard]]
		constexpr uint16 getBits() const noexcept;

		constexpr void setBits(const uint16 bits) noexcept;

		[[nodiscard]]
		constexpr bool isZero() const noexcept;

		[[nodiscard]]
		constexpr bool isNaN() const noexcept;

		[[nodiscard]]
		constexpr bool isInfinity() const noexcept;

		[[nodiscard]]
		constexpr bool signBit() const noexcept;

		[[nodiscard]]
		constexpr HalfFloat abs() const noexcept;

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		[[nodiscard]]
		friend constexpr bool operator ==(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			if (IsZero(lhs.m_bits) && IsZero(rhs.m_bits))
			{
				return true;
			}

			if (IsNaN(lhs.m_bits) || IsNaN(rhs.m_bits))
			{
				return false;
			}

			return (lhs.m_bits == rhs.m_bits);
		}

		[[nodiscard]]
		friend constexpr bool operator >(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			if (IsZero(lhs.m_bits) && IsZero(rhs.m_bits))
			{
				return false;
			}

			if (IsNaN(lhs.m_bits) || IsNaN(rhs.m_bits))
			{
				return false;
			}

			if ((lhs.m_bits & HalfFloat::SignMask) == 0)
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == HalfFloat::SignMask)
				{
					return true;
				}
				else
				{
					return (lhs.m_bits > rhs.m_bits);
				}
			}
			else
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == 0)
				{
					return false;
				}
				else
				{
					return ((lhs.m_bits & 0x7FFF) < (rhs.m_bits & 0x7FFF));
				}
			}
		}

		[[nodiscard]]
		friend constexpr bool operator >=(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			if (IsZero(lhs.m_bits) && IsZero(rhs.m_bits))
			{
				return true;
			}

			if (IsNaN(lhs.m_bits) || IsNaN(rhs.m_bits))
			{
				return false;
			}
			
			if ((lhs.m_bits & HalfFloat::SignMask) == 0)
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == HalfFloat::SignMask)
				{
					return true;
				}
				else
				{
					return (lhs.m_bits >= rhs.m_bits);
				}
			}
			else
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == 0)
				{
					return false;
				}
				else
				{
					return ((lhs.m_bits & 0x7FFF) <= (rhs.m_bits & 0x7FFF));
				}
			}
		}

		[[nodiscard]]
		friend constexpr bool operator <(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			return (rhs > lhs);
		}

		[[nodiscard]]
		friend constexpr bool operator <=(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			return (rhs >= lhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const HalfFloat& value)
		{
			return (output << value.asFloat());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, HalfFloat& value)
		{
			float t = 0.0f;

			if (input >> t)
			{
				value = t;
			}

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const HalfFloat& value);

	private:

		static constexpr uint16 SignMask = 0x8000;

		uint16 m_bits = 0;

		[[nodiscard]]
		static constexpr bool IsZero(uint16 bits) noexcept;

		[[nodiscard]]
		static constexpr bool IsNaN(uint16 bits) noexcept;
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::HalfFloat>
{
	[[nodiscard]]
	size_t operator ()(const s3d::HalfFloat& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/HalfFloat.ipp"
