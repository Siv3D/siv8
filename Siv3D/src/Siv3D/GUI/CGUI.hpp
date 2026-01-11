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

# pragma once
# include <Siv3D/Font.hpp>
# include <Siv3D/Texture.hpp>
# include <Siv3D/GUIColorStyle.hpp>
# include <Siv3D/GUIShapeStyle.hpp>
# include "IGUI.hpp"

namespace s3d
{
	class CGUI final : public ISiv3DGUI
	{
	public:

		~CGUI() override;

		void init() override;

		const Font& getDefaultFont() const noexcept override;

		const Texture& getHueTexture() const noexcept override;

		const GUIColorStyle& getColorStyle(const Theme& theme) const noexcept override;

		const GUIShapeStyle& getShapeStyle(const Theme& theme) const noexcept override;

	private:

		std::unique_ptr<Font> m_defaultFont;

		std::unique_ptr<Texture> m_hueTexture;

		std::array<GUIColorStyle, 32> m_colorStyles;

		std::array<GUIShapeStyle, 8> m_shapeStyles;
	};
}
