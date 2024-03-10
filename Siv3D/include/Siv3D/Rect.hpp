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
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	struct FormatData;

	/// @brief 長方形 (成分が int32 型)
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
	struct Rect
	{
		using position_type = Point;

		using size_type		= Point;

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
		Rect() = default;

		/// @brief 長方形を作成します。
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		explicit constexpr Rect(value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _size 長方形の幅と高さ
		[[nodiscard]]
		explicit constexpr Rect(Concept::Integral auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr Rect(value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		explicit constexpr Rect(Concept::Integral auto _w, Concept::Integral auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _size 長方形の大きさ
		[[nodiscard]]
		explicit constexpr Rect(size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の縦横の大きさ
		[[nodiscard]]
		constexpr Rect(value_type _x, value_type _y, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の縦横の大きさ
		[[nodiscard]]
		constexpr Rect(Concept::Integral auto _x, Concept::Integral auto _y, Concept::Integral auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _w 長方形の幅
		/// @param _h 	長方形の高さ
		[[nodiscard]]
		constexpr Rect(value_type _x, value_type _y, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _w 長方形の幅
		/// @param _h 	長方形の高さ
		[[nodiscard]]
		constexpr Rect(Concept::Integral auto _x, Concept::Integral auto _y, Concept::Integral auto _w, Concept::Integral auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の大きさ
		[[nodiscard]]
		constexpr Rect(value_type _x, value_type _y, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _x 長方形の左上の点の X 座標
		/// @param _y 長方形の左上の点の Y 座標
		/// @param _size 長方形の大きさ
		[[nodiscard]]
		constexpr Rect(Concept::Integral auto _x, Concept::Integral auto _y, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _size 長方形の大きさ
		[[nodiscard]]
		constexpr Rect(position_type _pos, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _size 長方形の大きさ
		[[nodiscard]]
		constexpr Rect(position_type _pos, Concept::Integral auto _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr Rect(position_type _pos, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _w 長方形の幅
		/// @param _h 長方形の高さ
		[[nodiscard]]
		constexpr Rect(position_type _pos, Concept::Integral auto _w, Concept::Integral auto _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _pos 長方形の左上の点の座標
		/// @param _size 長方形の大きさ
		[[nodiscard]]
		constexpr Rect(position_type _pos, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::center_<position_type> _center, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::center_<position_type> _center, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param _center 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::center_<position_type> _center, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::topLeft_<position_type> topLeft, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::topLeft_<position_type> topLeft, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topLeft 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::topLeft_<position_type> topLeft, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::topCenter_<position_type> topCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::topCenter_<position_type> topCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::topCenter_<position_type> topCenter, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::topRight_<position_type> topRight, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::topRight_<position_type> topRight, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param topRight 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::topRight_<position_type> topRight, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::rightCenter_<position_type> rightCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::rightCenter_<position_type> rightCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param rightCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::rightCenter_<position_type> rightCenter, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::bottomRight_<position_type> bottomRight, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::bottomRight_<position_type> bottomRight, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomRight 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::bottomRight_<position_type> bottomRight, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::bottomCenter_<position_type> bottomCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::bottomCenter_<position_type> bottomCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::bottomCenter_<position_type> bottomCenter, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::bottomLeft_<position_type> bottomLeft, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::bottomLeft_<position_type> bottomLeft, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param bottomLeft 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::bottomLeft_<position_type> bottomLeft, size_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::leftCenter_<position_type> leftCenter, value_type _size) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 
		/// @param _w 
		/// @param _h 
		[[nodiscard]]
		constexpr Rect(Arg::leftCenter_<position_type> leftCenter, value_type _w, value_type _h) noexcept;

		/// @brief 長方形を作成します。
		/// @param leftCenter 
		/// @param _size 
		[[nodiscard]]
		constexpr Rect(Arg::leftCenter_<position_type> leftCenter, size_type _size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const Rect& lhs, const Rect& rhs) noexcept
		{
			return ((lhs.pos == rhs.pos)
				&& (lhs.size == rhs.size));
		}















		/// @brief 空の長方形を返します。
		/// @return 空の長方形 (`Rect{ 0, 0, 0, 0 }`)
		[[nodiscard]]
		static constexpr Rect Empty() noexcept;





		friend void Formatter(FormatData& formatData, const Rect& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Rect, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Rect& value, s3d::BufferContext& ctx);
};

# include "detail/Rect.ipp"
