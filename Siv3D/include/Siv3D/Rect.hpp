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
