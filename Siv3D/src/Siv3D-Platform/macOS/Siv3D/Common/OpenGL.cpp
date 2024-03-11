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

# include "OpenGL.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	void CheckOpenGLError()
	{
		/*
		size_t limitter = 0;

		GLenum err;

		while ((err = glGetError()) != GL_NO_ERROR)
		{
			LOG_ERROR(fmt::format("OpenGL Error: 0x{:x}", err));

			if (++limitter > 30)
			{
				LOG_ERROR("OpenGL error report interrupted.");
				break;
			}
		}
		 */
	}
}
