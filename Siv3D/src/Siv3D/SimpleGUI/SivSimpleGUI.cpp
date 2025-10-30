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

# include <Siv3D/SimpleGUI.hpp>
# include <Siv3D/SimpleGUI/SimpleButton.hpp>
# include <Siv3D/SimpleGUI/SimpleCheckBox.hpp>
# include <Siv3D/GUI/IGUI.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace SimpleGUI
	{
		const Font& GetFont() noexcept
		{
			return SIV3D_ENGINE(GUI)->getDefaultFont();
		}

		bool Button(const StringView text, const Vec2& pos, const Theme theme)
		{
			return Button(text, pos, Anchor::TopLeft, unspecified, true, theme);
		}

		bool Button(const StringView text, const Vec2& pos, const Optional<double>& width, const Theme theme)
		{
			return Button(text, pos, Anchor::TopLeft, width, true, theme);
		}

		bool Button(const StringView text, const Vec2& pos, const Optional<double>& width, const bool enabled, const Theme theme)
		{
			return Button(text, pos, Anchor::TopLeft, width, enabled, theme);
		}

		bool Button(const StringView text, const Vec2& pos, const Anchor anchor, const Theme theme)
		{
			return Button(text, pos, anchor, unspecified, true, theme);
		}

		bool Button(const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		{
			SimpleButton button{ text, pos, anchor, width, enabled, theme };
			const bool result = button.update();
			button.draw();
			return result;
		}


		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Theme theme)
		{
			return CheckBox(checked, text, pos, Anchor::TopLeft, unspecified, true, theme);
		}

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Optional<double>& width, const Theme theme)
		{
			return CheckBox(checked, text, pos, Anchor::TopLeft, width, true, theme);
		}

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Optional<double>& width, const bool enabled, const Theme theme)
		{
			return CheckBox(checked, text, pos, Anchor::TopLeft, width, enabled, theme);
		}

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Anchor anchor, const Theme theme)
		{
			return CheckBox(checked, text, pos, anchor, unspecified, true, theme);
		}

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		{
			SimpleCheckBox checkBox{ checked, text, pos, anchor, width, enabled, theme };
			const bool result = checkBox.update();
			checked = checkBox.isChecked();
			checkBox.draw();
			return result;
		}
	}
}
