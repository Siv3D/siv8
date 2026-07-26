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

# include <cmath>
# include <utility>
# include <Siv3D/Char.hpp>
# include <Siv3D/ParseFloat.hpp>
# include <Siv3D/Polygon.hpp>
# include "PolygonParser.hpp"

namespace s3d
{
	PolygonParser::PolygonParser(const StringView source) noexcept
		: m_source{ source } {}

	Optional<Polygon> PolygonParser::parse()
	{
		if (not consume(U'('))
		{
			return none;
		}

		skipSpaces();

		if ((m_pos < m_source.size()) && (m_source[m_pos] == U')'))
		{
			++m_pos;
			skipSpaces();

			if (m_pos == m_source.size())
			{
				return Optional<Polygon>{ std::in_place };
			}

			return none;
		}

		Array<Vec2> outer;

		if (not parseRing(outer))
		{
			return none;
		}

		Array<Array<Vec2>> holes;

		for (;;)
		{
			skipSpaces();

			if (m_pos == m_source.size())
			{
				return none;
			}

			if (m_source[m_pos] == U')')
			{
				++m_pos;
				break;
			}

			if (m_source[m_pos] != U',')
			{
				return none;
			}

			++m_pos;
			holes.emplace_back();

			if (not parseRing(holes.back()))
			{
				return none;
			}
		}

		skipSpaces();

		if (m_pos != m_source.size())
		{
			return none;
		}

		Polygon polygon{ outer, std::move(holes) };

		if (polygon.isEmpty())
		{
			return none;
		}

		return polygon;
	}

	void PolygonParser::skipSpaces() noexcept
	{
		while ((m_pos < m_source.size()) && IsSpace(m_source[m_pos]))
		{
			++m_pos;
		}
	}

	bool PolygonParser::consume(const char32 expected) noexcept
	{
		skipSpaces();

		if ((m_pos == m_source.size()) || (m_source[m_pos] != expected))
		{
			return false;
		}

		++m_pos;
		return true;
	}

	bool PolygonParser::parseNumber(double& value) noexcept
	{
		skipSpaces();

		const size_t start = m_pos;

		if ((m_pos < m_source.size())
			&& ((m_source[m_pos] == U'+') || (m_source[m_pos] == U'-')))
		{
			++m_pos;
		}

		bool hasDigits = false;

		while ((m_pos < m_source.size()) && IsDigit(m_source[m_pos]))
		{
			hasDigits = true;
			++m_pos;
		}

		if ((m_pos < m_source.size()) && (m_source[m_pos] == U'.'))
		{
			++m_pos;

			while ((m_pos < m_source.size()) && IsDigit(m_source[m_pos]))
			{
				hasDigits = true;
				++m_pos;
			}
		}

		if (not hasDigits)
		{
			return false;
		}

		if ((m_pos < m_source.size())
			&& ((m_source[m_pos] == U'e') || (m_source[m_pos] == U'E')))
		{
			++m_pos;

			if ((m_pos < m_source.size())
				&& ((m_source[m_pos] == U'+') || (m_source[m_pos] == U'-')))
			{
				++m_pos;
			}

			const size_t exponentStart = m_pos;

			while ((m_pos < m_source.size()) && IsDigit(m_source[m_pos]))
			{
				++m_pos;
			}

			if (m_pos == exponentStart)
			{
				return false;
			}
		}

		const auto result = ParseFloatResult<double>(m_source.substr(start, (m_pos - start)));

		if ((not result) || (not std::isfinite(*result)))
		{
			return false;
		}

		value = *result;
		skipSpaces();
		return true;
	}

	bool PolygonParser::parsePoint(Vec2& point) noexcept
	{
		if (not consume(U'('))
		{
			return false;
		}

		if (not parseNumber(point.x))
		{
			return false;
		}

		if (not consume(U','))
		{
			return false;
		}

		if (not parseNumber(point.y))
		{
			return false;
		}

		return consume(U')');
	}

	bool PolygonParser::parseRing(Array<Vec2>& ring)
	{
		if (not consume(U'('))
		{
			return false;
		}

		Vec2 point;

		if (not parsePoint(point))
		{
			return false;
		}

		ring.push_back(point);

		for (;;)
		{
			skipSpaces();

			if (m_pos == m_source.size())
			{
				return false;
			}

			if (m_source[m_pos] == U')')
			{
				++m_pos;
				return true;
			}

			if (m_source[m_pos] != U',')
			{
				return false;
			}

			++m_pos;

			if (not parsePoint(point))
			{
				return false;
			}

			ring.push_back(point);
		}
	}
}
