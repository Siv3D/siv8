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
# include "Circular.hpp"
# include "ColorHSV.hpp"
# include "Array.hpp"
# include "Optional.hpp"
# include "PredefinedNamedParameter.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	enum class LineCap : uint8;
	struct LineStyle;

	////////////////////////////////////////////////////////////////
	//
	//	Line
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線分
	struct Line
	{
		/// @brief 線分の座標を表現する型
		using position_type	= Vec2;

		/// @brief 始点から終点までの移動量を表現する型
		using vector_type	= position_type;

		/// @brief 線分の座標の成分の型
		using value_type	= position_type::value_type;

		/// @brief 線分の開始位置
		position_type start;

		/// @brief 線分の終点位置
		position_type end;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Line() = default;

		/// @brief 線分を作成します。
		/// @param startX 始点の X 座標
		/// @param startY 始点の Y 座標
		/// @param endX 終点の X 座標
		/// @param endY 終点の Y 座標
		[[nodiscard]]
		constexpr Line(value_type startX, value_type startY, value_type endX, value_type endY) noexcept;

		/// @brief 線分を作成します。
		/// @param startX 始点の X 座標
		/// @param startY 始点の Y 座標
		/// @param endX 終点の X 座標
		/// @param endY 終点の Y 座標
		[[nodiscard]]
		constexpr Line(Concept::Arithmetic auto startX, Concept::Arithmetic auto startY, Concept::Arithmetic auto endX, Concept::Arithmetic auto endY) noexcept;

		/// @brief 線分を作成します。
		/// @param _start 始点の座標
		/// @param endX 終点の X 座標
		/// @param endY 終点の Y 座標
		[[nodiscard]]
		constexpr Line(position_type _start, value_type endX, value_type endY) noexcept;

		/// @brief 線分を作成します。
		/// @param _start 始点の座標
		/// @param endX 終点の X 座標
		/// @param endY 終点の Y 座標
		[[nodiscard]]
		constexpr Line(position_type _start, Concept::Arithmetic auto endX, Concept::Arithmetic auto endY) noexcept;

		/// @brief 線分を作成します。
		/// @param startX 始点の X 座標
		/// @param startY 始点の Y 座標
		/// @param _end 終点の座標
		[[nodiscard]]
		constexpr Line(value_type startX, value_type startY, position_type _end) noexcept;

		/// @brief 線分を作成します。
		/// @param startX 始点の X 座標
		/// @param startY 始点の Y 座標
		/// @param _end 終点の座標
		[[nodiscard]]
		constexpr Line(Concept::Arithmetic auto startX, Concept::Arithmetic auto startY, position_type _end) noexcept;

		/// @brief 線分を作成します。
		/// @param _start 始点の座標
		/// @param _end 終点の座標
		[[nodiscard]]
		constexpr Line(position_type _start, position_type _end) noexcept;

		/// @brief 線分を作成します。
		/// @param originX 始点の X 座標
		/// @param originY 始点の Y 座標
		/// @param vector 始点から終点までの移動量
		[[nodiscard]]
		constexpr Line(value_type originX, value_type originY, Arg::direction_<vector_type> vector) noexcept;

		/// @brief 線分を作成します。
		/// @param originX 始点の X 座標
		/// @param originY 始点の Y 座標
		/// @param vector 始点から終点までの移動量
		[[nodiscard]]
		constexpr Line(Concept::Arithmetic auto originX, Concept::Arithmetic auto originY, Arg::direction_<vector_type> vector) noexcept;

		/// @brief 線分を作成します。
		/// @param origin 始点の座標
		/// @param vector 始点から終点までの移動量
		[[nodiscard]]
		constexpr Line(position_type origin, Arg::direction_<vector_type> vector) noexcept;

		/// @brief 線分を作成します。
		/// @param originX 始点の X 座標
		/// @param originY 始点の Y 座標
		/// @param angle 始点から見た終点の方向（ラジアン）
		/// @param length 線分の長さ
		[[nodiscard]]
		Line(value_type originX, value_type originY, Arg::angle_<value_type> angle, value_type length) noexcept;

		/// @brief 線分を作成します。
		/// @param originX 始点の X 座標
		/// @param originY 始点の Y 座標
		/// @param angle 始点から見た終点の方向（ラジアン）
		/// @param length 線分の長さ
		[[nodiscard]]
		Line(value_type originX, value_type originY, Arg::angle_<value_type> angle, Concept::Arithmetic auto length) noexcept;

		/// @brief 線分を作成します。
		/// @param origin 始点の座標
		/// @param angle 始点から見た終点の方向（ラジアン）
		/// @param length 線分の長さ
		[[nodiscard]]
		Line(position_type origin, Arg::angle_<value_type> angle, value_type length) noexcept;

		/// @brief 線分を作成します。
		/// @param origin 始点の座標
		/// @param angle 始点から見た終点の方向（ラジアン）
		/// @param length 線分の長さ
		[[nodiscard]]
		Line(position_type origin, Arg::angle_<value_type> angle, Concept::Arithmetic auto length) noexcept;

		/// @brief 線分を作成します。
		/// @param center 線分の中心座標
		/// @param vector 始点から終点までの移動量
		[[nodiscard]]
		constexpr Line(Arg::center_<position_type> center, vector_type vector) noexcept;

		/// @brief 線分を作成します。
		/// @param center 線分の中心座標
		/// @param angle 始点から見た終点の方向（ラジアン）
		/// @param length 線分の長さ
		[[nodiscard]]
		Line(Arg::center_<position_type> center, Arg::angle_<value_type> angle, value_type length) noexcept;

		/// @brief 線分を作成します。
		/// @param center 線分の中心座標
		/// @param angle 始点から見た終点の方向（ラジアン）
		/// @param length 線分の長さ
		[[nodiscard]]
		Line(Arg::center_<position_type> center, Arg::angle_<value_type> angle, Concept::Arithmetic auto length) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Line が等しいかを返します。
		/// @param lhs 一方の Line
		/// @param rhs もう一方の Line
		/// @return 2 つの Line が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Line lhs, const Line rhs) noexcept
		{
			return ((lhs.start == rhs.start)
				 && (lhs.end == rhs.end));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withStart
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点を変更した新しい線分を返します。
		/// @param startX 新しい始点の X 座標
		/// @param startY 新しい始点の Y 座標
		/// @return 始点を変更した新しい線分
		[[nodiscard]]
		constexpr Line withStart(value_type startX, value_type startY) const noexcept;

		/// @brief 始点を変更した新しい線分を返します。
		/// @param _start 新しい始点の座標
		/// @return 始点を変更した新しい線分
		[[nodiscard]]
		constexpr Line withStart(position_type _start) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点を変更した新しい線分を返します。
		/// @param endX 新しい終点の X 座標
		/// @param endY 新しい終点の Y 座標
		/// @return 終点を変更した新しい線分
		[[nodiscard]]
		constexpr Line withEnd(value_type endX, value_type endY) const noexcept;

		/// @brief 終点を変更した新しい線分を返します。
		/// @param _end 新しい終点の座標
		/// @return 終点を変更した新しい線分
		[[nodiscard]]
		constexpr Line withEnd(position_type _end) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を変更した新しい線分を返します。
		/// @param x 新しい中心座標の X 座標
		/// @param y 新しい中心座標の Y 座標
		/// @return 中心座標を変更した新しい線分
		[[nodiscard]]
		constexpr Line withCenter(value_type x, value_type y) const noexcept;

		/// @brief 中心座標を変更した新しい線分を返します。
		/// @param _center 新しい中心座標
		/// @return 中心座標を変更した新しい線分
		[[nodiscard]]
		constexpr Line withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setStart
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点を変更します。
		/// @param startX 新しい始点の X 座標
		/// @param startY 新しい始点の Y 座標
		/// @return *this
		constexpr Line& setStart(value_type startX, value_type startY) noexcept;

		/// @brief 始点を変更します。
		/// @param _start 新しい始点の座標
		/// @return *this
		constexpr Line& setStart(position_type _start) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点を変更します。
		/// @param endX 新しい終点の X 座標
		/// @param endY 新しい終点の Y 座標
		/// @return *this
		constexpr Line& setEnd(value_type endX, value_type endY) noexcept;

		/// @brief 終点を変更します。
		/// @param _end 新しい終点の座標
		/// @return *this
		constexpr Line& setEnd(position_type _end) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を変更します。
		/// @param x 新しい中心座標の X 座標
		/// @param y 新しい中心座標の Y 座標
		/// @return *this
		constexpr Line& setCenter(value_type x, value_type y) noexcept;

		/// @brief 中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		constexpr Line& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分を変更します。
		/// @param startX 新しい始点の X 座標
		/// @param startY 新しい始点の Y 座標
		/// @param endX 新しい終点の X 座標
		/// @param endY 新しい終点の Y 座標
		/// @return *this
		constexpr Line& set(value_type startX, value_type startY, value_type endX, value_type endY) noexcept;

		/// @brief 線分を変更します。
		/// @param _start 新しい始点の座標
		/// @param endX 新しい終点の X 座標
		/// @param endY 新しい終点の Y 座標
		/// @return *this
		constexpr Line& set(position_type _start, value_type endX, value_type endY) noexcept;

		/// @brief 線分を変更します。
		/// @param startX 新しい始点の X 座標
		/// @param startY 新しい始点の Y 座標
		/// @param _end 新しい終点の座標
		/// @return *this
		constexpr Line& set(value_type startX, value_type startY, position_type _end) noexcept;

		/// @brief 線分を変更します。
		/// @param _start 新しい始点の座標
		/// @param _end 新しい終点の座標
		/// @return *this
		constexpr Line& set(position_type _start, position_type _end) noexcept;

		/// @brief 線分を変更します。
		/// @param line 新しい線分
		/// @return *this
		constexpr Line& set(const Line& line) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標を移動した新しい線分を返します。
		/// @param x X 軸方向の移動量
		/// @param y Y 軸方向の移動量
		/// @return 座標を移動した新しい線分
		[[nodiscard]]
		constexpr Line movedBy(value_type x, value_type y) const noexcept;

		/// @brief 座標を移動した新しい線分を返します。
		/// @param v 移動量
		/// @return 座標を移動した新しい線分
		[[nodiscard]]
		constexpr Line movedBy(vector_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分を移動させます。
		/// @param x X 軸方向の移動量
		/// @param y Y 軸方向の移動量
		/// @return *this
		constexpr Line& moveBy(value_type x, value_type y) noexcept;

		/// @brief 線分を移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr Line& moveBy(vector_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分を伸縮した新しい線分を返します。
		/// @param extentionBothSides 両側に伸縮する量（合わせて、この 2 倍の長さが追加されます）
		/// @return 線分を伸縮した新しい線分
		[[nodiscard]]
		Line stretched(value_type extentionBothSides) const noexcept;

		/// @brief 線分を伸縮した新しい線分を返します。
		/// @param extentionStart 始点側に伸縮する量
		/// @param extentionEnd 終点側に伸縮する量
		/// @return 線分を伸縮した新しい線分
		[[nodiscard]]
		Line stretched(value_type extentionStart, value_type extentionEnd) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromStart
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点を固定して線分を拡大縮小した新しい線分を返します。
		/// @param scale 拡大倍率
		/// @return 始点を固定して線分を拡大縮小した新しい線分
		[[nodiscard]]
		Line scaledFromStart(value_type scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点を固定して線分を拡大縮小した新しい線分を返します。
		/// @param scale 拡大倍率
		/// @return 終点を固定して線分を拡大縮小した新しい線分
		[[nodiscard]]
		Line scaledFromEnd(value_type scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心を固定して線分を拡大縮小した新しい線分を返します。
		/// @param scale 拡大倍率
		/// @return 中心を固定して線分を拡大縮小した新しい線分
		[[nodiscard]]
		Line scaledFromCenter(value_type scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAtStart
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点を固定して時計回りに回転させた新しい線分を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 始点を固定して回転させた新しい線分
		[[nodiscard]]
		Line rotatedAtStart(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotateAtStart
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 始点を固定して時計回りに回転させます。
		/// @param angle 回転角度（ラジアン）
		/// @return *this
		Line& rotateAtStart(double angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAtEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点を固定して時計回りに回転させた新しい線分を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 終点を固定して回転させた新しい線分
		[[nodiscard]]
		Line rotatedAtEnd(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotateAtEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点を固定して時計回りに回転させます。
		/// @param angle 回転角度（ラジアン）
		/// @return *this
		Line& rotateAtEnd(double angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAtCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心を固定して時計回りに回転させた新しい線分を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 中心を固定して回転させた新しい線分
		[[nodiscard]]
		Line rotatedAtCenter(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotateAtCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心を固定して時計回りに回転させます。
		/// @param angle 回転角度（ラジアン）
		/// @return *this
		Line& rotateAtCenter(double angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vector
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点から終点までの移動量を返します。
		/// @return 始点から終点までの移動量
		[[nodiscard]]
		constexpr vector_type vector() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalizedVector
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分の方向（始点から終点へのベクトル）を正規化したベクトルを返します。
		/// @return 線分の方向を表す単位ベクトル。線分の長さが 0 の場合はゼロベクトル
		[[nodiscard]]
		vector_type normalizedVector() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perpendicularUnitVector
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分の方向（始点から終点へのベクトル）に直交する単位ベクトルを返します。
		/// @return 線分の方向に直交する単位ベクトル。線分の長さが 0 の場合はゼロベクトル
		[[nodiscard]]
		vector_type perpendicularUnitVector() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点と終点を入れ替えた新しい線分を返します。
		/// @return 始点と終点を入れ替えた新しい線分
		[[nodiscard]]
		constexpr Line reversed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reverse
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点と終点を入れ替えます。
		/// @return *this
		constexpr Line& reverse() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分が 0 より大きい長さを持つかを返します。
		/// @return 線分が 0 より大きい長さを持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分の長さを返します。
		/// @return 線分の長さ
		[[nodiscard]]
		value_type length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lengthSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分の長さの 2 乗を返します。
		/// @return 線分の長さの 2 乗
		[[nodiscard]]
		constexpr value_type lengthSq() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの頂点座標の参照を返します
		/// @param index インデックス（0: 始点, 1: 終点）
		/// @return 指定したインデックスの頂点座標の参照
		/// @throw std::out_of_range index が 0 または 1 でない場合
		[[nodiscard]]
		position_type& pointAtIndex(size_t index) noexcept;

		/// @brief 指定したインデックスの頂点座標の参照を返します
		/// @param index インデックス（0: 始点, 1: 終点）
		/// @return 指定したインデックスの頂点座標の参照
		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分（またはその延長線）上で始点から指定した長さの距離にある座標を返します。
		/// @param length 始点からの距離
		/// @return 線分（またはその延長線）上で始点から指定した長さの距離にある座標
		/// @remark 線分の長さが 0 の場合は始点の座標を返します。
		[[nodiscard]]
		position_type pointAtLength(double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	interpolatedPointAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分の長さを 1 として、線分（またはその延長線）上で指定した位置に対応する座標を返します。
		/// @param t 位置（始点から、線分の長さを 1 とした場合の位置）
		/// @return 線分（またはその延長線）上で指定した位置に対応する座標
		[[nodiscard]]
		constexpr position_type interpolatedPointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	center
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分の中心座標を返します。
		/// @return 線分の中心座標
		[[nodiscard]]
		constexpr position_type center() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	closestPointTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標に最も近い線分上の座標を返します。
		/// @param pos 座標
		/// @return 指定した座標に最も近い線分上の座標
		[[nodiscard]]
		position_type closestPointTo(position_type pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分を囲む最小の長方形を返します。
		/// @return 線分を囲む最小の長方形
		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の線分との線形補間を返します。
		/// @param other 別の線分
		/// @param f 補間係数
		/// @return 線形補間された線分
		[[nodiscard]]
		constexpr Line lerp(const Line& other, double f) const noexcept;

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

		/// @brief 別の線分との交点を返します。
		/// @param other 別の線分
		/// @return 交差しない場合は none, 交差する場合はその座標、2 つの線分が重なっている場合 (QNaN, QNaN)
		[[nodiscard]]
		Optional<position_type> intersectsAt(const Line& other) const;

		//template <class Shape2DType>
		//[[nodiscard]]
		//Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersectsAtPrecise
		//
		////////////////////////////////////////////////////////////////

		///// @brief 別の線分 other との交点を返します。`intersectsAt()` と異なり、両者の順序が異なっても一致する結果を返します。
		///// @param other 別の線分
		///// @return 交差しない場合は none, 交差する場合はその座標、2 つの線分が重なっている場合 (QNaN, QNaN)
		//[[nodiscard]]
		//Optional<position_type> intersectsAtPrecise(const Line& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		//const Line& paint(Image& dst, const Color& color) const;

		//const Line& paint(Image& dst, int32 thickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const Line& overwrite(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		//const Line& overwrite(Image& dst, int32 thickness, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintArrow
		//
		////////////////////////////////////////////////////////////////

		//const Line& paintArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteArrow
		//
		////////////////////////////////////////////////////////////////

		//const Line& overwriteArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintDoubleHeadedArrow
		//
		////////////////////////////////////////////////////////////////

		//const Line& paintDoubleHeadedArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteDoubleHeadedArrow
		//
		////////////////////////////////////////////////////////////////

		//const Line& overwriteDoubleHeadedArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 線分を描きます。
		/// @param color 色
		/// @return *this
		const Line& draw(const ColorF& color = Palette::White) const;

		/// @brief 線分を描きます。
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& draw(const ColorF& colorBegin, const ColorF& colorEnd) const;

		/// @brief 線分を描きます。
		/// @param thickness 線分の太さ
		/// @param color 色
		/// @return *this
		const Line& draw(double thickness, const ColorF& color = Palette::White) const;

		/// @brief 線分を描きます。
		/// @param thickness 線分の太さ
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& draw(double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		/// @brief 線分を描きます。
		/// @param startCap 始点側の形状
		/// @param endCap 終点側の形状
		/// @param thickness 線分の太さ
		/// @param color 色
		/// @return *this
		const Line& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White) const;

		/// @brief 線分を描きます。
		/// @param startCap 始点側の形状
		/// @param endCap 終点側の形状
		/// @param thickness 線分の太さ
		/// @param colorBegin 
		/// @param colorEnd 
		/// @return 
		const Line& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		/// @brief 線分を描きます。
		/// @param style 線のスタイル
		/// @param thickness 線分の太さ
		/// @param color 色
		/// @return *this
		const Line& draw(const LineStyle& style, double thickness, const ColorF& color = Palette::White) const;

		/// @brief 線分を描きます。
		/// @param style 線のスタイル
		/// @param thickness 線分の太さ
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& draw(const LineStyle& style, double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawUncapped
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点や終点をキャップせずに線分を描きます。
		/// @param color 色
		/// @return *this
		const Line& drawUncapped(const ColorF& color = Palette::White) const;

		/// @brief 始点や終点をキャップせずに線分を描きます。
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& drawUncapped(const ColorF& colorBegin, const ColorF& colorEnd) const;

		/// @brief 始点や終点をキャップせずに線分を描きます。
		/// @param thickness 線分の太さ
		/// @param color 色
		/// @return *this
		const Line& drawUncapped(double thickness, const ColorF& color = Palette::White) const;

		/// @brief 始点や終点をキャップせずに線分を描きます。
		/// @param thickness 線分の太さ
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& drawUncapped(double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawRounded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点と終点が丸い線分を描きます。
		/// @param color 色
		/// @return *this
		const Line& drawRounded(const ColorF& color = Palette::White) const;

		/// @brief 始点と終点が丸い線分を描きます。
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& drawRounded(const ColorF& colorBegin, const ColorF& colorEnd) const;

		/// @brief 始点と終点が丸い線分を描きます。
		/// @param thickness 線分の太さ
		/// @param color 色
		/// @return *this
		const Line& drawRounded(double thickness, const ColorF& color = Palette::White) const;

		/// @brief 始点と終点が丸い線分を描きます。
		/// @param thickness 線分の太さ
		/// @param colorBegin 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const Line& drawRounded(double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawArrow
		//
		////////////////////////////////////////////////////////////////

		///// @brief 線分をもとに矢印を描きます。
		///// @param width 矢印の線の幅
		///// @param headSize 矢印の三角形のサイズ
		///// @param color 色
		///// @return *this
		//const Line& drawArrow(double width = 1.0, const SizeF& headSize = SizeF{ 5.0, 5.0 }, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawDoubleHeadedArrow
		//
		////////////////////////////////////////////////////////////////

		///// @brief 線分をもとに両方向矢印を描きます。
		///// @param width 矢印の線の幅
		///// @param headSize 矢印の三角形のサイズ
		///// @param color 色
		///// @return *this
		//const Line& drawDoubleHeadedArrow(double width = 1.0, const SizeF& headSize = SizeF{ 5.0, 5.0 }, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Line の内容を出力します。
		/// @tparam CharType ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Line
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Line& value)
		{
			return output << CharType('(')
				<< value.start << CharType(',') << CharType(' ')
				<< value.end << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Line の内容を読み込みます。
		/// @tparam CharType ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Line の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Line& value)
		{
			CharType unused;
			return input >> unused
				>> value.start >> unused
				>> value.end >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Line を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Rect
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Line& value);

	private:

		[[noreturn]]
		static void ThrowPointAtIndexOutOfRange();
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Line>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Line& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Line, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Line& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Line>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Line& value) const noexcept
	{
		return value.hash();
	}
};
