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

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Ellipse() = default;

		/// @brief 中心が (0, 0) の楕円（円）を作成します。
		/// @param r 楕円の半径
		[[nodiscard]]
		explicit constexpr Ellipse(value_type r) noexcept;

		/// @brief 中心が (0, 0) の楕円（円）を作成します。
		/// @param r 楕円の半径
		[[nodiscard]]
		explicit constexpr Ellipse(Concept::Arithmetic auto r) noexcept;

		/// @brief 中心が (0, 0) の楕円を作成します。
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(value_type _a, value_type _b) noexcept;

		/// @brief 中心が (0, 0) の楕円を作成します。
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(Concept::Arithmetic auto _a, Concept::Arithmetic auto _b) noexcept;

		/// @brief 楕円（円）を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param r 楕円の半径
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, value_type r) noexcept;

		/// @brief 楕円（円）を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param r 楕円の半径
		[[nodiscard]]
		constexpr Ellipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto r) noexcept;

		/// @brief 楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, value_type _a, value_type _b) noexcept;

		/// @brief 楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _a, Concept::Arithmetic auto _b) noexcept;

		/// @brief 楕円（円）を作成します。
		/// @param _center 中心座標
		/// @param _r 楕円の半径
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, value_type r) noexcept;

		/// @brief 楕円（円）を作成します。
		/// @param _center 中心座標
		/// @param _r 楕円の半径
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, Concept::Arithmetic auto r) noexcept;

		/// @brief 楕円を作成します。
		/// @param _center 中心座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, value_type _a, value_type _b) noexcept;

		/// @brief 楕円を作成します。
		/// @param _center 中心座標
		/// @param _a X 軸上の半径
		/// @param _b Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, Concept::Arithmetic auto _a, Concept::Arithmetic auto _b) noexcept;

		/// @brief 楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _axes X 軸上の半径と Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, const size_type& _axes) noexcept;

		/// @brief 楕円を作成します。
		/// @param _x 中心の X 座標
		/// @param _y 中心の Y 座標
		/// @param _axes X 軸上の半径と Y 軸上の半径
		[[nodiscard]]
		constexpr Ellipse(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, const size_type& _axes) noexcept;

		/// @brief 楕円を作成します。
		/// @param _center 中心座標
		/// @param _axes X 軸上の半径と Y 軸上の半径
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
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Ellipse が等しいかを返します。
		/// @param lhs 一方の Ellipse
		/// @param rhs もう一方の Ellipse
		/// @return 2 つの Ellipse が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Ellipse& lhs, const Ellipse& rhs) noexcept
		{
			return ((lhs.center == rhs.center)
				&& (lhs.axes == rhs.axes));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withX
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心の X 座標を変更した新しい楕円を返します。
		/// @param _x 新しい X 座標
		/// @return 中心の X 座標を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withX(value_type _x) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心の Y 座標を変更した新しい楕円を返します。
		/// @param _y 新しい Y 座標
		/// @return 中心の Y 座標を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を変更した新しい楕円を返します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @return 中心座標を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withCenter(value_type _x, value_type _y) const noexcept;

		/// @brief 中心座標を変更した新しい楕円を返します。
		/// @param _center 新しい中心座標
		/// @return 中心座標を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withA
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径を変更した新しい楕円を返します。
		/// @param _a 新しい X 軸上の半径
		/// @return X 軸上の半径を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withA(value_type _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withB
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸上の半径を変更した新しい楕円を返します。
		/// @param _b 新しい Y 軸上の半径
		/// @return Y 軸上の半径を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withB(value_type _b) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAxes
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径と Y 軸上の半径を変更した新しい楕円を返します。
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @return X 軸上の半径と Y 軸上の半径を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withAxes(value_type _a, value_type _b) const noexcept;

		/// @brief X 軸上の半径と Y 軸上の半径を変更した新しい楕円を返します。
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @return X 軸上の半径と Y 軸上の半径を変更した新しい楕円
		[[nodiscard]]
		constexpr Ellipse withAxes(size_type _axes) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の中心座標を変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @return *this
		/// @remark `.setCenter(_x, _y)` と同じです。
		constexpr Ellipse& setPos(value_type _x, value_type _y) noexcept;

		/// @brief 楕円の中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		/// @remark `.setCenter(_center)` と同じです。
		constexpr Ellipse& setPos(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の中心座標を変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @return *this
		/// @remark `.setPos(_x, _y)` と同じです。
		constexpr Ellipse& setCenter(value_type _x, value_type _y) noexcept;

		/// @brief 楕円の中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		/// @remark `.setPos(_center)` と同じです。
		constexpr Ellipse& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setA
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径を変更します。
		/// @param _a 新しい X 軸上の半径
		/// @return *this
		constexpr Ellipse& setA(value_type _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setB
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸上の半径を変更します。
		/// @param _b 新しい Y 軸上の半径
		/// @return *this
		constexpr Ellipse& setB(value_type _b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setAxes
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸上の半径と Y 軸上の半径を変更します。
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @return *this
		constexpr Ellipse& setAxes(value_type _a, value_type _b) noexcept;

		/// @brief X 軸上の半径と Y 軸上の半径を変更します。
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @return *this
		constexpr Ellipse& setAxes(size_type _axes) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param _x 新しい中心の X 座標
		/// @param _y 新しい中心の Y 座標
		/// @param _r 新しい半径
		/// @return *this
		constexpr Ellipse& set(value_type _x, value_type _y, value_type _r) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param _center 新しい中心座標
		/// @param _r 新しい半径
		/// @return *this
		constexpr Ellipse& set(const position_type& _center, value_type _r) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param _x 新しい中心の X 座標
		/// @param _y 新しい中心の Y 座標
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @return *this
		constexpr Ellipse& set(value_type _x, value_type _y, value_type _a, value_type _b) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param _center 新しい中心座標
		/// @param _a 新しい X 軸上の半径
		/// @param _b 新しい Y 軸上の半径
		/// @return *this
		constexpr Ellipse& set(const position_type& _center, value_type _a, value_type _b) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param _x 新しい中心の X 座標
		/// @param _y 新しい中心の Y 座標
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @return *this
		constexpr Ellipse& set(value_type _x, value_type _y, const size_type& _axes) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param _center 新しい中心座標
		/// @param _axes 新しい X 軸上の半径と Y 軸上の半径
		/// @return *this
		constexpr Ellipse& set(const position_type& _center, const size_type& _axes) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param circle 新しい楕円
		/// @return *this
		constexpr Ellipse& set(const Circle& circle) noexcept;

		/// @brief 楕円の中心座標と半径を変更します。
		/// @param ellipse 新しい楕円
		/// @return *this
		constexpr Ellipse& set(const Ellipse& ellipse) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を移動した新しい楕円を返します。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return 新しい楕円
		[[nodiscard]]
		constexpr Ellipse movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 中心座標を移動した新しい楕円を返します。
		/// @param v 移動量
		/// @return 新しい楕円
		[[nodiscard]]
		constexpr Ellipse movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を移動させます。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return *this
		constexpr Ellipse& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 中心座標を移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr Ellipse& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Ellipse stretched(value_type size) const noexcept;

		[[nodiscard]]
		constexpr Ellipse stretched(double _x, double _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Ellipse scaled(double s) const noexcept;

		[[nodiscard]]
		constexpr Ellipse scaled(double sx, double sy) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	top, right, bottom, left
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の周上で最も上にある点の座標を返します。
		/// @return 最も上にある点の座標
		[[nodiscard]]
		constexpr position_type top() const noexcept;

		/// @brief 楕円の周上で最も右にある点の座標を返します。
		/// @return 最も右にある点の座標
		[[nodiscard]]
		constexpr position_type right() const noexcept;

		/// @brief 楕円の周上で最も下にある点の座標を返します。
		/// @return 最も下にある点の座標
		[[nodiscard]]
		constexpr position_type bottom() const noexcept;

		/// @brief 楕円の周上で最も左にある点の座標を返します。
		/// @return 最も左にある点の座標
		[[nodiscard]]
		constexpr position_type left() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalDiameter, verticalDiameter
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸に平行な直径（線分）を返します。
		/// @return X 軸に平行な直径（線分）
		[[nodiscard]]
		constexpr Line horizontalDiameter() const noexcept;

		/// @brief Y 軸に平行な直径（線分）を返します。
		/// @return Y 軸に平行な直径（線分）
		[[nodiscard]]
		constexpr Line verticalDiameter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の面積を返します。
		/// @return 楕円の面積
		[[nodiscard]]
		constexpr value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 楕円の周の長さを返します。
		/// @return 楕円の周の長さ
		[[nodiscard]]
		constexpr value_type perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingCircle
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Circle boundingCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		///// @brief 楕円から Polygon を作成します。
		///// @param quality 品質
		///// @return Polygon
		//[[nodiscard]]
		//Polygon asPolygon(uint32 quality = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の楕円との線形補間を返します。
		/// @param other 別の楕円
		/// @param f 補間係数
		/// @return 線形補間された楕円
		[[nodiscard]]
		constexpr Ellipse lerp(const Ellipse& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と交差しているかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と交差している場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool intersects(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersetsAt
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//bool contains(const Shape2DType& other) const;

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
		//	paint
		//
		////////////////////////////////////////////////////////////////

		//const Ellipse& paint(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const Ellipse& overwrite(Image& dst, const Color& color) const;

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

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Ellipse の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Ellipse
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Ellipse& value)
		{
			return output << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.a << CharType(',') << CharType(' ')
				<< value.b << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Ellipse の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Ellipse の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Ellipse& value)
		{
			CharType unused;
			return input >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.a >> unused
				>> value.b >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Ellipse を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Ellipse
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Ellipse& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Ellipse>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Ellipse& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Ellipse, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Ellipse& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Ellipse>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Ellipse& value) const noexcept
	{
		return value.hash();
	}
};
