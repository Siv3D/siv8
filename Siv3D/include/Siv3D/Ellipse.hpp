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

namespace s3d
{
	/// @brief 楕円
	struct Ellipse
	{
		using position_type	= Vec2;

		using size_type		= position_type::value_type;

		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

			union
		{
			/// @brief 楕円の中心座標
			position_type center;

			struct
			{
				/// @brief 楕円の中心の X 座標
				value_type x;

				/// @brief 楕円の中心の Y 座標
				value_type y;
			};
		};

		union
		{
			/// @brief 楕円の X 軸 Y 軸に沿った半径
			position_type axes;

			struct
			{
				/// @brief 楕円の X 軸上の半径
				value_type a;

				/// @brief 楕円の Y 軸上の半径
				value_type b;
			};
		};

	SIV3D_DISABLE_MSVC_WARNINGS_POP()

		/// @brief 
		[[nodiscard]]
		Ellipse() = default;

		/// @brief 
		/// @param r 
		[[nodiscard]]
		explicit constexpr Ellipse(size_type r) noexcept;

		/// @brief 
		/// @param _a 
		/// @param _b 
		[[nodiscard]]
		constexpr Ellipse(size_type _a, size_type _b) noexcept;

		/// @brief 
		/// @param _x 
		/// @param _y 
		/// @param r 
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, size_type r) noexcept;

		/// @brief 
		/// @param _x 
		/// @param _y 
		/// @param _a 
		/// @param _b 
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, size_type _a, size_type _b) noexcept;

		/// @brief 
		/// @param _center 
		[[nodiscard]]
		explicit constexpr Ellipse(position_type _center) noexcept;

		/// @brief 
		/// @param _center 
		/// @param _r 
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, size_type r) noexcept;

		/// @brief 
		/// @param _center 
		/// @param _a 
		/// @param _b 
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, size_type _a, size_type _b) noexcept;

		/// @brief 
		/// @param _x 
		/// @param _y 
		/// @param _axes 
		[[nodiscard]]
		constexpr Ellipse(value_type _x, value_type _y, const Vec2& _axes) noexcept;

		/// @brief 
		/// @param _center 
		/// @param _axes 
		[[nodiscard]]
		constexpr Ellipse(const position_type& _center, const Vec2& _axes) noexcept;

		/// @brief 
		/// @param circle 
		[[nodiscard]]
		explicit constexpr Ellipse(const Circle& circle) noexcept;
	};
}
