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

# include <Siv3D/Pentablet.hpp>
# include <Siv3D/Pentablet/IPentablet.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Pentablet
	{
		bool IsAvailable()
		{
			return SIV3D_ENGINE(Pentablet)->isAvailable();
		}

		bool IsConnected()
		{
			return SIV3D_ENGINE(Pentablet)->isConnected();
		}

		const String& GetName()
		{
			return SIV3D_ENGINE(Pentablet)->getName();
		}

		const PenCaps& GetCaps()
		{
			return SIV3D_ENGINE(Pentablet)->getCaps();
		}

		const PenState& GetState()
		{
			return SIV3D_ENGINE(Pentablet)->getState();
		}
	}
}
