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

# include <Siv3D/Theme.hpp>
# include <Siv3D/GUI/IGUI.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	const GUIColorStyle& Theme::getColorStyle() const noexcept
	{
		return SIV3D_ENGINE(GUI)->getColorStyle(*this);
	}

	const GUIShapeStyle& Theme::getShapeStyle() const noexcept
	{
		return SIV3D_ENGINE(GUI)->getShapeStyle(*this);
	}

}
