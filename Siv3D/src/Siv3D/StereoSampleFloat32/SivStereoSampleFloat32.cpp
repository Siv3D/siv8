//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/WaveSample.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const StereoSampleFloat32 value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.left);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.right);
		formatData.string.push_back(U')');
	}
}
