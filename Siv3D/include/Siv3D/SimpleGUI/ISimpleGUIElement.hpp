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
# include "../Common.hpp"
# include "../PointVector.hpp"
# include "../Anchor.hpp"
# include "../Theme.hpp"
# include "../Cursor.hpp"
# include "../2DShapes.hpp"

namespace s3d
{
	class ISimpleGUIElement
	{
	public:

		ISimpleGUIElement() = default;

		constexpr ISimpleGUIElement(const Vec2& pos, Anchor anchor, bool enabled, Theme theme);

		virtual ~ISimpleGUIElement() = default;

		virtual RectF region() const = 0;

		virtual bool update() = 0;

		virtual void draw() const = 0;

		virtual void drawOverlay() const;

		virtual void show();

		virtual void hide();

		virtual bool isHovered() const noexcept;

		virtual bool isPressed() const noexcept;

		virtual void onMouseEnter();

		virtual void onMouseLeave();

		virtual void onMouseDown();

		virtual void onMouseUp();

		template <class Self>
		constexpr Self& setPos(this Self& self, const Vec2& pos) noexcept;

		[[nodiscard]]
		constexpr Vec2 getPos() const noexcept;

		template <class Self>
		constexpr Self& setAnchor(this Self& self, Anchor anchor) noexcept;

		[[nodiscard]]
		constexpr Anchor getAnchor() const noexcept;

		template <class Self>
		constexpr Self& setEnabled(this Self& self, bool enabled) noexcept;

		[[nodiscard]]
		constexpr bool isEnabled() const noexcept;

		template <class Self>
		constexpr Self& setVisible(this Self& self, bool visible) noexcept;

		[[nodiscard]]
		constexpr bool isVisible() const noexcept;

		template <class Self>
		constexpr Self& setTheme(this Self& self, Theme theme) noexcept;

		[[nodiscard]]
		constexpr Theme getTheme() const noexcept;

	protected:

		template <class Self>
		constexpr Self& setDirty(this Self& self) noexcept;

		template <class Self>
		constexpr Self& clearDirty(this Self& self) noexcept;

		[[nodiscard]]
		constexpr bool isDirty() const noexcept;

	private:

		Vec2 m_pos{ 0, 0 };

		Anchor m_anchor = Anchor::TopLeft;

		bool m_enabled = true;

		bool m_visible = true;

		mutable bool m_isDirty = true;

		Theme m_theme = Theme::Light;

	protected:

		struct MouseState
		{
			bool hovered = false;
			bool pressed = false;
		} m_mouseState;

		void updateMouseEvent(const MouseState& oldState, const MouseState& newState);
	};
}

# include "ISimpleGUIElement.ipp"
