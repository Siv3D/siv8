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
	struct FormatData;

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
		using position_type = Vec2;

		using size_type = Vec2;

		using value_type = size_type::value_type;

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
		/// @param _h 	長方形の高さ
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
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を描画します。
		/// @param color 色
		/// @return *this
		const RectF& draw(const ColorF& color = Palette::White) const;

		/// @brief 長方形を描画します。
		/// @param colors 左上、右上、右下、左下の各頂点の色
		/// @remark 描画時には 2 つの三角形に分割されるため、斜め方向のグラデーションは方向によってグラデーションのかかり方が変わります。上下、左右方向のグラデーションには影響しません。
		/// @return *this
		const RectF& draw(const ColorF(&colors)[4]) const;

		/// @brief 長方形を描画します。
		/// @param topColor 長方形の上側の色
		/// @param bottomColor 長方形の下側の色
		/// @remark `rect.draw(Arg::top = ColorF{ 0.0 }, Arg::bottom = ColorF{ 1.0 })` のように呼びます。
		/// @return *this
		const RectF& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 長方形を描画します。
		/// @param leftColor 長方形の左側の色
		/// @param rightColor 長方形の右側の色
		/// @remark `rect.draw(Arg::left = ColorF{ 0.0 }, Arg::right = ColorF{ 1.0 })` のように呼びます。
		/// @return *this
		const RectF& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 長方形を描画します。
		/// @param topLeftColor 長方形の左上の色
		/// @param bottomRightColor 長方形の右下の色
		/// @remark `rect.draw(Arg::topLeft = ColorF{ 0.0 }, Arg::bottomRight = ColorF{ 1.0 })` のように呼びます。
		/// @return *this
		const RectF& draw(Arg::topLeft_<ColorF> topLeftColor, Arg::bottomRight_<ColorF> bottomRightColor) const;

		/// @brief 長方形を描画します。
		/// @param topRightColor 長方形の右上の色
		/// @param bottomLeftColor 長方形の左下の色
		/// @remark `rect.draw(Arg::topRight = ColorF{ 0.0 }, Arg::bottomLeft = ColorF{ 1.0 })` のように呼びます。
		/// @return 
		const RectF& draw(Arg::topRight_<ColorF> topRightColor, Arg::bottomLeft_<ColorF> bottomLeftColor) const;


		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const RectF& value);
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
