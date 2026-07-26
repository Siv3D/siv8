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
# include <Siv3D/Array.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/StringView.hpp>

namespace s3d
{
	class PolygonParser
	{
	public:

		[[nodiscard]]
		explicit PolygonParser(StringView source) noexcept;

		[[nodiscard]]
		Optional<Polygon> parse();

	private:

		void skipSpaces() noexcept;

		[[nodiscard]]
		bool consume(char32 expected) noexcept;

		[[nodiscard]]
		bool parseNumber(double& value) noexcept;

		[[nodiscard]]
		bool parsePoint(Vec2& point) noexcept;

		[[nodiscard]]
		bool parseRing(Array<Vec2>& ring);

		StringView m_source;

		size_t m_pos = 0;
	};
}
