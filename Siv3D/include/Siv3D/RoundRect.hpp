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
	/// @brief 角丸長方形
	struct RoundRect
	{
		using position_type	= RectF::position_type;

		using size_type		= RectF::size_type;

		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			RectF rect;

			struct
			{
				value_type x, y, w, h;
			};
		};

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

		[[nodiscard]]
		constexpr RoundRect(value_type _x, value_type _y, value_type _w, value_type _h, value_type _r) noexcept;
		
		[[nodiscard]]
		constexpr RoundRect(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, value_type _w, value_type _h, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(value_type _x, value_type _y, size_type size, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, size_type size, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, size_type size, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(position_type pos, size_type size, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, value_type _w, value_type _h, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, Concept::Arithmetic auto _w, Concept::Arithmetic auto _h, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, size_type size, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(Arg::center_<position_type> _center, size_type size, Concept::Arithmetic auto _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(const RectF& _rect, value_type _r) noexcept;

		[[nodiscard]]
		constexpr RoundRect(const RectF& _rect, Concept::Arithmetic auto _r) noexcept;
	};
}
