//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
	struct FormatData;
	struct PatternParameters;
	enum class LineType : uint8;

	////////////////////////////////////////////////////////////////
	//
	//	RectF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 長方形 (成分が double 型)
	/// @remark
	///
	///  pos, tl()    top()     tr()
	///        +-----------------+
	///        |                 |
	/// left() |                 | right()
	///		   |                 |
	///        +-----------------+
	///       bl()   bottom()   br() 
	/// 
	struct RectF
	{
		/// @brief 長方形の座標を表現する型
		using position_type = Vec2;

		/// @brief 長方形のサイズを表現する型
		using size_type		= SizeF;

		/// @brief 長方形の座標やサイズの成分の型
		using value_type	= size_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 長方形の左上の点の位置
			position_type pos;

			struct
			{
				/// @brief 長方形の左上の点の X 座標
				value_type x;

				/// @brief 長方形の左上の点の Y 座標
				value_type y;
			};
		};

		union
		{
			/// @brief 長方形のサイズ
			size_type size;

			struct
			{
				/// @brief 長方形の幅
				/// @remark この値が負の時の挙動は未規定です。
				value_type w;

				/// @brief 長方形の高さ
				/// @remark この値が負の時の挙動は未規定です。
				value_type h;
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
		RectF() = default;
	
		/// @brief 長方形を作成します。
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		explicit constexpr RectF(value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		explicit constexpr RectF(Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		explicit constexpr RectF(Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		explicit constexpr RectF(size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(value_type _x, value_type _y, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _w 長方形の幅
		/// @param _h 	長方形の高さ
		[[nodiscard]]
		constexpr RectF(value_type _x, value_type _y, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(value_type _x, value_type _y, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(position_type _pos, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(position_type _pos, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(position_type _pos, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(position_type _pos, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(position_type _pos, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param rect 長方形
		[[nodiscard]]
		constexpr RectF(const Rect& rect) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::center_<position_type> _center, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::center_<position_type> _center, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::center_<position_type> _center, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::center_<position_type> _center, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::center_<position_type> _center, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 長方形の左上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topLeft_<position_type> topLeft, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 長方形の左上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topLeft_<position_type> topLeft, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 長方形の左上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::topLeft_<position_type> topLeft, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 長方形の左上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::topLeft_<position_type> topLeft, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 長方形の左上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topLeft_<position_type> topLeft, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 長方形の上辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topCenter_<position_type> topCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 長方形の上辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topCenter_<position_type> topCenter, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 長方形の上辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::topCenter_<position_type> topCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 長方形の上辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::topCenter_<position_type> topCenter, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 長方形の上辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topCenter_<position_type> topCenter, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 長方形の右上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topRight_<position_type> topRight, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 長方形の右上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topRight_<position_type> topRight, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 長方形の右上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::topRight_<position_type> topRight, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 長方形の右上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::topRight_<position_type> topRight, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 長方形の右上の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::topRight_<position_type> topRight, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 長方形の右辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::rightCenter_<position_type> rightCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 長方形の右辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::rightCenter_<position_type> rightCenter, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 長方形の右辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::rightCenter_<position_type> rightCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 長方形の右辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::rightCenter_<position_type> rightCenter, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 長方形の右辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::rightCenter_<position_type> rightCenter, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 長方形の右下の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomRight_<position_type> bottomRight, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 長方形の右下の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomRight_<position_type> bottomRight, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 長方形の右下の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomRight_<position_type> bottomRight, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 長方形の右下の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomRight_<position_type> bottomRight, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 長方形の右下の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomRight_<position_type> bottomRight, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 長方形の下辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomCenter_<position_type> bottomCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 長方形の下辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomCenter_<position_type> bottomCenter, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 長方形の下辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomCenter_<position_type> bottomCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 長方形の下辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomCenter_<position_type> bottomCenter, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 長方形の下辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomCenter_<position_type> bottomCenter, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 長方形の左下の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomLeft_<position_type> bottomLeft, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 長方形の左下の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomLeft_<position_type> bottomLeft, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 長方形の左下の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomLeft_<position_type> bottomLeft, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 長方形の左下の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomLeft_<position_type> bottomLeft, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 長方形の左下の点の座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::bottomLeft_<position_type> bottomLeft, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 長方形の左辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::leftCenter_<position_type> leftCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 長方形の左辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::leftCenter_<position_type> leftCenter, Concept::Arithmetic auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 長方形の左辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::leftCenter_<position_type> leftCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 長方形の左辺の中心座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr RectF(Arg::leftCenter_<position_type> leftCenter, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 長方形の左辺の中心座標
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		constexpr RectF(Arg::leftCenter_<position_type> leftCenter, size_type _size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの RectF が等しいかを返します。
		/// @param lhs 一方の RectF
		/// @param rhs もう一方の RectF
		/// @return 2 つの RectF が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const RectF& lhs, const RectF& rhs) noexcept
		{
			return ((lhs.pos == rhs.pos)
				 && (lhs.size == rhs.size));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withX
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 座標を変更した新しい長方形を返します。
		/// @param _x 新しい X 座標
		/// @return X 座標を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withX(value_type _x) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withY
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 座標を変更した新しい長方形を返します。
		/// @param _y 新しい Y 座標
		/// @return Y 座標を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上の座標を変更した新しい長方形を返します。
		/// @param _x 新しい左上の X 座標
		/// @param _y 新しい左上の Y 座標
		/// @return 左上の座標を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withPos(value_type _x, value_type _y) const noexcept;

		/// @brief 左上の座標を変更した新しい長方形を返します。
		/// @param _pos 新しい左上の座標
		/// @return 左上の座標を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withPos(position_type _pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withW
		//
		////////////////////////////////////////////////////////////////

		/// @brief 幅を変更した新しい長方形を返します。
		/// @param _w 新しい幅
		/// @return 幅を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withW(value_type _w) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withH
		//
		////////////////////////////////////////////////////////////////

		/// @brief 高さを変更した新しい長方形を返します。
		/// @param _h 新しい高さ
		/// @return 高さを変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withH(value_type _h) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief サイズを変更した新しい長方形を返します。
		/// @param _w 新しい幅
		/// @param _h 新しい高さ
		/// @return サイズを変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withSize(value_type _w, value_type _h) const noexcept;

		/// @brief サイズを変更した新しい長方形を返します。
		/// @param _size 新しいサイズ
		/// @return サイズを変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withSize(size_type _size) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標を変更した新しい長方形を返します。
		/// @param _cx 新しい中心の X 座標
		/// @param _cy 新しい中心の Y 座標
		/// @return 中心座標を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withCenter(value_type _cx, value_type _cy) const noexcept;

		/// @brief 中心座標を変更した新しい長方形を返します。
		/// @param _center 新しい中心座標
		/// @return 中心座標を変更した新しい長方形
		[[nodiscard]]
		constexpr RectF withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の左上の点の座標を変更します。
		/// @param _x 新しい左上の X 座標
		/// @param _y 新しい左上の Y 座標
		/// @return *this
		constexpr RectF& setPos(value_type _x, value_type _y) noexcept;

		/// @brief 長方形の左上の点の座標を変更します。
		/// @param _pos 新しい左上の座標
		/// @return *this
		constexpr RectF& setPos(position_type _pos) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の中心座標を変更します。
		/// @param _x 新しい中心座標の X 座標
		/// @param _y 新しい中心座標の Y 座標
		/// @return *this
		constexpr RectF& setCenter(value_type _x, value_type _y) noexcept;

		/// @brief 長方形の中心座標を変更します。
		/// @param _center 新しい中心座標
		/// @return *this
		constexpr RectF& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形のサイズを変更します。
		/// @param _size 新しい幅と高さ
		/// @return *this
		constexpr RectF& setSize(value_type _size) noexcept;

		/// @brief 長方形のサイズを変更します。
		/// @param _w 新しい幅
		/// @param _h 新しい高さ
		/// @return *this
		constexpr RectF& setSize(value_type _w, value_type _h) noexcept;

		/// @brief 長方形のサイズを変更します。
		/// @param _size 新しいサイズ
		/// @return *this
		constexpr RectF& setSize(size_type _size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @param _w 新しい幅
		/// @param _h 新しい高さ
		/// @return *this
		constexpr RectF& set(value_type _x, value_type _y, value_type _w, value_type _h) noexcept;

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @param _size 新しい幅と高さ
		/// @return *this
		constexpr RectF& set(value_type _x, value_type _y, value_type _size) noexcept;

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param _x 新しい X 座標
		/// @param _y 新しい Y 座標
		/// @param _size 新しいサイズ
		/// @return *this
		constexpr RectF& set(value_type _x, value_type _y, size_type _size) noexcept;

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param _pos 新しい座標
		/// @param _size 新しい幅と高さ
		/// @return *this
		constexpr RectF& set(position_type _pos, value_type _size) noexcept;

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param _pos 新しい座標
		/// @param _w 新しい幅
		/// @param _h 新しい高さ
		/// @return *this
		constexpr RectF& set(position_type _pos, value_type _w, value_type _h) noexcept;

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param _pos 新しい座標
		/// @param _size 新しいサイズ
		/// @return *this
		constexpr RectF& set(position_type _pos, size_type _size) noexcept;

		/// @brief 長方形の左上の座標とサイズを変更します。
		/// @param rect 新しい長方形
		/// @return *this
		constexpr RectF& set(const RectF& rect) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標を移動した新しい長方形を返します。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return 新しい長方形
		[[nodiscard]]
		constexpr RectF movedBy(value_type _x, value_type _y) const noexcept;

		/// @brief 座標を移動した新しい長方形を返します。
		/// @param v 移動量
		/// @return 新しい長方形
		[[nodiscard]]
		constexpr RectF movedBy(size_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を移動させます。
		/// @param _x X 軸方向の移動量
		/// @param _y Y 軸方向の移動量
		/// @return *this
		constexpr RectF& moveBy(value_type _x, value_type _y) noexcept;

		/// @brief 長方形を移動させます。
		/// @param v 移動量
		/// @return *this
		constexpr RectF& moveBy(size_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左右・上下方向に拡大縮小した長方形を返します。
		/// @param xy 左方向・右方向・上方向・下方向のそれぞれの拡大縮小量
		/// @return 左右・上下方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(value_type xy) const noexcept;

		/// @brief 左右・上下方向に拡大縮小した長方形を返します。
		/// @param _x 左方向と右方向のそれぞれの拡大縮小量
		/// @param _y 上方向と下方向のそれぞれの拡大縮小量
		/// @return 左右・上下方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(value_type _x, value_type _y) const noexcept;

		/// @brief 左右・上下方向に拡大縮小した長方形を返します。
		/// @param xy 左方向と右方向・上方向と下方向のそれぞれの拡大縮小量
		/// @return 左右・上下方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(size_type xy) const noexcept;

		/// @brief 上下左右方向に拡大縮小した長方形を返します。
		/// @param top 上方向の拡大縮小量
		/// @param right 右方向の拡大縮小量
		/// @param bottom 下方向の拡大縮小量
		/// @param left 左方向の拡大縮小量
		/// @return 上下左右方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(value_type top, value_type right, value_type bottom, value_type left) const noexcept;

		/// @brief 上方向に拡大縮小した長方形を返します。
		/// @param top 上方向の拡大縮小量
		/// @return 上方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(Arg::top_<value_type> top) const noexcept;
		
		/// @brief 右方向に拡大縮小した長方形を返します。
		/// @param right 右方向の拡大縮小量
		/// @return 右方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(Arg::right_<value_type> right) const noexcept;
		
		/// @brief 下方向に拡大縮小した長方形を返します。
		/// @param bottom 下方向の拡大縮小量
		/// @return 下方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(Arg::bottom_<value_type> bottom) const noexcept;
		
		/// @brief 左方向に拡大縮小した長方形を返します。
		/// @param left 左方向の拡大縮小量
		/// @return 左方向に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF stretched(Arg::left_<value_type> left) const noexcept;
		
		////////////////////////////////////////////////////////////////
		//
		//	scaledFromCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心を固定して拡大縮小した長方形を返します。
		/// @param s 拡大倍率
		/// @return 中心を固定して拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFromCenter(double s) const noexcept;

		/// @brief 中心を固定して拡大縮小した長方形を返します。
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 中心を固定して拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFromCenter(double sx, double sy) const noexcept;

		/// @brief 中心を固定して拡大縮小した長方形を返します。
		/// @param s X 軸方向と Y 軸方向のそれぞれの拡大倍率
		/// @return 中心を基準に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFromCenter(Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromTopLeft
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上を固定して拡大縮小した長方形を返します。
		/// @param s 拡大倍率
		/// @return 左上を固定して拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFromTopLeft(double s) const noexcept;

		/// @brief 左上を固定して拡大縮小した長方形を返します。
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 左上を固定して拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFromTopLeft(double sx, double sy) const noexcept;

		/// @brief 左上を固定して拡大縮小した長方形を返します。
		/// @param s X 軸方向と Y 軸方向のそれぞれの拡大倍率
		/// @return 左上を固定して拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFromTopLeft(Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を起点に拡大縮小した長方形を返します。
		/// @param _pos 拡大縮小の起点となる座標
		/// @param s 拡大倍率
		/// @return 指定した座標を起点に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFrom(Vec2 _pos, double s) const noexcept;

		/// @brief 指定した座標を起点に拡大縮小した長方形を返します。
		/// @param _pos 拡大縮小の起点となる座標
		/// @param sx X 軸方向の拡大倍率
		/// @param sy Y 軸方向の拡大倍率
		/// @return 指定した座標を起点に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFrom(Vec2 _pos, double sx, double sy) const noexcept;

		/// @brief 指定した座標を起点に拡大縮小した長方形を返します。
		/// @param _pos 拡大縮小の起点となる座標
		/// @param s X 軸方向と Y 軸方向のそれぞれの拡大倍率
		/// @return 指定した座標を起点に拡大縮小した長方形
		[[nodiscard]]
		constexpr RectF scaledFrom(Vec2 _pos, Vec2 s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形が空でないかを返します。
		/// @remark `hasArea()` と同じです。
		/// @return 長方形が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形が空であるかを返します。
		/// @return 空の長方形である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasArea
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形が大きさを持っているかを返します。
		/// @return 長方形が大きさを持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool hasArea() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	leftX, rightX, topY, bottomY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の左辺の X 座標を返します。
		/// @return 長方形の左辺の X 座標
		[[nodiscard]]
		constexpr value_type leftX() const noexcept;

		/// @brief 長方形の右辺の X 座標を返します。
		/// @return 長方形の右辺の X 座標
		[[nodiscard]]
		constexpr value_type rightX() const noexcept;

		/// @brief 長方形の上辺の Y 座標を返します。
		/// @return 長方形の上辺の Y 座標
		[[nodiscard]]
		constexpr value_type topY() const noexcept;

		/// @brief 長方形の下辺の Y 座標を返します。
		/// @return 長方形の下辺の Y 座標
		[[nodiscard]]
		constexpr value_type bottomY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centerX, centerY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の中心の X 座標を返します。
		/// @return 長方形の中心の X 座標
		[[nodiscard]]
		constexpr double centerX() const noexcept;

		/// @brief 長方形の中心の Y 座標を返します。
		/// @return 長方形の中心の Y 座標
		[[nodiscard]]
		constexpr double centerY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tl, tr, br, bl
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の左上の座標を返します。
		/// @return 長方形の左上の座標
		[[nodiscard]]
		constexpr position_type tl() const noexcept;

		/// @brief 長方形の右上の座標を返します。
		/// @return 長方形の右上の座標
		[[nodiscard]]
		constexpr position_type tr() const noexcept;

		/// @brief 長方形の右下の座標を返します。
		/// @return 長方形の右下の座標
		[[nodiscard]]
		constexpr position_type br() const noexcept;

		/// @brief 長方形の左下の座標を返します。
		/// @return 長方形の左下の座標
		[[nodiscard]]
		constexpr position_type bl() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	topCenter, rightCenter, bottomCenter, leftCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の上辺の中心座標を返します。
		/// @return 長方形の上辺の中心座標
		[[nodiscard]]
		constexpr Vec2 topCenter() const noexcept;

		/// @brief 長方形の右辺の中心座標を返します。
		/// @return 長方形の右辺の中心座標
		[[nodiscard]]
		constexpr Vec2 rightCenter() const noexcept;

		/// @brief 長方形の下辺の中心座標を返します。
		/// @return 長方形の下辺の中心座標
		[[nodiscard]]
		constexpr Vec2 bottomCenter() const noexcept;

		/// @brief 長方形の左辺の中心座標を返します。
		/// @return 長方形の左辺の中心座標
		[[nodiscard]]
		constexpr Vec2 leftCenter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	center
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の中心座標を返します。
		/// @return 長方形の中心座標
		[[nodiscard]]
		constexpr Vec2 center() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getRelativePoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の左上を (0 ,0), 右下を (1, 1) としたときの (relativeX, relativeY) の座標を返します。
		/// @param relativeX X 座標の相対的な位置
		/// @param relativeY Y 座標の相対的な位置
		/// @return 長方形の左上を (0 ,0), 右下を (1, 1) としたときの (relativeX, relativeY) の座標
		[[nodiscard]]
		constexpr Vec2 getRelativePoint(double relativeX, double relativeY) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	top, right, bottom, left
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の上辺を Line として返します。
		/// @return 長方形の上辺
		[[nodiscard]]
		constexpr Line top() const noexcept;

		/// @brief 長方形の右辺を Line として返します。
		/// @return 長方形の右辺
		[[nodiscard]]
		constexpr Line right() const noexcept;

		/// @brief 長方形の下辺を Line として返します。
		/// @return 長方形の下辺
		[[nodiscard]]
		constexpr Line bottom() const noexcept;

		/// @brief 長方形の左辺を Line として返します。
		/// @return 長方形の左辺
		[[nodiscard]]
		constexpr Line left() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの頂点座標を返します。
		/// @param index インデックス（0 が左上、1 が右上、2 が右下、3 が左下）
		/// @return 指定したインデックスの頂点座標
		/// @throw std::out_of_range index が範囲外の場合
		[[nodiscard]]
		constexpr position_type pointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の周上の指定した距離に対応する座標を返します。
		/// @param length 距離（左上から時計回り）
		/// @return 長方形の周上の指定した距離に対応する座標
		[[nodiscard]]
		Vec2 pointAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	interpolatedPointAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の周の長さを 1 として、周上の指定した位置に対応する座標を返します。
		/// @param t 位置（左上から時計回り、周の長さを 1 とした場合の位置）
		/// @return 長方形の周上の指定した位置に対応する座標
		[[nodiscard]]
		Vec2 interpolatedPointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sideAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの辺を Line として返します。
		/// @param index インデックス（0 が上辺、1 が右辺、2 が下辺、3 が左辺）
		/// @return 指定したインデックスの辺
		/// @throw std::out_of_range index が範囲外の場合
		[[nodiscard]]
		constexpr Line sideAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	sideIndexAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の周上の指定した距離に対応する辺のインデックスを返します。
		/// @param length 距離（左上から時計回り）
		/// @return 長方形の周上の指定した距離に対応する辺のインデックス（0 が上辺、1 が右辺、2 が下辺、3 が左辺）
		[[nodiscard]]
		size_t sideIndexAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	triangleAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの三角形を返します。
		/// @param index インデックス（0 が tl-tr-bl, 1 が bl-tr-br）
		/// @return 指定したインデックスの三角形
		/// @throw std::out_of_range index が範囲外の場合
		[[nodiscard]]
		constexpr Triangle triangleAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の面積を返します。
		/// @return 長方形の面積
		[[nodiscard]]
		constexpr value_type area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の周の長さを返します。
		/// @return 長方形の周の長さ
		[[nodiscard]]
		constexpr value_type perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalAspectRatio
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の横縦比（幅 / 高さ）を返します。
		/// @return 長方形の横縦比（幅 / 高さ）。高さが 0 の場合は 0
		[[nodiscard]]
		constexpr double horizontalAspectRatio() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToTop
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、上に配置された長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 上に配置された長方形
		[[nodiscard]]
		constexpr RectF copiedToTop(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、右に配置された長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 右に配置された長方形
		[[nodiscard]]
		constexpr RectF copiedToRight(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToBottom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、下に配置された長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 下に配置された長方形
		[[nodiscard]]
		constexpr RectF copiedToBottom(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copiedToLeft
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ大きさで、左に配置された長方形を返します。
		/// @param distance 配置のすき間の大きさ
		/// @return 左に配置された長方形
		[[nodiscard]]
		constexpr RectF copiedToLeft(double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心を軸に、時計回りに回転した長方形を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した長方形
		[[nodiscard]]
		Quad rotated(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を軸として、時計回りに回転した長方形を返します。
		/// @param _x 回転軸の X 座標
		/// @param _y 回転軸の Y 座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した長方形
		[[nodiscard]]
		Quad rotatedAt(double _x, double _y, double angle) const noexcept;

		/// @brief 指定した座標を軸として、時計回りに回転した長方形を返します。
		/// @param _pos 回転軸の座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した長方形
		[[nodiscard]]
		Quad rotatedAt(Vec2 _pos, double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated90At
		//
		////////////////////////////////////////////////////////////////

		/// @brief _posを中心とし、時計回りに 90° * n 回転した長方形を返します。
		/// @param _pos 回転の中心座標
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return _posを中心とし、時計回りに 90° * n 回転した長方形
		[[nodiscard]]
		constexpr RectF rotated90At(const position_type& _pos, int32 n = 1) const noexcept;

		/// @brief _posを中心とし、自身を時計回りに 90° * n 回転します。
		/// @param _pos 回転の中心座標
		/// @param n 時計回りに 90° 回転させる回数（負の場合は反時計回り）
		/// @return *this
		constexpr RectF& rotate90At(const position_type& _pos, int32 n = 1) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shearedX, shearedY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を X 方向にせん断した平行四辺形を返します。
		/// @param vx X 方向のせん断量
		/// @return X 方向にせん断した平行四辺形
		/// @remark 長方形の左上の頂点は (x + vx, y) に移動します。
		[[nodiscard]]
		constexpr Quad shearedX(double vx) const noexcept;

		/// @brief 長方形を Y 方向にせん断した平行四辺形を返します。
		/// @param vy Y 方向のせん断量
		/// @return Y 方向にせん断した平行四辺形
		/// @remark 長方形の左上の頂点は (x, y - vy) に移動します。
		[[nodiscard]]
		constexpr Quad shearedY(double vy) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	skewedX, skewedY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を X 方向に傾斜した平行四辺形を返します。
		/// @param angle X 方向の傾斜角度（ラジアン）
		/// @return X 方向に傾斜した平行四辺形
		[[nodiscard]]
		Quad skewedX(double angle) const noexcept;

		/// @brief 長方形を Y 方向に傾斜した平行四辺形を返します。
		/// @param angle Y 方向の傾斜角度（ラジアン）
		/// @return Y 方向に傾斜した平行四辺形
		[[nodiscard]]
		Quad skewedY(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rounded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角を丸めた RoundRect を作成して返します。
		/// @param r 角の半径
		/// @return 作成した RoundRect
		[[nodiscard]]
		constexpr RoundRect rounded(double r) const noexcept;

		//[[nodiscard]]
		//Polygon rounded(double tl, double tr, double br, double bl) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	chamfered
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を面取りした Polygon を返します。
		/// @param size 面取りの大きさ
		/// @return 面取りした Polygon
		//[[nodiscard]]
		//Polygon chamfered(double size) const;

		/// @brief 長方形を面取りした Polygon を返します。
		/// @param tl 左上の面取りの大きさ
		/// @param tr 右上の面取りの大きさ
		/// @param br 右下の面取りの大きさ
		/// @param bl 左下の面取りの大きさ
		/// @return 面取りした Polygon
		//[[nodiscard]]
		//Polygon chamfered(double tl, double tr, double br, double bl) const;

		////////////////////////////////////////////////////////////////
		//
		//	asQuad
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を Quad として返します。
		/// @return 長方形の Quad
		[[nodiscard]]
		constexpr Quad asQuad() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outline
		//
		////////////////////////////////////////////////////////////////

		///// @brief 長方形の輪郭を LineString として返します。
		///// @param closeRing 頂点配列の終点を始点と重ねるかどうか
		///// @return 長方形の輪郭の LineString
		//[[nodiscard]]
		//LineString outline(CloseRing closeRing = CloseRing::No) const;

		///// @brief 長方形の輪郭の一部を LineString として返します。
		///// @param distanceFromOrigin 開始地点の距離（長方形の左上の頂点から時計回りでの距離）
		///// @param length 長さ
		///// @return 長方形の輪郭の一部の LineString
		//[[nodiscard]]
		//LineString outline(double distanceFromOrigin, double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	outer
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<Vec2> outer() const;

		////////////////////////////////////////////////////////////////
		//
		//	ring
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<Vec2> ring() const;

		////////////////////////////////////////////////////////////////
		//
		//	asPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を Polygon として返します。
		/// @return 長方形の Polygon
		[[nodiscard]]
		Polygon asPolygon() const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の長方形との線形補間を返します。
		/// @param other 別の長方形
		/// @param f 補間係数
		/// @return 線形補間された長方形
		[[nodiscard]]
		constexpr RectF lerp(const RectF& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getOverlap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の長方形と重なる領域を返します。重ならない場合は空の長方形を返します。
		/// @param other 別の長方形
		/// @return 別の長方形と重なる領域。重ならない場合は空の長方形
		[[nodiscard]]
		constexpr RectF getOverlap(const RectF& other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clamped
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を指定された範囲内にクランプした長方形を返します。
		/// @param area クランプする範囲
		/// @return クランプされた長方形
		[[nodiscard]]
		constexpr RectF clamped(const RectF& area) const noexcept;

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

		/// @brief 長方形が現在のフレームで左クリックされ始めたかを返します。
		/// @return 長方形が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 長方形が左クリックされているかを返します。
		/// @return 長方形が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで長方形への左クリックが離されたかを返します。
		/// @return 現在のフレームで長方形への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形が現在のフレームで右クリックされ始めたかを返します。
		/// @return 長方形が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 長方形が右クリックされているかを返します。
		/// @return 長方形が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで長方形への右クリックが離されたかを返します。
		/// @return 現在のフレームで長方形への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形上にマウスカーソルがあるかを返します。
		/// @return 長方形上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		//const RectF& paint(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const RectF& overwrite(Image& dst, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintFrame
		//
		////////////////////////////////////////////////////////////////

		//const RectF& paintFrame(Image& dst, int32 innerThickness, int32 outerThickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteFrame
		//
		////////////////////////////////////////////////////////////////

		//const RectF& overwriteFrame(Image& dst, int32 innerThickness, int32 outerThickness, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を描きます。
		/// @param color 色
		/// @return *this
		const RectF& draw(const ColorF& color = Palette::White) const;

		/// @brief 長方形を描きます。
		/// @param colors 左上、右上、右下、左下の各頂点の色
		/// @remark 描画時には 2 つの三角形に分割されるため、斜め方向のグラデーションは方向によってグラデーションのかかり方が変わります。上下、左右方向のグラデーションには影響しません。
		/// @return *this
		const RectF& draw(const ColorF(&colors)[4]) const;

		/// @brief 長方形を描きます。
		/// @param topColor 長方形の上側の色
		/// @param bottomColor 長方形の下側の色
		/// @remark `rect.draw(Arg::top = ColorF{ 0.0 }, Arg::bottom = ColorF{ 1.0 })` のように呼びます。
		/// @return *this
		const RectF& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 長方形を描きます。
		/// @param leftColor 長方形の左側の色
		/// @param rightColor 長方形の右側の色
		/// @remark `rect.draw(Arg::left = ColorF{ 0.0 }, Arg::right = ColorF{ 1.0 })` のように呼びます。
		/// @return *this
		const RectF& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 長方形を描きます。
		/// @param topLeftColor 長方形の左上の色
		/// @param bottomRightColor 長方形の右下の色
		/// @remark `rect.draw(Arg::topLeft = ColorF{ 0.0 }, Arg::bottomRight = ColorF{ 1.0 })` のように呼びます。
		/// @return *this
		const RectF& draw(Arg::topLeft_<ColorF> topLeftColor, Arg::bottomRight_<ColorF> bottomRightColor) const;

		/// @brief 長方形を描きます。
		/// @param topRightColor 長方形の右上の色
		/// @param bottomLeftColor 長方形の左下の色
		/// @remark `rect.draw(Arg::topRight = ColorF{ 0.0 }, Arg::bottomLeft = ColorF{ 1.0 })` のように呼びます。
		/// @return 
		const RectF& draw(Arg::topRight_<ColorF> topRightColor, Arg::bottomLeft_<ColorF> bottomLeftColor) const;

		/// @brief 長方形を描きます。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const RectF& draw(const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const RectF& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param innerColor 内側の色
		/// @param outerColor 外側の色
		/// @return *this
		const RectF& drawFrame(double thickness, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param topColor 上側の色
		/// @param bottomColor 下側の色
		/// @return *this
		const RectF& drawFrame(double thickness, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param leftColor 左側の色
		/// @param rightColor 右側の色
		/// @return *this
		const RectF& drawFrame(double thickness, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 長方形の枠を描きます。
		/// @param innerThickness 基準の長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の長方形から外側方向への枠の太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const RectF& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		/// @brief 長方形の枠を描きます。
		/// @param innerThickness 基準の長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の長方形から外側方向への枠の太さ（ピクセル）
		/// @param innerColor 内側の色
		/// @param outerColor 外側の色
		/// @return *this
		const RectF& drawFrame(double innerThickness, double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 長方形の枠を描きます。
		/// @param innerThickness 基準の長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の長方形から外側方向への枠の太さ（ピクセル）
		/// @param topColor 上側の色
		/// @param bottomColor 下側の色
		/// @return *this
		const RectF& drawFrame(double innerThickness, double outerThickness, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 長方形の枠を描きます。
		/// @param innerThickness 基準の長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の長方形から外側方向への枠の太さ（ピクセル）
		/// @param leftColor 左側の色
		/// @param rightColor 右側の色
		/// @return *this
		const RectF& drawFrame(double innerThickness, double outerThickness, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const RectF& drawFrame(double thickness, const PatternParameters& pattern) const;

		/// @brief 長方形の枠を描きます。
		/// @param innerThickness 基準の長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の長方形から外側方向への枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const RectF& drawFrame(double innerThickness, double outerThickness, const PatternParameters& pattern) const;

		/// @brief 長方形の枠を描きます。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param lineType 線の種類
		/// @param color 色
		/// @return *this
		const RectF& drawFrame(double thickness, LineType lineType, const ColorF& color = Palette::White) const;

		/// @brief 長方形の枠を描きます。
		/// @param innerThickness 基準の長方形から内側方向への枠の太さ（ピクセル）
		/// @param outerThickness 基準の長方形から外側方向への枠の太さ（ピクセル）
		/// @param lineType 線の種類
		/// @param color 色
		/// @return *this
		const RectF& drawFrame(double innerThickness, double outerThickness, LineType lineType, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawShadow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形の影を描きます。
		/// @param offset 影の位置のオフセット（ピクセル）
		/// @param blur ぼかしの大きさ（ピクセル）
		/// @param spread 影の膨張（ピクセル）
		/// @param color 影の色
		/// @param fill 影の内部を塗りつぶすか
		/// @return *this
		//const RectF& drawShadow(const Vec2& offset, double blur, double spread = 0.0, const ColorF& color = ColorF{ 0.0, 0.5 }, bool fill = true) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedQuad operator ()(const Texture& texture) const;

		[[nodiscard]]
		TexturedQuad operator ()(const TextureRegion& textureRegion) const;

		////////////////////////////////////////////////////////////////
		//
		//	Empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の長方形を返します。
		/// @return 空の長方形 (`RectF{ 0, 0, 0, 0 }`)
		[[nodiscard]]
		static constexpr RectF Empty() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromPoints
		//
		////////////////////////////////////////////////////////////////

		/// @brief 対角線上の 2 点の座標をもとに長方形を作成します。
		/// @param a 対角線をなす座標の 1 つ
		/// @param b 対角線をなす座標の 1 つ
		/// @return 作成した長方形
		[[nodiscard]]
		static constexpr RectF FromPoints(position_type a, position_type b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに RectF の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value RectF
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const RectF& value)
		{
			return output << CharType('(')
				<< value.x << CharType(',') << CharType(' ')
				<< value.y << CharType(',') << CharType(' ')
				<< value.w << CharType(',') << CharType(' ')
				<< value.h << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから RectF の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value RectF の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, RectF& value)
		{
			CharType unused;
			return input >> unused
				>> value.x >> unused
				>> value.y >> unused
				>> value.w >> unused
				>> value.h >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief RectF を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value RectF
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const RectF& value);

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
struct fmt::formatter<s3d::RectF>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::RectF& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::RectF, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::RectF& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::RectF>
{
	[[nodiscard]]
	size_t operator ()(const s3d::RectF& value) const noexcept
	{
		return value.hash();
	}
};
