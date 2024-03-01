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

# include <Siv3D/PointVector.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{

	template <>
	void Vector2D<double>::_Formatter(FormatData& formatData, const Vector2D<double>& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.push_back(U')');
	}

	template <>
	void Vector2D<float>::_Formatter(FormatData& formatData, const Vector2D<float>& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.push_back(U')');
	}

	template struct Vector2D<float>;
	template struct Vector2D<double>;
}
