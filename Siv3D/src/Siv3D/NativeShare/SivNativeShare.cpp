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

# include <Siv3D/NativeShare.hpp>
# include <Siv3D/NativeShare/INativeShare.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace NativeShare
	{
		bool Show(const Image& image)
		{
			return SIV3D_ENGINE(NativeShare)->show(image);
		}
	}
}
