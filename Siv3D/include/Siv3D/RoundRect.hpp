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
# include "NamedParameter.hpp"
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	/// @brief 角丸長方形
	struct RoundRect
	{
		using position_type	= RectF::position_type;

		using size_type		= RectF::size_type;

		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			RectF rect;

			struct
			{
				value_type x, y, w, h;
			};
		};

		value_type r;

	SIV3D_DISABLE_MSVC_WARNINGS_POP()

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		RoundRect() = default;

		[[nodiscard]]
		constexpr RoundRect(value_type _x, value_type _y, value_type _w, value_type _h, value_type _r) noexcept;
		
		[[nodiscard]]
		constexpr RoundRect(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, value_type _w, value_type _h, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(value_type _x, value_type _y, size_type size, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, size_type size, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, size_type size, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, size_type size, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, value_type _w, value_type _h, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, size_type size, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, size_type size, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(const RectF& _rect, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(const RectF& _rect, Concept::Arithmetic auto _r) noexcept;





		////////////////////////////////////////////////////////////////
		//
		//	leftClicked, leftPressed, leftReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形が現在のフレームで左クリックされ始めたかを返します。
		/// @return 角丸長方形が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 角丸長方形が左クリックされているかを返します。
		/// @return 角丸長方形が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで角丸長方形への左クリックが離されたかを返します。
		/// @return 現在のフレームで角丸長方形への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形が現在のフレームで右クリックされ始めたかを返します。
		/// @return 角丸長方形が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 角丸長方形が右クリックされているかを返します。
		/// @return 角丸長方形が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで角丸長方形への右クリックが離されたかを返します。
		/// @return 現在のフレームで角丸長方形への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形上にマウスカーソルがあるかを返します。
		/// @return 角丸長方形上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;



		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const RoundRect& draw(const ColorF& color = Palette::White) const;

		const RoundRect& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		const RoundRect& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		const RoundRect& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		const RoundRect& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		const RoundRect& drawFrame(double thickness, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		const RoundRect& drawFrame(double thickness, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		const RoundRect& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		const RoundRect& drawFrame(double innerThickness, double outerThickness, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		const RoundRect& drawFrame(double innerThickness, double outerThickness, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		const RoundRect& drawFrame(double thickness, const PatternParameters& pattern) const;

		const RoundRect& drawFrame(double innerThickness, double outerThickness, const PatternParameters& pattern) const;
	};
}
