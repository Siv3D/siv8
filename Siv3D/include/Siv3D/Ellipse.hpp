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
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	struct PatternParameters;

	/// @brief 楕円
	struct Ellipse
	{
		/// @brief 楕円の座標を表現する型
		using position_type	= Vec2;

		/// @brief 楕円の半径を表現する型
		using size_type		= Vec2;

		/// @brief 楕円の座標や半径の要素の型
		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 楕円の中心座標
			position_type center;

			struct
			{
				/// @brief 楕円の中心の X 座標
				value_type x;

				/// @brief 楕円の中心の Y 座標
				value_type y;
			};
		};

		union
		{
			/// @brief 楕円の X 軸 Y 軸に沿った半径
			size_type axes;

			struct
			{
				/// @brief 楕円の X 軸上の半径
				value_type a;

				/// @brief 楕円の Y 軸上の半径
				value_type b;
			};
		};

	SIV3D_DISABLE_MSVC_WARNINGS_POP()

		/// @brief 
		[[nodiscard]]
		Ellipse() = default;

		/// @brief 
		/// @param r 
		[[nodiscard]]
		explicit constexpr Ellipse(value_type r) noexcept;

		/// @brief 
		/// @param _a 
		/// @param _b 
		[[nodiscard]]
		constexpr Ellipse(value_type _a, value_type _b) noexcept;

		/// @brief 
		/// @param _x 
		/// @param _y 
		/// @param r 
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, value_type r) noexcept;

		/// @brief 
		/// @param _x 
		/// @param _y 
		/// @param _a 
		/// @param _b 
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, value_type _a, value_type _b) noexcept;

		/// @brief 
		/// @param _center 
		[[nodiscard]]
		explicit constexpr Ellipse(position_type _center) noexcept;

		/// @brief 
		/// @param _center 
		/// @param _r 
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, value_type r) noexcept;

		/// @brief 
		/// @param _center 
		/// @param _a 
		/// @param _b 
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, value_type _a, value_type _b) noexcept;

		/// @brief 
		/// @param _x 
		/// @param _y 
		/// @param _axes 
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, const size_type& _axes) noexcept;

		/// @brief 
		/// @param _center 
		/// @param _axes 
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, const size_type& _axes) noexcept;

		/// @brief 指定した円を楕円に変換します。
		/// @param circle 円
		[[nodiscard]]
		explicit constexpr Ellipse(const Circle& circle) noexcept;

		/// @brief 指定した長方形に内接する楕円を作成します。
		/// @param rect 長方形
		[[nodiscard]]
		explicit constexpr Ellipse(const RectF& rect) noexcept;





		////////////////////////////////////////////////////////////////
		//
		//	leftClicked, leftPressed, leftReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円が現在のフレームで左クリックされ始めたかを返します。
		/// @return 楕円が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 楕円が左クリックされているかを返します。
		/// @return 楕円が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで楕円への左クリックが離されたかを返します。
		/// @return 現在のフレームで楕円への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円が現在のフレームで右クリックされ始めたかを返します。
		/// @return 楕円が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 楕円が右クリックされているかを返します。
		/// @return 楕円が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで楕円への右クリックが離されたかを返します。
		/// @return 現在のフレームで楕円への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円上にマウスカーソルがあるかを返します。
		/// @return 楕円上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;






		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円を描きます。
		/// @param color 楕円の色
		/// @return *this
		const Ellipse& draw(const ColorF& color = Palette::White) const;

		/// @brief 楕円を描きます。
		/// @param innerColor 楕円の内側の色
		/// @param outerColor 楕円の外側の色
		/// @return *this
		const Ellipse& draw(const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 楕円を描きます。
		/// @param topColor 楕円の上側の色
		/// @param bottomColor 楕円の下側の色
		/// @return *this
		const Ellipse& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 円を描きます。
		/// @param leftColor 楕円の左側の色
		/// @param rightColor 楕円の右側の色
		/// @return *this
		const Ellipse& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 楕円を描きます。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Ellipse& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の枠を描きます
		/// @param thickness 枠の太さ
		/// @param color 枠の色
		/// @return *this
		const Ellipse& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 楕円の枠を描きます
		/// @param thickness 枠の太さ
		/// @param innerColor 内側部分の枠の色
		/// @param outerColor 外側部分の枠の色
		/// @return *this
		const Ellipse& drawFrame(double thickness, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 楕円の枠を描きます。
		/// @param innerThickness 基準の楕円から内側方向への枠の太さ
		/// @param outerThickness 基準の楕円から外側方向への枠の太さ
		/// @param color 枠の色
		/// @return *this
		const Ellipse& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		/// @brief 楕円の枠を描きます。
		/// @param innerThickness 基準の楕円から内側方向への枠の太さ
		/// @param outerThickness 基準の楕円から外側方向への枠の太さ
		/// @param innerColor 内側部分の枠の色
		/// @param outerColor 外側部分の枠の色
		/// @return *this
		const Ellipse& drawFrame(double innerThickness, double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 楕円の枠を描きます。
		/// @param thickness 枠の太さ
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Ellipse& drawFrame(double thickness, const PatternParameters& pattern) const;

		/// @brief 楕円の枠を描きます。
		/// @param innerThickness 基準の楕円から内側方向への枠の太さ
		/// @param outerThickness 基準の楕円から外側方向への枠の太さ
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Ellipse& drawFrame(double innerThickness, double outerThickness, const PatternParameters& pattern) const;
	};
}
