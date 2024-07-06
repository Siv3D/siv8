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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Quad
	//
	////////////////////////////////////////////////////////////////

	/// @brief 凸四角形
	struct Quad
	{
		using position_type = Vec2;

		using value_type = position_type::value_type;

		position_type p0;

		position_type p1;

		position_type p2;

		position_type p3;


		[[nodiscard]]
		constexpr Quad movedBy(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		constexpr Quad movedBy(position_type v) const noexcept;

		constexpr Quad& moveBy(value_type x, value_type y) noexcept;

		constexpr Quad& moveBy(position_type v) noexcept;



		[[nodiscard]]
		position_type& p(size_t index) noexcept;

		[[nodiscard]]
		const position_type& p(size_t index) const noexcept;

		[[nodiscard]]
		constexpr Triangle triangleAtIndex(size_t index) const;



		const Quad& draw(const ColorF& color = Palette::White) const;

		const Quad& draw(const ColorF& color0, const ColorF& color1, const ColorF& color2, const ColorF& color3) const;

	private:

		[[noreturn]]
		static void ThrowTriangleAtIndexOutOfRange();
	};
}
