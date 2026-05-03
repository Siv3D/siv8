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

# include <Siv3D/IPv4Endpoint.hpp>

namespace s3d
{
	namespace
	{
		template <class Char>
		static Optional<IPv4Endpoint> ParseIPv4EndpointImpl(const Char* p, const Char* end)
		{
			std::array<uint8, 4> octets{ 0, 0, 0, 0 };

			//-------------------------
			// IPv4: A.B.C.D
			//-------------------------
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

			//-------------------------
			// Separator ':'
			//-------------------------
			if ((p == end) || (*p != Char(':')))
			{
				return none;
			}
			++p; // ':'

			//-------------------------
			// Port: 0..65535 (1～5 桁)
			//-------------------------
			if (p == end)
			{
				return none; // ポートは必須
			}

			// 先頭は必ず数字
			const Char pc0 = *p;
			if ((pc0 < Char('0')) || (Char('9') < pc0))
			{
				return none;
			}

			uint32 portValue = 0;
			int portDigits = 0;

			// leading zero 禁止（ただし "0" 単体は許容）
			const bool portLeadingZero = (pc0 == Char('0'));

			while ((p != end) && (Char('0') <= *p) && (*p <= Char('9')))
			{
				if (portDigits == 5)
				{
					return none; // 6 桁以上
				}

				portValue = (portValue * 10u) + static_cast<uint32>(*p - Char('0'));
				++portDigits;
				++p;
			}

			if (portDigits == 0)
			{
				return none;
			}

			if (portLeadingZero && (portDigits != 1))
			{
				return none; // "00", "01" など禁止
			}

			if (65535u < portValue)
			{
				return none;
			}

			// 余計な文字が残っていれば失敗（空白も禁止）
			if (p != end)
			{
				return none;
			}

			return IPv4Endpoint{
				IPv4Address{ octets[0], octets[1], octets[2], octets[3] },
				static_cast<IPv4Endpoint::port_type>(portValue)
			};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	to_string, str
	//
	////////////////////////////////////////////////////////////////

	std::string IPv4Endpoint::to_string() const
	{
		const auto& octets = m_address.octets();
		return fmt::format("{}.{}.{}.{}:{}", octets[0], octets[1], octets[2], octets[3], m_port);
	}

	String IPv4Endpoint::str() const
	{
		const auto& octets = m_address.octets();
		return fmt::format(U"{}.{}.{}.{}:{}", octets[0], octets[1], octets[2], octets[3], m_port);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	Optional<IPv4Endpoint> IPv4Endpoint::Parse(const std::string_view s)
	{
		return ParseIPv4EndpointImpl(s.data(), (s.data() + s.size()));
	}

	Optional<IPv4Endpoint> IPv4Endpoint::Parse(const StringView s)
	{
		return ParseIPv4EndpointImpl(s.data(), (s.data() + s.size()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const IPv4Endpoint& value)
	{
		formatData.string.append(value.str());
	}
}
