//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CCursorStyle.hpp"
# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CCursorStyle::~CCursorStyle()
	{
		LOG_SCOPED_DEBUG("CCursorStyle::~CCursorStyle()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::init()
	{
		LOG_SCOPED_DEBUG("CCursorStyle::init()");

		m_systemCursors[FromEnum(CursorStyle::Arrow)]			= ::LoadCursorW(nullptr, IDC_ARROW);
		m_systemCursors[FromEnum(CursorStyle::IBeam)]			= ::LoadCursorW(nullptr, IDC_IBEAM);
		m_systemCursors[FromEnum(CursorStyle::Cross)]			= ::LoadCursorW(nullptr, IDC_CROSS);
		m_systemCursors[FromEnum(CursorStyle::Hand)]			= ::LoadCursorW(nullptr, IDC_HAND);
		m_systemCursors[FromEnum(CursorStyle::NotAllowed)]		= ::LoadCursorW(nullptr, IDC_NO);
		m_systemCursors[FromEnum(CursorStyle::ResizeUpDown)]	= ::LoadCursorW(nullptr, IDC_SIZENS);
		m_systemCursors[FromEnum(CursorStyle::ResizeLeftRight)]	= ::LoadCursorW(nullptr, IDC_SIZEWE);
		m_systemCursors[FromEnum(CursorStyle::ResizeNWSE)]		= ::LoadCursorW(nullptr, IDC_SIZENWSE);
		m_systemCursors[FromEnum(CursorStyle::ResizeNESW)]		= ::LoadCursorW(nullptr, IDC_SIZENESW);
		m_systemCursors[FromEnum(CursorStyle::ResizeAll)]		= ::LoadCursorW(nullptr, IDC_SIZEALL);
		m_systemCursors[FromEnum(CursorStyle::Hidden)]			= nullptr;

		m_currentCursor		= m_systemCursors[FromEnum(CursorStyle::Arrow)];
		m_defaultCursor		= m_currentCursor;
		m_requestedCursor	= m_defaultCursor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::update()
	{
		if (m_currentCursor != m_requestedCursor)
		{
			m_currentCursor = m_requestedCursor;
			
			::SetCursor(m_currentCursor);
		}
		else if (m_requestedCursor == nullptr)
		{
			// カーソルを必ず非表示にするための workaround
			::SetCursor(nullptr);
		}

		m_requestedCursor = m_defaultCursor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setDefaultStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::setDefaultStyle(const CursorStyle style)
	{
		m_defaultCursor = m_systemCursors[FromEnum(style)];
	}

	////////////////////////////////////////////////////////////////
	//
	//	registerCustomStyle
	//
	////////////////////////////////////////////////////////////////

	bool CCursorStyle::registerCustomStyle(const StringView name, const Image& image, const Point hotSpot, const AlphaPremultiplied alphaPremultiplied)
	{
		if (m_customCursors.contains(name))
		{
			LOG_FAIL(fmt::format("❌ CCursorStyle::registerCustomStyle(): `{}` is already registered", name));
			return false;
		}

		const BITMAPV5HEADER bi
		{
			.bV5Size		= sizeof(bi),
			.bV5Width		= image.width(),
			.bV5Height		= -image.height(),
			.bV5Planes		= 1,
			.bV5BitCount	= 32,
			.bV5Compression	= BI_BITFIELDS,
			.bV5RedMask		= 0x00FF0000,
			.bV5GreenMask	= 0x0000FF00,
			.bV5BlueMask	= 0x000000FF,
			.bV5AlphaMask	= 0xFF000000,
		};

		const HDC hdc = ::GetDC(nullptr);
		void* pData;
		HBITMAP hBitmap = ::CreateDIBSection(hdc, reinterpret_cast<const BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pData, nullptr, 0);
		ScopeExit cleanupBitmap = [&hBitmap]() { ::DeleteObject(hBitmap); };

		::ReleaseDC(nullptr, hdc);

		if (not hBitmap)
		{
			return false;
		}

		HBITMAP hMask = ::CreateBitmap(image.width(), image.height(), 1, 1, nullptr);
		ScopeExit cleanupMask = [&hMask]() { ::DeleteObject(hMask); };

		if (not hMask)
		{
			return false;
		}

		{
			uint8* pDst = static_cast<uint8*>(pData);
			const Color* pSrc = image.data();
			
			if (alphaPremultiplied)
			{
				for (size_t i = 0; i < image.num_pixels(); ++i)
				{
					const Color unmultiplied = Color::UnpremultiplyAlpha(*pSrc);

					pDst[0] = unmultiplied.b;
					pDst[1] = unmultiplied.g;
					pDst[2] = unmultiplied.r;
					pDst[3] = unmultiplied.a;

					pDst += 4;
					++pSrc;
				}
			}
			else
			{
				for (size_t i = 0; i < image.num_pixels(); ++i)
				{
					pDst[0] = pSrc->b;
					pDst[1] = pSrc->g;
					pDst[2] = pSrc->r;
					pDst[3] = pSrc->a;

					pDst += 4;
					++pSrc;
				}
			}
		}

		ICONINFO iconInfo =
		{
			.fIcon		= FALSE, // Cursor
			.xHotspot	= static_cast<DWORD>(Max(0, hotSpot.x)),
			.yHotspot	= static_cast<DWORD>(Max(0, hotSpot.y)),
			.hbmMask	= hMask,
			.hbmColor	= hBitmap,
		};

		if (HICON hIcon = ::CreateIconIndirect(&iconInfo))
		{
			m_customCursors.emplace(name, UniqueResource{ hIcon, CursorDeleter });	
			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	unregisterCustomStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::unregisterCustomStyle(const StringView name)
	{
		if (auto it = m_customCursors.find(name);
			it != m_customCursors.end())
		{
			const HICON hIcon = it->second.get();

			if (m_defaultCursor == hIcon)
			{
				m_defaultCursor = m_systemCursors[FromEnum(CursorStyle::Arrow)];
			}

			if (m_requestedCursor == hIcon)
			{
				m_requestedCursor = m_defaultCursor;
			}

			{
				std::lock_guard lock{ m_mutex };
				m_garbage.emplace_back(std::move(it->second));
			}

			m_customCursors.erase(it);

			::SetCursor(nullptr);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearCustomStyles
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::clearCustomStyles()
	{
		{
			std::lock_guard lock{ m_mutex };

			for (auto& cursor : m_customCursors)
			{
				const HICON hIcon = cursor.second.get();

				if (m_defaultCursor == hIcon)
				{
					m_defaultCursor = m_systemCursors[FromEnum(CursorStyle::Arrow)];
				}

				if (m_requestedCursor == hIcon)
				{
					m_requestedCursor = m_defaultCursor;
				}

				m_garbage.emplace_back(std::move(cursor.second));
			}
		}

		m_customCursors.clear();

		::SetCursor(nullptr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	requestStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::requestStyle(const CursorStyle style)
	{
		m_requestedCursor = m_systemCursors[FromEnum(style)];
	}

	////////////////////////////////////////////////////////////////
	//
	//	requestStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::requestStyle(const StringView name)
	{
		if (auto it = m_customCursors.find(name);
			it != m_customCursors.end())
		{
			m_requestedCursor = it->second.get();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onSetCursor
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::onSetCursor()
	{
		::SetCursor(m_currentCursor);

		{
			std::lock_guard lock{ m_mutex };
			m_garbage.clear();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::CursorDeleter(const HICON h)
	{
		[[maybe_unused]] const BOOL result = ::DestroyIcon(h);
		LOG_DEBUG(fmt::format("DestroyIcon -> {}", (result != 0)));
	}
}
