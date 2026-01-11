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

# include <Siv3D/SimpleGUI.hpp>
# include <Siv3D/SimpleGUI/SimpleButton.hpp>
# include <Siv3D/SimpleGUI/SimpleCheckBox.hpp>
# include <Siv3D/SimpleGUI/SimpleProgressBar.hpp>
# include <Siv3D/GUI/IGUI.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace SimpleGUI
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetFont
		//
		////////////////////////////////////////////////////////////////

		const Font& GetFont() noexcept
		{
			return SIV3D_ENGINE(GUI)->getDefaultFont();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetHueTexture
		//
		////////////////////////////////////////////////////////////////

		const Texture& GetHueTexture() noexcept
		{
			return SIV3D_ENGINE(GUI)->getHueTexture();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Button
		//
		////////////////////////////////////////////////////////////////

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

		bool Button(const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const Theme theme)
		{
			return Button(text, pos, anchor, width, true, theme);
		}

		bool Button(const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		{
			SimpleButton button{ text, pos, anchor, width, enabled, theme };
			
			bool result = false;

			for (const auto layer : SimpleGUI::AllLayers)
			{
				result |= button.update(layer);
			}

			for (const auto layer : SimpleGUI::AllLayers)
			{
				button.draw(layer);
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	CheckBox
		//
		////////////////////////////////////////////////////////////////

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

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const Theme theme)
		{
			return CheckBox(checked, text, pos, anchor, width, true, theme);
		}

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		{
			SimpleCheckBox checkBox{ checked, text, pos, anchor, width, enabled, theme };
			
			bool result = false;

			for (const auto layer : SimpleGUI::AllLayers)
			{
				result |= checkBox.update(layer);
			}

			checked = checkBox.isChecked();

			for (const auto layer : SimpleGUI::AllLayers)
			{
				checkBox.draw(layer);
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	ProgressBar
		//
		////////////////////////////////////////////////////////////////

		void ProgressBar(const double progress, const Vec2& pos, const Theme theme)
		{
			return ProgressBar(progress, pos, Anchor::TopLeft, SimpleProgressBar::DefaultWidth, true, theme);
		}

		void ProgressBar(const double progress, const Vec2& pos, const double width, const Theme theme)
		{
			return ProgressBar(progress, pos, Anchor::TopLeft, width, true, theme);
		}

		void ProgressBar(const double progress, const Vec2& pos, const double width, const bool enabled, const Theme theme)
		{
			return ProgressBar(progress, pos, Anchor::TopLeft, width, enabled, theme);
		}

		void ProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const Theme theme)
		{
			return ProgressBar(progress, pos, anchor, SimpleProgressBar::DefaultWidth, true, theme);
		}

		void ProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		{
			return ProgressBar(progress, pos, anchor, width, true, theme);
		}

		void ProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const double width, const bool enabled, const Theme theme)
		{
			const SimpleProgressBar progressBar{ progress, pos, anchor, width, enabled, theme };

			for (const auto layer : SimpleGUI::AllLayers)
			{
				progressBar.draw(layer);
			}
		}
	}
}
