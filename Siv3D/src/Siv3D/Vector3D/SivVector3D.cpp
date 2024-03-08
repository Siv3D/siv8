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

# include <Siv3D/Vector3D.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{

	template <>
	void Vector3D<double>::_Formatter(FormatData& formatData, const Vector3D<double>& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.z);
		formatData.string.push_back(U')');
	}

	template <>
	void Vector3D<float>::_Formatter(FormatData& formatData, const Vector3D<float>& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.z);
		formatData.string.push_back(U')');
	}

	template struct Vector3D<float>;
	template struct Vector3D<double>;
}
