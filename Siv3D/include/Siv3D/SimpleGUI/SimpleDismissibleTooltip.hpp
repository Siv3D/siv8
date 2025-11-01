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

# pragma once
# include "../DrawableText.hpp"
# include "ISimpleGUIElement.hpp"
# include "SimpleTooltip.hpp"

namespace s3d
{
	class SimpleDismissibleTooltip : public SimpleTooltip
	{
	public:

		using SimpleTooltip::SimpleTooltip;

		virtual void drawOverlay() const override;

		virtual void onMouseDown() override;
	};
}
