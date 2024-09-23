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
		/// @brief 四角形の座標を表現する型
		using position_type	= Vec2;

		/// @brief 四角形の座標の成分の型
		using value_type	= position_type::value_type;

		/// @brief 四角形の点 0 の座標
		position_type p0;

		/// @brief 四角形の点 1 の座標
		position_type p1;

		/// @brief 四角形の点 2 の座標
		position_type p2;

		/// @brief 四角形の点 3 の座標
		position_type p3;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Quad() = default;

		/// @brief 四角形を作成します。
		/// @param x0 点 0 の X 座標
		/// @param y0 点 0 の Y 座標
		/// @param x1 点 1 の X 座標
		/// @param y1 点 1 の Y 座標
		/// @param x2 点 2 の X 座標
		/// @param y2 点 2 の Y 座標
		/// @param x3 点 3 の X 座標
		/// @param y3 点 3 の Y 座標
		[[nodiscard]]
		constexpr Quad(value_type x0, value_type y0, value_type x1, value_type y1, value_type x2, value_type y2, value_type x3, value_type y3) noexcept;

		/// @brief 四角形を作成します。
		/// @param x0 点 0 の X 座標
		/// @param y0 点 0 の Y 座標
		/// @param x1 点 1 の X 座標
		/// @param y1 点 1 の Y 座標
		/// @param x2 点 2 の X 座標
		/// @param y2 点 2 の Y 座標
		/// @param x3 点 3 の X 座標
		/// @param y3 点 3 の Y 座標
		[[nodiscard]]
		constexpr Quad(Concept::Arithmetic auto x0, Concept::Arithmetic auto y0, Concept::Arithmetic auto x1, Concept::Arithmetic auto y1, Concept::Arithmetic auto x2, Concept::Arithmetic auto y2, Concept::Arithmetic auto x3, Concept::Arithmetic auto y3) noexcept;

		/// @brief 四角形を作成します。
		/// @param _p0 点 0 の座標
		/// @param _p1 点 1 の座標
		/// @param _p2 点 2 の座標
		/// @param _p3 点 3 の座標
		[[nodiscard]]
		constexpr Quad(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Quad が等しいかを返します。
		/// @param lhs 一方の Quad
		/// @param rhs もう一方の Quad
		/// @return 2 つの Quad が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Quad& lhs, const Quad& rhs) noexcept
		{
			return ((lhs.p0 == rhs.p0)
				&& (lhs.p1 == rhs.p1)
				&& (lhs.p2 == rhs.p2)
				&& (lhs.p3 == rhs.p3));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withP0
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の点 0 の座標を変更した新しい四角形を返します。
		/// @param _x 新しい点 0 の X 座標
		/// @param _y 新しい点 0 の Y 座標
		/// @return 新しい点 0 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP0(value_type _x, value_type _y) const noexcept;

		/// @brief 四角形の点 0 の座標を変更した新しい四角形を返します。
		/// @param _p0 新しい点 0 の座標
		/// @return 新しい点 0 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP0(position_type _p0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の点 1 の座標を変更した新しい四角形を返します。
		/// @param _x 新しい点 1 の X 座標
		/// @param _y 新しい点 1 の Y 座標
		/// @return 新しい点 1 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP1(value_type _x, value_type _y) const noexcept;

		/// @brief 四角形の点 1 の座標を変更した新しい四角形を返します。
		/// @param _p1 新しい点 1 の座標
		/// @return 新しい点 1 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP1(position_type _p1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の点 2 の座標を変更した新しい四角形を返します。
		/// @param _x 新しい点 2 の X 座標
		/// @param _y 新しい点 2 の Y 座標
		/// @return 新しい点 2 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP2(value_type _x, value_type _y) const noexcept;

		/// @brief 四角形の点 2 の座標を変更した新しい四角形を返します。
		/// @param _p2 新しい点 2 の座標
		/// @return 新しい点 2 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP2(position_type _p2) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の点 3 の座標を変更した新しい四角形を返します。
		/// @param _x 新しい点 3 の X 座標
		/// @param _y 新しい点 3 の Y 座標
		/// @return 新しい点 3 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP3(value_type _x, value_type _y) const noexcept;

		/// @brief 四角形の点 3 の座標を変更した新しい四角形を返します。
		/// @param _p3 新しい点 3 の座標
		/// @return 新しい点 3 の座標を持つ新しい四角形
		[[nodiscard]]
		constexpr Quad withP3(position_type _p3) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の各頂点の座標を変更します。
		/// @param x0 点 0 の X 座標
		/// @param y0 点 0 の Y 座標
		/// @param x1 点 1 の X 座標
		/// @param y1 点 1 の Y 座標
		/// @param x2 点 2 の X 座標
		/// @param y2 点 2 の Y 座標
		/// @param x3 点 3 の X 座標
		/// @param y3 点 3 の Y 座標
		/// @return *this
		constexpr Quad& set(value_type x0, value_type y0, value_type x1, value_type y1, value_type x2, value_type y2, value_type x3, value_type y3) noexcept;

		/// @brief 四角形の各頂点の座標を変更します。
		/// @param _p0 点 0 の座標
		/// @param _p1 点 1 の座標
		/// @param _p2 点 2 の座標
		/// @param _p3 点 3 の座標
		/// @return *this
		constexpr Quad& set(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept;

		/// @brief 四角形を変更します。
		/// @param triangle 新しい四角形
		/// @return *this
		constexpr Quad& set(const Quad& triangle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形を平行移動した新しい四角形を返します。
		/// @param x X 方向の移動距離
		/// @param y Y 方向の移動距離
		/// @return 平行移動した新しい四角形
		[[nodiscard]]
		constexpr Quad movedBy(value_type x, value_type y) const noexcept;

		/// @brief 四角形を平行移動した新しい四角形を返します。
		/// @param v 移動距離
		/// @return 平行移動した新しい四角形
		[[nodiscard]]
		constexpr Quad movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形を平行移動します。
		/// @param x X 方向の移動距離
		/// @param y Y 方向の移動距離
		/// @return *this
		constexpr Quad& moveBy(value_type x, value_type y) noexcept;

		/// @brief 四角形を平行移動します。
		/// @param v 移動距離
		/// @return *this
		constexpr Quad& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 辺を外側に拡大または内側に縮小した新しい四角形を返します。
		/// @param size 拡大縮小の量（正の値で外側に拡大、負の値で内側に縮小）
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		Quad stretched(value_type size) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を中心に回転させた新しい四角形を返します。
		/// @param x 回転の中心の X 座標
		/// @param y 回転の中心の Y 座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転させた新しい四角形
		[[nodiscard]]
		Quad rotatedAt(value_type x, value_type y, value_type angle) const noexcept;

		/// @brief 指定した座標を中心に回転させた新しい四角形を返します。
		/// @param pos 回転の中心の座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転させた新しい四角形
		[[nodiscard]]
		Quad rotatedAt(position_type pos, value_type angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を基準に拡大縮小した新しい四角形を返します。
		/// @param s 拡大倍率
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		constexpr Quad scaledFromOrigin(value_type s) const noexcept;

		/// @brief 原点を基準に拡大縮小した新しい四角形を返します。
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		constexpr Quad scaledFromOrigin(value_type sx, value_type sy) const noexcept;

		/// @brief 原点を基準に拡大縮小した新しい四角形を返します。
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		constexpr Quad scaledFromOrigin(Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を基準に拡大縮小します。
		/// @param s 拡大倍率
		/// @return *this
		constexpr Quad& scaleFromOrigin(value_type s) noexcept;

		/// @brief 原点を基準に拡大縮小します。
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return *this
		constexpr Quad& scaleFromOrigin(value_type sx, value_type sy) noexcept;

		/// @brief 原点を基準に拡大縮小します。
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return *this
		constexpr Quad& scaleFromOrigin(Vec2 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を基準に拡大縮小した新しい四角形を返します。
		/// @param pos 基準位置
		/// @param s 拡大倍率
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		constexpr Quad scaledFrom(position_type pos, value_type s) const noexcept;

		/// @brief 指定した位置を基準に拡大縮小した新しい四角形を返します。
		/// @param pos 基準位置
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		constexpr Quad scaledFrom(position_type pos, value_type sx, value_type sy) const noexcept;

		/// @brief 指定した位置を基準に拡大縮小した新しい四角形を返します。
		/// @param pos 基準位置
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return 拡大縮小した新しい四角形
		[[nodiscard]]
		constexpr Quad scaledFrom(position_type pos, Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を基準に拡大縮小します。
		/// @param pos 基準位置
		/// @param s 拡大倍率
		/// @return *this
		constexpr Quad& scaleFrom(position_type pos, value_type s) noexcept;

		/// @brief 指定した位置を基準に拡大縮小します。
		/// @param pos 基準位置
		/// @param sx X 方向の拡大倍率
		/// @param sy Y 方向の拡大倍率
		/// @return *this
		constexpr Quad& scaleFrom(position_type pos, value_type sx, value_type sy) noexcept;

		/// @brief 指定した位置を基準に拡大縮小します。
		/// @param pos 基準位置
		/// @param s X 方向および Y 方向の拡大倍率
		/// @return *this
		constexpr Quad& scaleFrom(position_type pos, Vec2 s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isClockwise
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の頂点が時計回りであるかを返します。
		/// @return 頂点が時計回りである場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isClockwise() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isConvex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形が凸四角形であるかを返します。
		/// @return 四角形が凸四角形である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isConvex() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasArea
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形が面積を持つかを返します。
		/// @return 四角形が面積を持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasArea() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centroidX, centroidY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の重心の X 座標を返します。
		/// @return 四角形の重心の X 座標
		[[nodiscard]]
		constexpr value_type centroidX() const noexcept;

		/// @brief 四角形の重心の Y 座標を返します。
		/// @return 四角形の重心の Y 座標
		[[nodiscard]]
		constexpr value_type centroidY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centroid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の重心を返します。
		/// @return 四角形の重心
		[[nodiscard]]
		constexpr position_type centroid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		position_type& pointAtIndex(size_t index);

		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の周上の指定した距離に対応する座標を返します。
		/// @param length 距離（p0-p1-p2-p3 の順）
		/// @return 四角形の周上の指定した距離に対応する座標
		[[nodiscard]]
		Vec2 pointAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	interpolatedPointAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の周の長さを 1 として、周上の指定した位置に対応する座標を返します。
		/// @param t 位置（p0-p1-p2-p3 の順、周の長さを 1 とした場合の位置）
		/// @return 四角形の周上の指定した位置に対応する座標
		[[nodiscard]]
		Vec2 interpolatedPointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sideAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの辺を Line として返します。
		/// @param index インデックス（0 が p0-p1, 1 が p1-p2, 2 が p2-p3, 3 が p3-p0）
		/// @return 指定したインデックスの辺
		/// @throw std::out_of_range index が範囲外の場合
		[[nodiscard]]
		constexpr Line sideAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	sideIndexAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の周上の指定した距離に対応する辺のインデックスを返します。
		/// @param length 距離（p0-p1-p2 の順）
		/// @return 四角形の周上の指定した距離に対応する辺のインデックス（0 が p0-p1, 1 が p1-p2, 2 が p2-p0）
		[[nodiscard]]
		size_t sideIndexAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	triangleAtIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Triangle triangleAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の面積を返します。
		/// @return 四角形の面積
		[[nodiscard]]
		constexpr value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形の周の長さを返します。
		/// @return 四角形の周の長さ
		[[nodiscard]]
		value_type perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形を囲む最小の矩形を返します。
		/// @return 四角形を囲む最小の矩形
		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingCircle
		//
		////////////////////////////////////////////////////////////////

		///// @brief 四角形を囲む最小の円を返します。
		///// @return 四角形を囲む最小の円
		//[[nodiscard]]
		//Circle boundingCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	calculateBuffer
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon calculateBuffer(double distance) const;

		////////////////////////////////////////////////////////////////
		//
		//	calculateRoundBuffer
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon calculateRoundBuffer(double distance) const;

		////////////////////////////////////////////////////////////////
		//
		//	outline
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//LineString outline(CloseRing closeRing = CloseRing::No) const;

		//[[nodiscard]]
		//LineString outline(double distanceFromOrigin, double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon asPolygon() const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の四角形との線形補間を計算します。
		/// @param other 別の四角形
		/// @param f 補間係数
		/// @return 線形補間された四角形
		[[nodiscard]]
		constexpr Quad lerp(const Quad& other, double f) const noexcept;

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
		//	intersectsAt
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

		/// @brief 四角形が現在のフレームで左クリックされ始めたかを返します。
		/// @return 四角形が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 四角形が左クリックされているかを返します。
		/// @return 四角形が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで四角形への左クリックが離されたかを返します。
		/// @return 現在のフレームで四角形への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形が現在のフレームで右クリックされ始めたかを返します。
		/// @return 四角形が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 四角形が右クリックされているかを返します。
		/// @return 四角形が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで四角形への右クリックが離されたかを返します。
		/// @return 現在のフレームで四角形への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形上にマウスカーソルがあるかを返します。
		/// @return 四角形上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		//const Quad& paint(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const Quad& overwrite(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintFrame
		//
		////////////////////////////////////////////////////////////////

		//const Quad& paintFrame(Image& dst, int32 thickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteFrame
		//
		////////////////////////////////////////////////////////////////

		//const Quad& overwriteFrame(Image& dst, int32 thickness, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 四角形を描きます。
		/// @param color 四角形の色
		/// @return *this
		const Quad& draw(const ColorF& color = Palette::White) const;

		/// @brief 四角形を描きます。
		/// @param color0 頂点 0 の色
		/// @param color1 頂点 1 の色
		/// @param color2 頂点 2 の色
		/// @param color3 頂点 3 の色
		/// @return *this
		const Quad& draw(const ColorF& color0, const ColorF& color1, const ColorF& color2, const ColorF& color3) const;

		/// @brief 四角形を描きます。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Quad& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//TexturedQuad operator ()(const Texture& texture) const;

		//[[nodiscard]]
		//TexturedQuad operator ()(const TextureRegion& textureRegion) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Quad の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Quad
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Quad& value)
		{
			return output << CharType('(')
				<< value.p0 << CharType(',') << CharType(' ')
				<< value.p1 << CharType(',') << CharType(' ')
				<< value.p2 << CharType(',') << CharType(' ')
				<< value.p3 << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Quad の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Quad の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Quad& value)
		{
			CharType unused;
			return input >> unused
				>> value.p0 >> unused
				>> value.p1 >> unused
				>> value.p2 >> unused
				>> value.p3 >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Quad を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Quad
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Quad& value);

	private:

		[[noreturn]]
		static void ThrowPointAtIndexOutOfRange();

		[[noreturn]]
		static void ThrowSideAtIndexOutOfRange();

		[[noreturn]]
		static void ThrowTriangleAtIndexOutOfRange();
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Quad>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Quad& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Quad, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Quad& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Quad>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Quad& value) const noexcept
	{
		return value.hash();
	}
};
