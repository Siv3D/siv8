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



		/// @brief 円を描きます。
		/// @param color 円の色
		/// @return *this
		const Circle& draw(const ColorF& color = Palette::White) const;

		/// @brief 円を描きます。
		/// @param innerColor 円の内側の色
		/// @param outerColor 円の外側の色
		/// @return *this
		const Circle& draw(const ColorF& innerColor, const ColorF& outerColor) const;





		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Circle& value);
	};
}
