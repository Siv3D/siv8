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

namespace s3d
{
	namespace Cursor
	{
		namespace Internal
		{
			void SetCameraTransform(const Mat3x2& matrix);

			void SetLocalTransform(const Mat3x2& matrix);
		}
	}
}
