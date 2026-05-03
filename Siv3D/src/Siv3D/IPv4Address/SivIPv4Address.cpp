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

# include <Siv3D/IPv4Address.hpp>

namespace s3d
{
	namespace
	{
		template <class Char>
		static Optional<IPv4Address> ParseIPv4Impl(const Char* p, const Char* end)
		{
			std::array<uint8, 4> octets{ 0, 0, 0, 0 };

			for (int part = 0; part < 4; ++part)
			{
				// 1～3 桁の数字が必須
				if (p == end)
				{
					return none;
				}

				// 先頭は必ず数字
				const Char c0 = *p;
				if ((c0 < Char('0')) || (Char('9') < c0))
				{
					return none;
				}

				// 読み取り
				unsigned value = 0;
				int digits = 0;

				// leading zero 禁止（ただし "0" 単体は許容）
				const bool leadingZero = (c0 == Char('0'));

				while ((p != end) && (Char('0') <= *p) && (*p <= Char('9')))
				{
					if (digits == 3)
					{
						return none; // 4 桁以上
					}

					value = (value * 10u) + static_cast<unsigned>(*p - Char('0'));
					++digits;
					++p;
				}

				if (digits == 0)
				{
					return none;
				}

				if (leadingZero && (digits != 1))
				{
					return none; // "00", "01" など禁止
				}

				if (255u < value)
				{
					return none;
				}

				octets[part] = static_cast<uint8>(value);

				// 区切りチェック
				if (part < 3)
				{
					if ((p == end) || (*p != Char('.')))
					{
						return none;
					}
					++p; // '.'
				}
			}

			// 余計な文字が残っていれば失敗（空白も禁止）
			if (p != end)
			{
				return none;
			}

			return IPv4Address{ octets[0], octets[1], octets[2], octets[3] };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	to_string, str
	//
	////////////////////////////////////////////////////////////////

	std::string IPv4Address::to_string() const
	{
		return fmt::format("{}.{}.{}.{}", m_octets[0], m_octets[1], m_octets[2], m_octets[3]);
	}

	String IPv4Address::str() const
	{
		return fmt::format(U"{}.{}.{}.{}", m_octets[0], m_octets[1], m_octets[2], m_octets[3]);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	Optional<IPv4Address> IPv4Address::Parse(const std::string_view ip)
	{
		return ParseIPv4Impl(ip.data(), (ip.data() + ip.size()));
	}

	Optional<IPv4Address> IPv4Address::Parse(const StringView ip)
	{
		return ParseIPv4Impl(ip.data(), (ip.data() + ip.size()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const IPv4Address& value)
	{
		formatData.string.append(value.str());
	}
}
