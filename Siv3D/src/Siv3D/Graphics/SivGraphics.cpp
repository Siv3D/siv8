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

# include <Siv3D/Graphics.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Graphics
	{
		////////////////////////////////////////////////////////////////
		//
		//	SetVSyncEnabled
		//
		////////////////////////////////////////////////////////////////

		void SetVSyncEnabled(const bool enabled)
		{
			return SIV3D_ENGINE(Renderer)->setVSyncEnabled(enabled);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsVSyncEnabled
		//
		////////////////////////////////////////////////////////////////

		bool IsVSyncEnabled()
		{
			return SIV3D_ENGINE(Renderer)->isVSyncEnabled();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetLowLatencyMode
		//
		////////////////////////////////////////////////////////////////

		void SetLowLatencyMode(const bool enabled)
		{
			return SIV3D_ENGINE(Renderer)->setLowLatencyMode(enabled);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsLowLatencyMode
		//
		////////////////////////////////////////////////////////////////

		bool IsLowLatencyMode()
		{
			return SIV3D_ENGINE(Renderer)->isLowLatencyMode();
		}
	}
}
