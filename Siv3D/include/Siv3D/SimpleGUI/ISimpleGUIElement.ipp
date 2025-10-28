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

namespace s3d
{
	constexpr ISimpleGUIElement::ISimpleGUIElement(const Vec2& pos, const Anchor anchor, const bool enabled, const Theme theme)
		: m_pos{ pos }
		, m_anchor{ anchor }
		, m_enabled{ enabled }
		, m_theme{ theme } {}

	inline void ISimpleGUIElement::drawOverlay() const {}

	template <class Self>
	constexpr Self& ISimpleGUIElement::setPos(this Self& self, const Vec2& pos) noexcept
	{
		self.m_pos = pos;
		return self;
	}

	constexpr Vec2 ISimpleGUIElement::getPos() const noexcept
	{
		return m_pos;
	}

	template <class Self>
	constexpr Self& ISimpleGUIElement::setAnchor(this Self& self, const Anchor anchor) noexcept
	{
		self.m_anchor = anchor;
		return self;
	}

	constexpr Anchor ISimpleGUIElement::getAnchor() const noexcept
	{
		return m_anchor;
	}

	template <class Self>
	constexpr Self& ISimpleGUIElement::setEnabled(this Self& self, const bool enabled) noexcept
	{
		self.m_enabled = enabled;
		return self;
	}

	constexpr bool ISimpleGUIElement::isEnabled() const noexcept
	{
		return m_enabled;
	}

	template <class Self>
	constexpr Self& ISimpleGUIElement::setVisible(this Self& self, const bool visible) noexcept
	{
		self.m_visible = visible;
		return self;
	}

	constexpr bool ISimpleGUIElement::isVisible() const noexcept
	{
		return m_visible;
	}

	template <class Self>
	constexpr Self& ISimpleGUIElement::setTheme(this Self& self, const Theme theme) noexcept
	{
		self.m_theme = theme;
		self.m_isDirty = true;
		return self;
	}

	constexpr Theme ISimpleGUIElement::getTheme() const noexcept
	{
		return m_theme;
	}

	template <class Self>
	constexpr Self& ISimpleGUIElement::setDirty(this Self& self) noexcept
	{
		self.m_isDirty = true;
		return self;
	}

	template <class Self>
	constexpr Self& ISimpleGUIElement::clearDirty(this Self& self) noexcept
	{
		self.m_isDirty = false;
		return self;
	}

	constexpr bool ISimpleGUIElement::isDirty() const noexcept
	{
		return m_isDirty;
	}
}
