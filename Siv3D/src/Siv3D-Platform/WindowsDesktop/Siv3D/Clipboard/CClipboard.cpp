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

# include "CClipboard.hpp"
# include <Siv3D/Image.hpp>
# include <Siv3D/MemoryViewReader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		struct ClipboardGuard
		{
			bool opened = false;

			[[nodiscard]]
			explicit ClipboardGuard(const HWND hwnd)
				: opened{ TryOpenClipboard(hwnd) } {}

			~ClipboardGuard()
			{
				if (opened)
				{
					::CloseClipboard();
				}
			}

			[[nodiscard]]
			explicit operator bool() const noexcept
			{
				return opened;
			}

			[[nodiscard]]
			static bool TryOpenClipboard(const HWND hwnd, const int32 retries = 5)
			{
				for (int32 i = 0; i < retries; ++i)
				{
					if (::OpenClipboard(hwnd))
					{
						return true;
					}

					const int32 waitTimeMillisec = i;
					::Sleep(waitTimeMillisec);
				}

				return false;
			}
		};

		[[nodiscard]]
		static HGLOBAL MakeGlobalCopy(const void* data, const size_t size_bytes)
		{
			HGLOBAL h = ::GlobalAlloc(GMEM_MOVEABLE, size_bytes);
			if (not h)
			{
				return nullptr;
			}

			void* pDst = ::GlobalLock(h);
			if (not pDst)
			{
				::GlobalFree(h);
				return nullptr;
			}

			std::memcpy(pDst, data, size_bytes);			
			::GlobalUnlock(h);
			
			return h;
		}

		// 4byte境界
		[[nodiscard]]
		static constexpr size_t Align4(size_t x) noexcept
		{
			return (x + 3) & ~size_t(3);
		}

		// CF_DIB 用の HGLOBAL を作る（24bpp / top-down）
		[[nodiscard]]
		static HGLOBAL MakeCFDIB24(const Image& image)
		{
			const int32 w = image.width();
			const int32 h = image.height();
			if (w <= 0 || h <= 0) return nullptr;

			const size_t rowBytes = Align4(static_cast<size_t>(w) * 3); // BGR 24bpp
			const size_t pixelBytes = rowBytes * static_cast<size_t>(h);
			const size_t totalBytes = sizeof(BITMAPINFOHEADER) + pixelBytes;

			HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, totalBytes);
			if (!hMem) return nullptr;

			uint8_t* p = static_cast<uint8_t*>(::GlobalLock(hMem));
			if (!p)
			{
				::GlobalFree(hMem);
				return nullptr;
			}

			auto* bih = reinterpret_cast<BITMAPINFOHEADER*>(p);
			std::memset(bih, 0, sizeof(BITMAPINFOHEADER));
			bih->biSize			= sizeof(BITMAPINFOHEADER);
			bih->biWidth		= w;
			bih->biHeight		= -h; // top-down
			bih->biPlanes		= 1;
			bih->biBitCount		= 24;
			bih->biCompression	= BI_RGB;
			bih->biSizeImage	= static_cast<DWORD>(pixelBytes);

			uint8_t* dstBase = p + sizeof(BITMAPINFOHEADER);

			// Image: RGBA -> DIB
			const Color* src = image.data();
			for (int32 y = 0; y < h; ++y)
			{
				uint8* dst = dstBase + rowBytes * static_cast<size_t>(y);

				for (int32 x = 0; x < w; ++x)
				{
					uint8 r, g, b;

					if (src->a == 255)
					{
						r = src->r;
						g = src->g;
						b = src->b;
					}
					else if (src->a == 0)
					{
						r = 255;
						g = 255;
						b = 255;
					}
					else
					{
						const uint32 srcAlpha = src->a;
						const uint32 invSrcAlpha = (255 - srcAlpha);
						b = static_cast<uint8>(invSrcAlpha + Color::Div255Round(src->b * srcAlpha));
						g = static_cast<uint8>(invSrcAlpha + Color::Div255Round(src->g * srcAlpha));
						r = static_cast<uint8>(invSrcAlpha + Color::Div255Round(src->r * srcAlpha));
					}

					*dst++ = b;
					*dst++ = g;
					*dst++ = r;
					++src;
				}

				// パディング（0で埋める）
				const size_t pad = rowBytes - static_cast<size_t>(w) * 3;
				if (pad)
				{
					std::memset(dst, 0, pad);
				}
			}

			::GlobalUnlock(hMem);
			return hMem;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CClipboard::~CClipboard()
	{
		LOG_SCOPED_DEBUG("CClipboard::~CClipboard()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::init()
	{
		LOG_SCOPED_DEBUG("CClipboard::init()");

		m_hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
		m_sequenceNumber = ::GetClipboardSequenceNumber();

		m_cfPNG		= ::RegisterClipboardFormatW(L"PNG");
		m_cfHTML	= ::RegisterClipboardFormatW(L"HTML Format");
		m_cfRTF		= ::RegisterClipboardFormatW(L"Rich Text Format");
		m_cfCSV		= ::RegisterClipboardFormatW(L"CSV");
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasChanged
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasChanged()
	{
		const uint64 currentSequenceNumber = ::GetClipboardSequenceNumber();
		return (std::exchange(m_sequenceNumber, currentSequenceNumber) != currentSequenceNumber);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSequenceNumber
	//
	////////////////////////////////////////////////////////////////

	uint64 CClipboard::getSequenceNumber()
	{
		return ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::clear()
	{
		{
			ClipboardGuard clipboard(m_hWnd);
			if (not clipboard)
			{
				return;
			}

			::EmptyClipboard();
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setText
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setText(const StringView text)
	{
		{
			ClipboardGuard clipboard(m_hWnd);
			if (not clipboard)
			{
				return;
			}

			if (not ::EmptyClipboard())
			{
				return;
			}

			const std::wstring wstr = text.toWstr();
			const size_t size_bytes = ((wstr.size() + 1) * sizeof(wchar_t)); // wstring 終端のヌル文字分も含める

			HGLOBAL hMem = MakeGlobalCopy(wstr.data(), size_bytes);

			if (not hMem)
			{
				return;
			}

			if (not ::SetClipboardData(CF_UNICODETEXT, hMem))
			{
				::GlobalFree(hMem); // 失敗時のみ解放（成功時は OS 所有）
				return;
			}
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getText
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getText(String& text)
	{
		text.clear();

		{
			ClipboardGuard clipboard{ m_hWnd };
			if (not clipboard)
			{
				return false;
			}

			HANDLE hData = ::GetClipboardData(CF_UNICODETEXT);
			if (not hData)
			{
				return false;
			}

			const void* p = ::GlobalLock(hData);
			if (not p)
			{
				return false;
			}

			text = Unicode::FromWstring(static_cast<const wchar_t*>(p));

			::GlobalUnlock(hData);
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasText
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasText()
	{
		return (::IsClipboardFormatAvailable(CF_UNICODETEXT) != 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setImage
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setImage(const Image& image)
	{
		if (image.isEmpty())
		{
			return;
		}

		{
			ClipboardGuard clipboard(m_hWnd);
			if (not clipboard)
			{
				return;
			}
			
			if (not ::EmptyClipboard())
			{
				return;
			}

			if (const Blob png = image.encodePNG())
			{
				if (m_cfPNG)
				{
					if (const HGLOBAL hPng = MakeGlobalCopy(png.data(), png.size_bytes()))
					{
						if (not ::SetClipboardData(m_cfPNG, hPng))
						{
							::GlobalFree(hPng); // 失敗時のみ解放（成功時は OS 所有）
						}
					}
				}
			}

			// CF_DIB
			if (HGLOBAL hDib = MakeCFDIB24(image))
			{
				// 成功時は OS 所有。失敗時のみ GlobalFree
				if (!::SetClipboardData(CF_DIB, hDib))
				{
					::GlobalFree(hDib);
				}
			}
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImage
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getImage(Image& image, const PremultiplyAlpha premultiplyAlpha)
	{
		image.clear();

		ClipboardGuard clipboard{ m_hWnd };
		if (not clipboard)
		{
			return false;
		}

		// 1) PNG 形式
		if (m_cfPNG)
		{
			HANDLE hData = ::GetClipboardData(m_cfPNG);
			if (hData)
			{
				const void* p = ::GlobalLock(hData);
				if (p)
				{
					const size_t sizeBytes = static_cast<size_t>(::GlobalSize(hData));
					if (sizeBytes > 0)
					{
						Image decoded{ MemoryViewReader { p, sizeBytes }, premultiplyAlpha, ImageFormat::PNG };
						::GlobalUnlock(hData);

						if (decoded)
						{
							image = std::move(decoded);
							return true;
						}
					}
					else
					{
						::GlobalUnlock(hData);
					}
				}
			}
		}

		auto TryFromDIB = [&](UINT format) -> bool
		{
			HANDLE hData = ::GetClipboardData(format);
			if (not hData)
			{
				return false;
			}

			const uint8* p = static_cast<const uint8*>(::GlobalLock(hData));
			if (not p)
			{
				return false;
			}

			const auto Unlock = [&]() { ::GlobalUnlock(hData); };

			// DIB は先頭が BITMAPINFOHEADER / BITMAPV4HEADER / BITMAPV5HEADER のいずれか
			if (sizeof(BITMAPINFOHEADER) > ::GlobalSize(hData))
			{
				Unlock();
				return false;
			}

			const BITMAPINFOHEADER* bih = reinterpret_cast<const BITMAPINFOHEADER*>(p);
			const DWORD headerSize = bih->biSize;

			// 最低限の妥当性
			if ((headerSize < sizeof(BITMAPINFOHEADER))
				|| (headerSize > sizeof(BITMAPV5HEADER))
				|| (bih->biPlanes != 1)
				|| (bih->biWidth <= 0)
				|| (bih->biHeight == 0))
			{
				Unlock();
				return false;
			}

			const int32 width = static_cast<int32>(bih->biWidth);
			const int32 heightAbs = static_cast<int32>(std::abs(bih->biHeight));
			const bool topDown = (bih->biHeight < 0);

			const uint16 bpp = static_cast<uint16>(bih->biBitCount);
			const DWORD compression = bih->biCompression;

			if ((bpp != 32) && (bpp != 24))
			{
				Unlock();
				return false;
			}

			// 色テーブル（パレット）があるケースは今回は対象外（bpp 24/32 以外は弾いているが念のため）
			// ビットマップデータの開始位置：
			//   headerSize + masks(必要なら) + colorTable(必要なら)
			size_t offset = headerSize;

			// BITFIELDS の場合、マスクが header の直後にある（V4/V5 はヘッダ内にもある）
			DWORD redMask = 0, greenMask = 0, blueMask = 0, alphaMask = 0;

			if (compression == BI_BITFIELDS)
			{
				if (headerSize >= sizeof(BITMAPV4HEADER))
				{
					const BITMAPV4HEADER* v4 = reinterpret_cast<const BITMAPV4HEADER*>(p);
					redMask = v4->bV4RedMask;
					greenMask = v4->bV4GreenMask;
					blueMask = v4->bV4BlueMask;
					alphaMask = v4->bV4AlphaMask;
				}
				else
				{
					// BITMAPINFOHEADER の BITFIELDS は直後に DWORD×3（RGB）
					if (::GlobalSize(hData) < (offset + sizeof(DWORD) * 3))
					{
						Unlock();
						return false;
					}

					const DWORD* masks = reinterpret_cast<const DWORD*>(p + offset);
					redMask = masks[0];
					greenMask = masks[1];
					blueMask = masks[2];
					alphaMask = 0;
					offset += sizeof(DWORD) * 3;
				}
			}
			else if (compression == BI_RGB)
			{
				// 24/32bit の BI_RGB は通常 BGR/BGRA
			}
			else
			{
				// RLE 等は非対応
				Unlock();
				return false;
			}

			// スキャンラインは 4byte アライン
			const size_t srcStride = ((static_cast<size_t>(width) * bpp + 31) / 32) * 4;

			// biSizeImage が 0 のこともあるので stride*height で見る
			const size_t needed = offset + (srcStride * static_cast<size_t>(heightAbs));
			if (::GlobalSize(hData) < needed)
			{
				Unlock();
				return false;
			}

			image.resize(width, heightAbs);

			const uint8* srcBase = p + offset;
			Color* dst = image.data();

			// 32bit の mask 展開（BITFIELDS で一般化）
			auto ExtractChannel = [](uint32 v, uint32 mask) -> uint8
			{
				if (mask == 0) return 0;
				// 末尾の 1 の位置へ
				uint32 shift = 0;
				while ((mask & 1u) == 0u) { mask >>= 1u; ++shift; }
				uint32 m = mask; // 連続 1 を仮定（現実的）
				uint32 x = (v >> shift) & m;

				// x を 0..255 にスケール
				// m は (2^bits-1) の形を期待
				const uint32 denom = m;
				if (denom == 0) return 0;
				return static_cast<uint8>((x * 255u) / denom);
			};

			for (int32 y = 0; y < heightAbs; ++y)
			{
				const int32 sy = topDown ? y : (heightAbs - 1 - y);
				const uint8* src = srcBase + (static_cast<size_t>(sy) * srcStride);

				if (bpp == 24)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint8 b = *src++;
						const uint8 g = *src++;
						const uint8 r = *src++;
						*dst++ = Color{ r, g, b, 255 };
					}
				}
				else // 32bpp
				{
					if (compression == BI_BITFIELDS && (redMask || greenMask || blueMask))
					{
						for (int32 x = 0; x < width; ++x)
						{
							const uint32 v = *reinterpret_cast<const uint32*>(src);
							src += 4;

							const uint8 r = ExtractChannel(v, redMask);
							const uint8 g = ExtractChannel(v, greenMask);
							const uint8 b = ExtractChannel(v, blueMask);
							const uint8 a = (alphaMask ? ExtractChannel(v, alphaMask) : 255);
							*dst++ = Color{ r, g, b, a };
						}
					}
					else
					{
						// BI_RGB の 32bit は一般的に BGRA
						for (int32 x = 0; x < width; ++x)
						{
							const uint8 b = *src++;
							const uint8 g = *src++;
							const uint8 r = *src++;
							const uint8 a = *src++;
							*dst++ = Color{ r, g, b, a };
						}
					}
				}
			}

			Unlock();
			return true;
		};

		// 2) CF_DIBV5 を優先
		if (TryFromDIB(CF_DIBV5))
		{
			return true;
		}

		// 3) CF_DIB
		if (TryFromDIB(CF_DIB))
		{
			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasImage
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasImage()
	{
		if (::IsClipboardFormatAvailable(CF_DIBV5) || ::IsClipboardFormatAvailable(CF_DIB))
		{
			return true;
		}

		if (m_cfPNG)
		{
			return (::IsClipboardFormatAvailable(m_cfPNG) != 0);
		}

		return false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setFilePaths
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setFilePaths(const Array<FilePath>& paths)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	getFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getFilePaths(Array<FilePath>& paths)
	{
		return(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setHTML
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setHTML(const StringView html)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	getHTML
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getHTML(String& html)
	{
		return(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setData
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setData(const StringView mimeType, const void* data, const size_t size)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	getData
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getData(const StringView mimeType, Blob& data)
	{
		return(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAvailableMimeTypes
	//
	////////////////////////////////////////////////////////////////

	Array<String> CClipboard::getAvailableMimeTypes()
	{
		Array<String> result;

		ClipboardGuard clipboard{ m_hWnd };
		if (not clipboard)
		{
			return result;
		}

		auto AddUnique = [&](const String& s)
		{
			if (s && (not result.contains(s)))
			{
				result << s;
			}
		};

		UINT format = 0;
		while ((format = ::EnumClipboardFormats(format)) != 0)
		{
			switch (format)
			{
			case CF_UNICODETEXT:
			case CF_TEXT:
				AddUnique(U"text/plain");
				continue;

			case CF_HDROP:
				AddUnique(U"text/uri-list");
				continue;

			case CF_DIB:
			case CF_DIBV5:
			case CF_BITMAP:
				AddUnique(U"image/bmp");
				continue;

			default:
				break;
			}

			// --- 登録済みフォーマット名ベースのマッピング ---
			if (format == m_cfPNG)
			{
				AddUnique(U"image/png");
				continue;
			}
			else if (format == m_cfHTML)
			{
				AddUnique(U"text/html");
				continue;
			}
			else if (format == m_cfRTF)
			{
				AddUnique(U"text/rtf");
				continue;
			}
			else if (format == m_cfCSV)
			{
				AddUnique(U"text/csv");
				continue;
			}

			// フォーマット名を取得（登録済みフォーマットのみ名前が取れる）
			wchar_t nameW[256] = {};
			
			if (const int nameLen = ::GetClipboardFormatNameW(format, nameW, static_cast<int>(std::size(nameW))); 
				0 < nameLen)
			{
				const String name = Unicode::FromWstring(std::wstring_view(nameW, nameLen));

				// 追加で典型名を拾う
				if (name == U"PNG")
				{
					AddUnique(U"image/png");
					continue;
				}
				else if (name == U"HTML Format")
				{
					AddUnique(U"text/html");
					continue;
				}
				else if (name == U"Rich Text Format")
				{
					AddUnique(U"text/rtf");
					continue;
				}
				else if (name == U"CSV")
				{
					AddUnique(U"text/csv");
					continue;
				}
				else if (name.contains(U'/'))
				{
					AddUnique(name);
					continue;
				}

				// それ以外は必要なら Win32 形式として返す（任意）
				//AddUnique(U"application/x-win32-clipboard;format=" + name);
				continue;
			}
		}

		return result;
	}
}
