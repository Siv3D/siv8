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

# include <Siv3D/Cursor/CCursor.hpp>

namespace s3d
{
	ISiv3DCursor* ISiv3DCursor::Create()
	{
		return new CCursor;
	}
}
