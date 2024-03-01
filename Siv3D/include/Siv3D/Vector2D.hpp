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

namespace s3d
{
	struct FormatData;

	/// @brief 2 次元のベクトル
	/// @tparam Type ベクトルの要素の型
	template <class Type>
	struct Vector2D
	{
		/// @brief ベクトルの要素の型
		using value_type = Type;

		/// @brief ベクトルの次元数
		static constexpr size_t Dimension = 2;

		/// @brief ベクトルの X 成分
		value_type x;

		/// @brief ベクトルの Y 成分
		value_type y;





		friend void Formatter(FormatData& formatData, const Vector2D& value)
		{
			_Formatter(formatData, value);
		}

		static void _Formatter(FormatData& formatData, const Vector2D& value);
	};

	using Float2	= Vector2D<float>;
	using Vec2		= Vector2D<double>;
	using SizeF		= Vec2;
}
