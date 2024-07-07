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
	////////////////////////////////////////////////////////////////
	//
	//	Circle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 円
	struct Circle
	{
		using position_type	= Vec2;

		using size_type		= position_type::value_type;

		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 円の中心座標
			position_type center;

			struct
			{
				/// @brief 円の中心の X 座標
				value_type x;

				/// @brief 円の中心の Y 座標
				value_type y;
			};
		};

		/// @brief 円の半径
		/// @remark この値が負の時の挙動は未規定です。
		size_type r;

	SIV3D_DISABLE_MSVC_WARNINGS_POP()
	
		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Circle() = default;

		[[nodiscard]]
		explicit constexpr Circle(size_type _r) noexcept;

		[[nodiscard]]
		explicit constexpr Circle(Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(value_type _x, value_type _y, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(position_type _center, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(position_type _center, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::center_<position_type> _center, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::center_<position_type> _center, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::topLeft_<position_type> _topLeft, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::topLeft_<position_type> _topLeft, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::topCenter_<position_type> _topCenter, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::topCenter_<position_type> _topCenter, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::topRight_<position_type> _topRight, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::topRight_<position_type> _topRight, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::rightCenter_<position_type> _rightCenter, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::rightCenter_<position_type> _rightCenter, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::bottomRight_<position_type> _bottomRight, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::bottomRight_<position_type> _bottomRight, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::bottomCenter_<position_type> _bottomCenter, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::bottomCenter_<position_type> _bottomCenter, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::bottomLeft_<position_type> _bottomLeft, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::bottomLeft_<position_type> _bottomLeft, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::leftCenter_<position_type> _leftCenter, size_type _r) noexcept;

		[[nodiscard]]
		constexpr Circle(Arg::leftCenter_<position_type> _leftCenter, Concept::Arithmetic auto _r) noexcept;

		/// @brief 指定した 2 点を通る円を作成します。
		/// @param p0 点
		/// @param p1 点
		[[nodiscard]]
		Circle(position_type p0, position_type p1) noexcept;

		/// @brief 指定した 3 点を通る円を作成します。
		/// @param p0 点
		/// @param p1 点
		/// @param p2 点
		[[nodiscard]]
		Circle(const position_type& p0, const position_type& p1, const position_type& p2) noexcept;

		/// @brief 指定した直径を持つ円を作成します。
		/// @param diameter 直径
		[[nodiscard]]
		explicit Circle(const Line& diameter) noexcept;
	
		/// @brief 中心と、円周上の点を指定して円を作成します。
		/// @param _center 中心座標
		/// @param p 円周上の点
		[[nodiscard]]
		Circle(Arg::center_<position_type> _center, const position_type& p) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const Circle& lhs, const Circle& rhs) noexcept
		{
			return ((lhs.center == rhs.center)
				 && (lhs.r == rhs.r));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withX
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Circle withX(value_type _x) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withY
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Circle withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Circle withCenter(value_type _x, value_type _y) const noexcept;

		[[nodiscard]]
		constexpr Circle withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withR
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Circle withR(value_type _r) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		constexpr Circle& setPos(value_type _x, value_type _y) noexcept;

		constexpr Circle& setPos(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		constexpr Circle& setCenter(value_type _x, value_type _y) noexcept;

		constexpr Circle& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setR
		//
		////////////////////////////////////////////////////////////////

		constexpr Circle& setR(value_type _r) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr Circle& set(value_type _x, value_type _y, size_type _r) noexcept;

		constexpr Circle& set(position_type _center, size_type _r) noexcept;

		constexpr Circle& set(Arg::center_<position_type> _center, size_type _r) noexcept;

		constexpr Circle& set(Arg::topLeft_<position_type> topLeft, size_type _r) noexcept;

		constexpr Circle& set(Arg::topCenter_<position_type> topCenter, size_type _r) noexcept;

		constexpr Circle& set(Arg::topRight_<position_type> topRight, size_type _r) noexcept;

		constexpr Circle& set(Arg::rightCenter_<position_type> rightCenter, size_type _r) noexcept;

		constexpr Circle& set(Arg::bottomRight_<position_type> bottomRight, size_type _r) noexcept;

		constexpr Circle& set(Arg::bottomCenter_<position_type> bottomCenter, size_type _r) noexcept;

		constexpr Circle& set(Arg::bottomLeft_<position_type> bottomLeft, size_type _r) noexcept;

		constexpr Circle& set(Arg::leftCenter_<position_type> leftCenter, size_type _r) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を移動した新しい円を返します。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return 新しい円
		[[nodiscard]]
		constexpr Circle movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 中心座標を移動した新しい円を返します。
		/// @param v 移動量
		/// @return 新しい円
		[[nodiscard]]
		constexpr Circle movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を移動させます。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return *this
		constexpr Circle& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 中心座標を移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr Circle& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 半径を変化させた新しい円を返します。
		/// @param size 半径の変化量
		/// @return 新しい円
		[[nodiscard]]
		constexpr Circle stretched(value_type size) const noexcept;

		/// @brief X 軸、Y 軸上で径を変化させて楕円を作成します。
		/// @param _x X 軸上の径の変化量
		/// @param _y Y 軸上の径の変化量
		/// @return 楕円
		[[nodiscard]]
		constexpr Ellipse stretched(double _x, double _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 半径を拡大した新しい円を返します。
		/// @param s 拡大倍率
		/// @return 新しい円
		[[nodiscard]]
		constexpr Circle scaled(double s) const noexcept;

		/// @brief X 軸、Y 軸上に拡大して楕円を作成します。
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 楕円
		[[nodiscard]]
		constexpr Ellipse scaled(double sx, double sy) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	top, right, bottom, left
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円周上で最も上にある点の座標を返します。
		/// @return 最も上にある点の座標
		[[nodiscard]]
		constexpr position_type top() const noexcept;

		/// @brief 円周上で最も右にある点の座標を返します。
		/// @return 最も右にある点の座標
		[[nodiscard]]
		constexpr position_type right() const noexcept;

		/// @brief 円周上で最も下にある点の座標を返します。
		/// @return 最も下にある点の座標
		[[nodiscard]]
		constexpr position_type bottom() const noexcept;

		/// @brief 円周上で最も左にある点の座標を返します。
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

		/// @brief 円の面積を返します。
		/// @return 円の面積
		[[nodiscard]]
		constexpr value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円の周の長さを返します。
		/// @return 円の周の長さ
		[[nodiscard]]
		constexpr value_type perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円に外接する正方形を返します。
		/// @return 円に外接する正方形
		[[nodiscard]]
		constexpr RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointByAngle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円周上の点をラジアン (12 時方向が 0, 3 時方向が π/2, 6 時方向が π) で取得します。
		/// @param angle 方向（ラジアン）
		/// @return 円周上の点
		[[nodiscard]]
		position_type getPointByAngle(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		///// @brief 円から Polygon を作成します。
		///// @param quality 品質
		///// @return Polygon
		//[[nodiscard]]
		//Polygon asPolygon(uint32 quality = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	pieAsPolygon
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon pieAsPolygon(double startAngle, double angle, uint32 quality = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	arcAsPolygon
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon arcAsPolygon(double startAngle, double angle, double innerThickness, double outerThickness, uint32 quality = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Circle lerp(const Circle& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

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

		/// @brief 円が現在のフレームで左クリックされ始めたかを返します。
		/// @return 円が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 円が左クリックされているかを返します。
		/// @return 円が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで円への左クリックが離されたかを返します。
		/// @return 現在のフレームで円への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円が現在のフレームで右クリックされ始めたかを返します。
		/// @return 円が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 円が右クリックされているかを返します。
		/// @return 円が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで円への右クリックが離されたかを返します。
		/// @return 現在のフレームで円への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円上にマウスカーソルがあるかを返します。
		/// @return 円上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		//const Circle& paint(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const Circle& overwrite(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintFrame
		//
		////////////////////////////////////////////////////////////////

		//const Circle& paintFrame(Image& dst, double innerThickness, double outerThickness, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteFrame
		//
		////////////////////////////////////////////////////////////////

		//const Circle& overwriteFrame(Image& dst, double innerThickness, double outerThickness, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintPie
		//
		////////////////////////////////////////////////////////////////

		//const Circle& paintPie(Image& dst, double startAngle, double angle, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwritePie
		//
		////////////////////////////////////////////////////////////////

		//const Circle& overwritePie(Image& dst, double startAngle, double angle, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintArc
		//
		////////////////////////////////////////////////////////////////

		//const Circle& paintArc(Image& dst, double startAngle, double angle, double innerThickness, double outerThickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteArc
		//
		////////////////////////////////////////////////////////////////

		//const Circle& overwriteArc(Image& dst, double startAngle, double angle, double innerThickness, double outerThickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円を描きます。
		/// @param color 円の色
		/// @return *this
		const Circle& draw(const ColorF& color = Palette::White) const;

		/// @brief 円を描きます。
		/// @param innerColor 円の内側の色
		/// @param outerColor 円の外側の色
		/// @return *this
		const Circle& draw(const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 円を描きます。
		/// @param topColor 円の上側の色
		/// @param bottomColor 円の下側の色
		/// @return *this
		const Circle& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 円を描きます。
		/// @param leftColor 円の左側の色
		/// @param rightColor 円の右側の色
		/// @return *this
		const Circle& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円の枠を描きます
		/// @param thickness 枠の太さ
		/// @param color 枠の色
		/// @return *this
		const Circle& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 円の枠を描きます
		/// @param thickness 枠の太さ
		/// @param innerColor 内側部分の枠の色
		/// @param outerColor 外側部分の枠の色
		/// @return *this
		const Circle& drawFrame(double thickness, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 円の枠を描きます。
		/// @param innerThickness 基準の円から内側方向への枠の太さ
		/// @param outerThickness 基準の円から外側方向への枠の太さ
		/// @param color 枠の色
		/// @return *this
		const Circle& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		/// @brief 円の枠を描きます。
		/// @param innerThickness 基準の円から内側方向への枠の太さ
		/// @param outerThickness 基準の円から外側方向への枠の太さ
		/// @param innerColor 内側部分の枠の色
		/// @param outerColor 外側部分の枠の色
		/// @return *this
		const Circle& drawFrame(double innerThickness, double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawPie
		//
		////////////////////////////////////////////////////////////////

		/// @brief 扇形を描きます。
		/// @param startAngle 扇形の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 扇形の角度（ラジアン）
		/// @param color 扇形の色
		/// @return *this
		//const Circle& drawPie(double startAngle, double angle, const ColorF& color = Palette::White) const;

		/// @brief 扇形を描きます。
		/// @param startAngle 扇形の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 扇形の角度（ラジアン）
		/// @param innerColor 扇の中心部分の色
		/// @param outerColor 扇の外側部分の色
		/// @return *this
		//const Circle& drawPie(double startAngle, double angle, const ColorF& innerColor, const ColorF& outerColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawArc
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円の弧を描きます。
		/// @param startAngle 弧の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 弧の角度（ラジアン）
		/// @param innerThickness 基準の円から内側方向への弧の太さ
		/// @param outerThickness 基準の円から外側方向への弧の太さ
		/// @param color 弧の色
		/// @return *this
		//const Circle& drawArc(double startAngle, double angle, double innerThickness = 1.0, double outerThickness = 0.0, const ColorF& color = Palette::White) const;

		/// @brief 円の弧を描きます。
		/// @param startAngle 弧の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 弧の角度（ラジアン）
		/// @param innerThickness 基準の円から内側方向への弧の太さ
		/// @param outerThickness 基準の円から外側方向への弧の太さ
		/// @param innerColor 内側部分の弧の色
		/// @param outerColor 外側部分の弧の色
		/// @return *this
		//const Circle& drawArc(double startAngle, double angle, double innerThickness, double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 円の弧を描きます。
		/// @param style 線のスタイル
		/// @param startAngle 弧の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 弧の角度（ラジアン）
		/// @param innerThickness 基準の円から内側方向への弧の太さ
		/// @param outerThickness 基準の円から外側方向への弧の太さ
		/// @param color 弧の色
		/// @return *this
		//const Circle& drawArc(const LineStyle& style, double startAngle, double angle, double innerThickness = 1.0, double outerThickness = 0.0, const ColorF& color = Palette::White) const;

		/// @brief 円の弧を描きます。
		/// @param style 線のスタイル
		/// @param startAngle 弧の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 弧の角度（ラジアン）
		/// @param innerThickness 基準の円から内側方向への弧の太さ
		/// @param outerThickness 基準の円から外側方向への弧の太さ
		/// @param innerColor 内側部分の弧の色
		/// @param outerColor 外側部分の弧の色
		/// @return *this
		//const Circle& drawArc(const LineStyle& style, double startAngle, double angle, double innerThickness, double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawSegment
		//
		////////////////////////////////////////////////////////////////

		/// @brief 弓形を描きます。
		/// @param arcMidpointAngle 弓形の弧の中心を円の中心から見た角度（ラジアン, 0 時の方向から時計回り）
		/// @param height 弓形の高さ
		/// @param color 色
		/// @return *this
		//const Circle& drawSegment(double arcMidpointAngle, double height, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawSegmentFromAngles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 弓形を描きます。
		/// @param startAngle 弓形の弧の開始角度（ラジアン, 0 時の方向から時計回り）
		/// @param angle 弓形の弧の角度（ラジアン）
		/// @param color 色
		/// @return *this
		//const Circle& drawSegmentFromAngles(double startAngle, double angle, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawShadow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円の影を描きます。
		/// @param offset 影の位置のオフセット
		/// @param blur ぼかしの大きさ（ピクセル）
		/// @param spread 影の膨張（ピクセル）
		/// @param color 影の色
		/// @return *this
		//const Circle& drawShadow(const Vec2& offset, double blur, double spread = 0.0, const ColorF& color = ColorF{ 0.0, 0.5 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//TexturedCircle operator ()(const Texture& texture) const;

		//[[nodiscard]]
		//TexturedCircle operator ()(const TextureRegion& textureRegion) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Circle& value)
		{
			return output << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.r << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Circle& value)
		{
			CharType unused;
			return input >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.r >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Circle& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Circle>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Circle& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Circle, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Circle& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Circle>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Circle& value) const noexcept
	{
		return value.hash();
	}
};
