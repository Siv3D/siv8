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
	struct PatternParameters;

	/// @brief 角丸長方形
	struct RoundRect
	{
		/// @brief 角丸長方形の座標を表現する型
		using position_type	= RectF::position_type;

		/// @brief 角丸長方形のサイズを表現する型
		using size_type		= RectF::size_type;

		/// @brief 角丸長方形の座標やサイズの成分の型
		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 基本の長方形
			RectF rect;

			struct
			{
				/// @brief 基本の長方形の左上の点の X 座標
				value_type x;

				/// @brief 基本の長方形の左上の点の Y 座標
				value_type y;

				/// @brief 基本の長方形の幅
				value_type w;

				/// @brief 基本の長方形の高さ
				value_type h;
			};
		};

		/// @brief 角丸の半径
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

		/// @brief 角丸長方形を作成します。
		/// @param _x 基本の長方形の左上の X 座標
		/// @param _y 基本の長方形の左上の Y 座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(value_type _x, value_type _y, value_type _w, value_type _h, value_type _r) noexcept;
		
		/// @brief 角丸長方形を作成します。
		/// @param _x 基本の長方形の左上の X 座標
		/// @param _y 基本の長方形の左上の Y 座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param pos 基本の長方形の左上の座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(position_type pos, value_type _w, value_type _h, value_type _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param pos 基本の長方形の左上の座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(position_type pos, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _x 基本の長方形の左上の X 座標
		/// @param _y 基本の長方形の左上の Y 座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(value_type _x, value_type _y, size_type size, value_type _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _x 基本の長方形の左上の X 座標
		/// @param _y 基本の長方形の左上の Y 座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, size_type size, Concept::Arithmetic auto _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param pos 基本の長方形の左上の座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(position_type pos, size_type size, value_type _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param pos 基本の長方形の左上の座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(position_type pos, size_type size, Concept::Arithmetic auto _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _center 基本の長方形の中心座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, value_type _w, value_type _h, value_type _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _center 基本の長方形の中心座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _center 基本の長方形の中心座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, size_type size, value_type _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _center 基本の長方形の中心座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, size_type size, Concept::Arithmetic auto _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _rect 基本の長方形
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(const RectF& _rect, value_type _r) noexcept;

		/// @brief 角丸長方形を作成します。
		/// @param _rect 基本の長方形
		/// @param _r 角丸の半径
		[[nodiscard]]
		constexpr RoundRect(const RectF& _rect, Concept::Arithmetic auto _r) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの RoundRect が等しいかを返します。
		/// @param lhs 一方の RoundRect
		/// @param rhs もう一方の RoundRect
		/// @return 2 つの RoundRect が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const RoundRect& lhs, const RoundRect& rhs) noexcept
		{
			return ((lhs.rect == rhs.rect)
				&& (lhs.r == rhs.r));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withX
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 座標を変更した新しい角丸長方形を返します。
		/// @param _x 新しい X 座標
		/// @return X 座標を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withX(value_type _x) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withY
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 座標を変更した新しい角丸長方形を返します。
		/// @param _y 新しい Y 座標
		/// @return Y 座標を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上の座標を変更した新しい角丸長方形を返します。
		/// @param _x 新しい左上の X 座標
		/// @param _y 新しい左上の Y 座標
		/// @return 左上の座標を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withPos(value_type _x, value_type _y) const noexcept;

		/// @brief 左上の座標を変更した新しい角丸長方形を返します。
		/// @param _pos 新しい左上の座標
		/// @return 左上の座標を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withPos(position_type _pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withW
		//
		////////////////////////////////////////////////////////////////

		/// @brief 幅を変更した新しい角丸長方形を返します。
		/// @param _w 新しい幅
		/// @return 幅を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withW(value_type _w) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withH
		//
		////////////////////////////////////////////////////////////////

		/// @brief 高さを変更した新しい角丸長方形を返します。
		/// @param _h 新しい高さ
		/// @return 高さを変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withH(value_type _h) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief サイズを変更した新しい角丸長方形を返します。
		/// @param _w 新しい幅
		/// @param _h 新しい高さ
		/// @return サイズを変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withSize(value_type _w, value_type _h) const noexcept;

		/// @brief サイズを変更した新しい角丸長方形を返します。
		/// @param _size 新しいサイズ
		/// @return サイズを変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withSize(size_type _size) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を変更した新しい角丸長方形を返します。
		/// @param _cx 新しい中心の X 座標
		/// @param _cy 新しい中心の Y 座標
		/// @return 中心座標を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withCenter(value_type _cx, value_type _cy) const noexcept;

		/// @brief 中心座標を変更した新しい角丸長方形を返します。
		/// @param _center 新しい中心座標
		/// @return 中心座標を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withR
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸の半径を変更した新しい角丸長方形を返します。
		/// @param _r 新しい角丸の半径
		/// @return 角丸の半径を変更した新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect withR(value_type _r) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の左上の点の座標を変更します。
		/// @param _x 新しい左上の X 座標
		/// @param _y 新しい左上の Y 座標
		/// @return *this
		constexpr RoundRect& setPos(value_type _x, value_type _y) noexcept;

		/// @brief 角丸長方形の左上の点の座標を変更します。
		/// @param _pos 新しい左上の座標
		/// @return *this
		constexpr RoundRect& setPos(position_type _pos) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の中心座標を変更します。
		/// @param _x 新しい中心座標の X 座標
		/// @param _y 新しい中心座標の Y 座標
		/// @return *this
		constexpr RoundRect& setCenter(value_type _x, value_type _y) noexcept;

		/// @brief 角丸長方形の中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		constexpr RoundRect& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形のサイズを変更します。
		/// @param _size 新しい幅と高さ
		/// @return *this
		constexpr RoundRect& setSize(value_type _size) noexcept;

		/// @brief 角丸長方形のサイズを変更します。
		/// @param _w 新しい幅
		/// @param _h 新しい高さ
		/// @return *this
		constexpr RoundRect& setSize(value_type _w, value_type _h) noexcept;

		/// @brief 角丸長方形のサイズを変更します。
		/// @param _size 新しいサイズ
		/// @return *this
		constexpr RoundRect& setSize(size_type _size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setR
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の角丸の半径を変更します。
		/// @param _r 新しい角丸の半径
		/// @return *this
		constexpr RoundRect& setR(value_type _r) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の各要素を変更します。
		/// @param _x 基本の長方形の左上の X 座標
		/// @param _y 基本の長方形の左上の Y 座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		/// @return *this
		constexpr RoundRect& set(value_type _x, value_type _y, value_type _w, value_type _h, value_type _r) noexcept;

		/// @brief 角丸長方形の各要素を変更します。
		/// @param pos 基本の長方形の左上の座標
		/// @param _w 基本の長方形の幅
		/// @param _h 基本の長方形の高さ
		/// @param _r 角丸の半径
		/// @return *this
		constexpr RoundRect& set(position_type pos, value_type _w, value_type _h, value_type _r) noexcept;

		/// @brief 角丸長方形の各要素を変更します。
		/// @param _x 基本の長方形の左上の X 座標
		/// @param _y 基本の長方形の左上の Y 座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		/// @return *this
		constexpr RoundRect& set(value_type _x, value_type _y, size_type size, value_type _r) noexcept;

		/// @brief 角丸長方形の各要素を変更します。
		/// @param pos 基本の長方形の左上の座標
		/// @param size 基本の長方形のサイズ
		/// @param _r 角丸の半径
		/// @return *this
		constexpr RoundRect& set(position_type pos, size_type size, value_type _r) noexcept;

		/// @brief 角丸長方形の各要素を変更します。
		/// @param _rect 基本の長方形
		/// @param _r 角丸の半径
		/// @return *this
		constexpr RoundRect& set(const RectF& _rect, value_type _r) noexcept;

		/// @brief 角丸長方形の各要素を変更します。
		/// @param roundRect 新しい角丸長方形
		/// @return *this
		constexpr RoundRect& set(const RoundRect& roundRect) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標を移動した新しい角丸長方形を返します。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return 新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 座標を移動した新しい角丸長方形を返します。
		/// @param v 移動量
		/// @return 新しい角丸長方形
		[[nodiscard]]
		constexpr RoundRect movedBy(size_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形を移動させます。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return *this
		constexpr RoundRect& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 角丸長方形を移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr RoundRect& moveBy(size_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左右・上下方向に拡大縮小した角丸長方形を返します。
		/// @param xy 左方向・右方向・上方向・下方向のそれぞれの拡大縮小量
		/// @return 左右・上下方向に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect stretched(value_type xy) const noexcept;

		/// @brief 左右・上下方向に拡大縮小した角丸長方形を返します。
		/// @param _x 左方向と右方向のそれぞれの拡大縮小量
		/// @param _y 上方向と下方向のそれぞれの拡大縮小量
		/// @return 左右・上下方向に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect stretched(value_type _x, value_type _y) const noexcept;

		/// @brief 左右・上下方向に拡大縮小した角丸長方形を返します。
		/// @param xy 左方向と右方向・上方向と下方向のそれぞれの拡大縮小量
		/// @return 左右・上下方向に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect stretched(size_type xy) const noexcept;

		/// @brief 上下左右方向に拡大縮小した角丸長方形を返します。
		/// @param top 上方向の拡大縮小量
		/// @param right 右方向の拡大縮小量
		/// @param bottom 下方向の拡大縮小量
		/// @param left 左方向の拡大縮小量
		/// @return 上下左右方向に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect stretched(value_type top, value_type right, value_type bottom, value_type left) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心を固定して拡大縮小した角丸長方形を返します。
		/// @param s 拡大倍率
		/// @return 中心を固定して拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFromCenter(double s) const noexcept;

		/// @brief 中心を固定して拡大縮小した角丸長方形を返します。
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 中心を固定して拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFromCenter(double sx, double sy) const noexcept;

		/// @brief 中心を固定して拡大縮小した角丸長方形を返します。
		/// @param s X 軸方向と Y 軸方向のそれぞれの拡大倍率
		/// @return 中心を基準に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFromCenter(Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromTopLeft
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上を固定して拡大縮小した角丸長方形を返します。
		/// @param s 拡大倍率
		/// @return 左上を固定して拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFromTopLeft(double s) const noexcept;

		/// @brief 左上を固定して拡大縮小した角丸長方形を返します。
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 左上を固定して拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFromTopLeft(double sx, double sy) const noexcept;

		/// @brief 左上を固定して拡大縮小した角丸長方形を返します。
		/// @param s X 軸方向と Y 軸方向のそれぞれの拡大倍率
		/// @return 左上を固定して拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFromTopLeft(Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を起点に拡大縮小した角丸長方形を返します。
		/// @param _pos 拡大縮小の起点となる座標
		/// @param s 拡大倍率
		/// @return 指定した座標を起点に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFrom(Vec2 _pos, double s) const noexcept;

		/// @brief 指定した座標を起点に拡大縮小した角丸長方形を返します。
		/// @param _pos 拡大縮小の起点となる座標
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 指定した座標を起点に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFrom(Vec2 _pos, double sx, double sy) const noexcept;

		/// @brief 指定した座標を起点に拡大縮小した角丸長方形を返します。
		/// @param _pos 拡大縮小の起点となる座標
		/// @param s X 軸方向と Y 軸方向のそれぞれの拡大倍率
		/// @return 指定した座標を起点に拡大縮小した角丸長方形
		[[nodiscard]]
		constexpr RoundRect scaledFrom(Vec2 _pos, Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasArea
		//
		////////////////////////////////////////////////////////////////

		/// @brief 基本の長方形が大きさを持っているかを返します。
		/// @return 基本の長方形が大きさを持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasArea() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	leftX, rightX, topY, bottomY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 基本の長方形の左辺の X 座標を返します。
		/// @return 基本の長方形の左辺の X 座標
		[[nodiscard]]
		constexpr value_type leftX() const noexcept;

		/// @brief 基本の長方形の右辺の X 座標を返します。
		/// @return 基本の長方形の右辺の X 座標
		[[nodiscard]]
		constexpr value_type rightX() const noexcept;

		/// @brief 基本の長方形の上辺の Y 座標を返します。
		/// @return 基本の長方形の上辺の Y 座標
		[[nodiscard]]
		constexpr value_type topY() const noexcept;

		/// @brief 基本の長方形の下辺の Y 座標を返します。
		/// @return 基本の長方形の下辺の Y 座標
		[[nodiscard]]
		constexpr value_type bottomY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centerX, centerY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の中心の X 座標を返します。
		/// @return 角丸長方形の中心の X 座標
		[[nodiscard]]
		constexpr double centerX() const noexcept;

		/// @brief 角丸長方形の中心の Y 座標を返します。
		/// @return 角丸長方形の中心の Y 座標
		[[nodiscard]]
		constexpr double centerY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tl, tr, br, bl
		//
		////////////////////////////////////////////////////////////////

		/// @brief 基本の長方形の左上の座標を返します。
		/// @return 基本の長方形の左上の座標
		[[nodiscard]]
		constexpr position_type tl() const noexcept;

		/// @brief 基本の長方形の右上の座標を返します。
		/// @return 基本の長方形の右上の座標
		[[nodiscard]]
		constexpr position_type tr() const noexcept;

		/// @brief 基本の長方形の右下の座標を返します。
		/// @return 基本の長方形の右下の座標
		[[nodiscard]]
		constexpr position_type br() const noexcept;

		/// @brief 基本の長方形の左下の座標を返します。
		/// @return 基本の長方形の左下の座標
		[[nodiscard]]
		constexpr position_type bl() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cornerTl, cornerTr, cornerBr, cornerBl
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上の角丸曲線上の中心座標を返します。
		/// @return 左上の角丸曲線上の中心座標
		[[nodiscard]]
		constexpr Vec2 cornerTl() const noexcept;

		/// @brief 右上の角丸曲線上の中心座標を返します。
		/// @return 右上の角丸曲線上の中心座標
		[[nodiscard]]
		constexpr Vec2 cornerTr() const noexcept;

		/// @brief 右下の角丸曲線上の中心座標を返します。
		/// @return 右下の角丸曲線上の中心座標
		[[nodiscard]]
		constexpr Vec2 cornerBr() const noexcept;

		/// @brief 左下の角丸曲線上の中心座標を返します。
		/// @return 左下の角丸曲線上の中心座標
		[[nodiscard]]
		constexpr Vec2 cornerBl() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tlCenter, trCenter, brCenter, blCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上の角丸の中心座標を返します。
		/// @return 左上の角丸の中心座標
		[[nodiscard]]
		constexpr Vec2 tlCenter() const noexcept;
		
		/// @brief 右上の角丸の中心座標を返します。
		/// @return 右上の角丸の中心座標
		[[nodiscard]]
		constexpr Vec2 trCenter() const noexcept;

		/// @brief 右下の角丸の中心座標を返します。
		/// @return 右下の角丸の中心座標
		[[nodiscard]]
		constexpr Vec2 brCenter() const noexcept;
	
		/// @brief 左下の角丸の中心座標を返します。
		/// @return 左下の角丸の中心座標
		[[nodiscard]]
		constexpr Vec2 blCenter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tlCircle, trCircle, brCircle, blCircle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上の角丸に対応する円を返します。
		/// @return 左上の角丸に対応する円
		[[nodiscard]]
		constexpr Circle tlCircle() const noexcept;

		/// @brief 右上の角丸に対応する円を返します。
		/// @return 右上の角丸に対応する円
		[[nodiscard]]
		constexpr Circle trCircle() const noexcept;

		/// @brief 右下の角丸に対応する円を返します。
		/// @return 右下の角丸に対応する円
		[[nodiscard]]
		constexpr Circle brCircle() const noexcept;

		/// @brief 左下の角丸に対応する円を返します。
		/// @return 左下の角丸に対応する円
		[[nodiscard]]
		constexpr Circle blCircle() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	topCenter, rightCenter, bottomCenter, leftCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 基本の長方形の上辺の中心座標を返します。
		/// @return 基本の長方形の上辺の中心座標
		[[nodiscard]]
		constexpr Vec2 topCenter() const noexcept;

		/// @brief 基本の長方形の右辺の中心座標を返します。
		/// @return 基本の長方形の右辺の中心座標
		[[nodiscard]]
		constexpr Vec2 rightCenter() const noexcept;

		/// @brief 基本の長方形の下辺の中心座標を返します。
		/// @return 基本の長方形の下辺の中心座標
		[[nodiscard]]
		constexpr Vec2 bottomCenter() const noexcept;

		/// @brief 基本の長方形の左辺の中心座標を返します。
		/// @return 基本の長方形の左辺の中心座標
		[[nodiscard]]
		constexpr Vec2 leftCenter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	center
		//
		////////////////////////////////////////////////////////////////

		/// @brief 基本の長方形の中心座標を返します。
		/// @return 基本の長方形の中心座標
		[[nodiscard]]
		constexpr Vec2 center() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の周上の指定した距離に対応する座標を返します。
		/// @param length 距離（左上の角丸の終わりから時計回り）
		/// @return 角丸長方形の周上の指定した距離に対応する座標
		[[nodiscard]]
		Vec2 pointAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	interpolatedPointAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の周の長さを 1 として、周上の指定した位置に対応する座標を返します。
		/// @param t 位置（左上の角丸の終わりから時計回り、周の長さを 1 とした場合の位置）
		/// @return 角丸長方形の周上の指定した位置に対応する座標
		[[nodiscard]]
		Vec2 interpolatedPointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の面積を返します。
		/// @return 角丸長方形の面積
		[[nodiscard]]
		constexpr value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の周の長さを返します。
		/// @return 角丸長方形の周の長さ
		[[nodiscard]]
		constexpr value_type perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToTop
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、上に配置された角丸長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 上に角丸配置された長方形
		[[nodiscard]]
		constexpr RoundRect copiedToTop(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、右に配置された角丸長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 右に配置された角丸長方形
		[[nodiscard]]
		constexpr RoundRect copiedToRight(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToBottom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、下に配置された角丸長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 下に配置された角丸長方形
		[[nodiscard]]
		constexpr RoundRect copiedToBottom(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToLeft
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、左に配置された角丸長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 左に配置された角丸長方形
		[[nodiscard]]
		constexpr RoundRect copiedToLeft(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outline
		//
		////////////////////////////////////////////////////////////////

		///// @brief 角丸長方形の輪郭を LineString として返します。
		///// @param closeRing 頂点配列の終点を始点と重ねるかどうか
		///// @return 角丸長方形の輪郭の LineString
		//[[nodiscard]]
		//LineString outline(CloseRing closeRing = CloseRing::No) const;

		///// @brief 角丸長方形の輪郭の一部を LineString として返します。
		///// @param distanceFromOrigin 開始地点の距離（左上の角丸の終わりから時計回りでの距離）
		///// @param length 長さ
		///// @return 角丸長方形の輪郭の一部の LineString
		//[[nodiscard]]
		//LineString outline(double distanceFromOrigin, double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		///// @brief 角丸長方形を Polygon として返します。
		///// @return 角丸長方形の Polygon
		//[[nodiscard]]
		//Polygon asPolygon() const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の角丸長方形との線形補間を返します。
		/// @param other 別の角丸長方形
		/// @param f 補間係数
		/// @return 線形補間された角丸長方形
		[[nodiscard]]
		constexpr RoundRect lerp(const RoundRect& other, double f) const noexcept;

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
		//	paint
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	paintFrame
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	overwriteFrame
		//
		////////////////////////////////////////////////////////////////


		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形を描きます。
		/// @param color 色
		/// @return *this
		const RoundRect& draw(const ColorF& color = Palette::White) const;

		/// @brief 角丸長方形を描きます。
		/// @param topColor 上側の色
		/// @param bottomColor 下側の色
		/// @return *this
		const RoundRect& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 角丸長方形を描きます。
		/// @param leftColor 左側の色
		/// @param rightColor 右側の色
		/// @return *this
		const RoundRect& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 角丸長方形を描きます。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const RoundRect& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const RoundRect& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param topColor 上側の色
		/// @param bottomColor 下側の色
		/// @return *this
		const RoundRect& drawFrame(double thickness, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param leftColor 左側の色
		/// @param rightColor 右側の色
		/// @return *this
		const RoundRect& drawFrame(double thickness, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param innerThickness 基準の角丸長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の角丸長方形から外側方向への枠の太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const RoundRect& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param innerThickness 基準の角丸長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の角丸長方形から外側方向への枠の太さ（ピクセル）
		/// @param topColor 上側の色
		/// @param bottomColor 下側の色
		/// @return *this
		const RoundRect& drawFrame(double innerThickness, double outerThickness, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param innerThickness 基準の角丸長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の角丸長方形から外側方向への枠の太さ（ピクセル）
		/// @param leftColor 左側の色
		/// @param rightColor 右側の色
		/// @return *this
		const RoundRect& drawFrame(double innerThickness, double outerThickness, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const RoundRect& drawFrame(double thickness, const PatternParameters& pattern) const;

		/// @brief 角丸長方形の枠を描きます。
		/// @param innerThickness 基準の角丸長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の角丸長方形から外側方向への枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const RoundRect& drawFrame(double innerThickness, double outerThickness, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawShadow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角丸長方形の影を描画します。
		/// @param offset 影の位置のオフセット（ピクセル）
		/// @param blur ぼかしの大きさ（ピクセル）
		/// @param spread 影の膨張（ピクセル）
		/// @param color 影の色
		/// @param fill 影の内部を塗りつぶすか
		/// @return *this
		//const RoundRect& drawShadow(const Vec2& offset, double blur, double spread = 0.0, const ColorF& color = ColorF{ 0.0, 0.5 }, bool fill = true) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//TexturedRoundRect operator ()(const Texture& texture) const;

		//[[nodiscard]]
		//TexturedRoundRect operator ()(const TextureRegion& textureRegion) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに RoundRect の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value RoundRect
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const RoundRect& value)
		{
			return output << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.w << CharType(',') << CharType(' ')
				<< value.h << CharType(',') << CharType(' ')
				<< value.r << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから RoundRect の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value RoundRect の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, RoundRect& value)
		{
			CharType unused;
			return input >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.w >> unused
				>> value.h >> unused
				>> value.r >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief RoundRect を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value RectF
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const RoundRect& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::RoundRect>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::RoundRect& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::RoundRect, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::RoundRect& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::RoundRect>
{
	[[nodiscard]]
	size_t operator ()(const s3d::RoundRect& value) const noexcept
	{
		return value.hash();
	}
};
