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
	//	Triangle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 三角形
	struct Triangle
	{
		using position_type = Vec2;

		using value_type = position_type::value_type;

		position_type p0;

		position_type p1;

		position_type p2;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Triangle() = default;

		[[nodiscard]]
		explicit constexpr Triangle(value_type sides) noexcept;

		[[nodiscard]]
		explicit constexpr Triangle(Concept::Arithmetic auto sides) noexcept;

		[[nodiscard]]
		Triangle(value_type sides, value_type angle) noexcept;

		[[nodiscard]]
		Triangle(Concept::Arithmetic auto sides, Concept::Arithmetic auto angle) noexcept;

		[[nodiscard]]
		constexpr Triangle(value_type x, value_type y, value_type sides) noexcept;

		[[nodiscard]]
		constexpr Triangle(Concept::Arithmetic auto x, Concept::Arithmetic auto y, Concept::Arithmetic auto sides) noexcept;

		[[nodiscard]]
		constexpr Triangle(const position_type& pos, value_type sides) noexcept;

		[[nodiscard]]
		constexpr Triangle(const position_type& pos, Concept::Arithmetic auto sides) noexcept;

		[[nodiscard]]
		Triangle(value_type x, value_type y, value_type sides, value_type angle) noexcept;

		[[nodiscard]]
		Triangle(Concept::Arithmetic auto x, Concept::Arithmetic auto y, Concept::Arithmetic auto sides, value_type angle) noexcept;

		[[nodiscard]]
		Triangle(const position_type& pos, value_type sides, value_type angle) noexcept;

		[[nodiscard]]
		Triangle(const position_type& pos, Concept::Arithmetic auto sides, value_type angle) noexcept;

		[[nodiscard]]
		constexpr Triangle(value_type x0, value_type y0, value_type x1, value_type y1, value_type x2, value_type y2) noexcept;

		[[nodiscard]]
		constexpr Triangle(Concept::Arithmetic auto x0, Concept::Arithmetic auto y0, Concept::Arithmetic auto x1, Concept::Arithmetic auto y1, Concept::Arithmetic auto x2, Concept::Arithmetic auto y2) noexcept;

		[[nodiscard]]
		constexpr Triangle(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept;


		//[[nodiscard]]
		//position_type& p(size_t index) noexcept;

		//[[nodiscard]]
		//const position_type& p(size_t index) const noexcept;


		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		constexpr Triangle& moveBy(value_type x, value_type y) noexcept;

		constexpr Triangle& moveBy(position_type v) noexcept;



		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr value_type area() const noexcept;


		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const Triangle& draw(const ColorF& color = Palette::White) const;

		const Triangle& draw(const ColorF& color0, const ColorF& color1, const ColorF& color2) const;
	};
}
