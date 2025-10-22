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
# include <Siv3D/Font.hpp>
# include "IGUI.hpp"

namespace s3d
{
	class CGUI final : public ISiv3DGUI
	{
	public:

		~CGUI() override;

		void init() override;

		const Font& getDefaultFont() const noexcept override;

	private:

		std::unique_ptr<Font> m_defaultFont;
	};
}
