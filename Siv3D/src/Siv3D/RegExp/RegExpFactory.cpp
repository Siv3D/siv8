//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CRegExp.hpp"

namespace s3d
{
	ISiv3DRegExp* ISiv3DRegExp::Create()
	{
		return new CRegExp;
	}
}
