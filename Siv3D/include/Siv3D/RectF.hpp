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





		/// @brief 長方形を描画します。
		/// @param color 色
		/// @return *this
		const RectF& draw(const ColorF& color = Palette::White) const;

	

		friend void Formatter(FormatData& formatData, const RectF& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::RectF, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::RectF& value, s3d::BufferContext& ctx);
};
