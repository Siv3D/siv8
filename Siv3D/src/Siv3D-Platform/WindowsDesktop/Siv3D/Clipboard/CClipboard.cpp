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
# include <shlobj_core.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/ScopeExit.hpp>
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

					const int32 waitTimeMillisec = (1 << i);
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
			const size_t totalBytes = (sizeof(BITMAPINFOHEADER) + pixelBytes);

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

			// Image: RGBA -> DIB（白背景合成）
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

		[[nodiscard]]
		static std::string RTF_EscapeUnicodeToU(const std::wstring_view rtf)
		{
			std::string out;
			out.reserve(rtf.size() * 2);

			for (wchar_t wc : rtf)
			{
				// ASCII はそのまま（RTF の制御語や \ { } も含む）
				if (wc >= 0x20 && wc <= 0x7E)
				{
					out.push_back(static_cast<char>(wc));
					continue;
				}

				// CR/LF/TAB はそのまま通す（任意）
				if (wc == L'\r' || wc == L'\n' || wc == L'\t')
				{
					out.push_back(static_cast<char>(wc));
					continue;
				}

				// RTF の \u は 16-bit signed 値（UTF-16 code unit）を想定する実装が多い
				const int16_t s = static_cast<int16_t>(wc);

				char buf[32];
				const int n = std::snprintf(buf, sizeof(buf), "\\u%d?", static_cast<int>(s));
				out.append(buf, buf + n);
			}

			return out;
		}

		[[nodiscard]]
		static std::string BuildCFHTML(const std::string& fragmentUtf8)
		{
			const std::string kStartMark = "<!--StartFragment-->";
			const std::string kEndMark = "<!--EndFragment-->";

			// フラグメントを HTML ドキュメントに埋める
			std::string htmlDoc;
			htmlDoc.reserve(fragmentUtf8.size() + 128);
			htmlDoc += "<html><body>";
			htmlDoc += kStartMark;
			htmlDoc += fragmentUtf8;
			htmlDoc += kEndMark;
			htmlDoc += "</body></html>";

			// ヘッダ（オフセットは後で埋める）
			std::string header;
			header += "Version:0.9\r\n";
			header += "StartHTML:00000000\r\n";
			header += "EndHTML:00000000\r\n";
			header += "StartFragment:00000000\r\n";
			header += "EndFragment:00000000\r\n";

			const size_t startHTML = header.size();
			const size_t endHTML = startHTML + htmlDoc.size();

			const size_t fragStartInDoc = htmlDoc.find(kStartMark);
			const size_t fragEndInDoc = htmlDoc.find(kEndMark);

			const size_t startFragment = startHTML + (fragStartInDoc + kStartMark.size());
			const size_t endFragment = startHTML + fragEndInDoc;

			auto Patch8 = [&](const char* key, size_t value)
			{
				const size_t pos = header.find(key);
				if (pos == std::string::npos) return;
				const size_t colon = header.find(':', pos);
				if (colon == std::string::npos) return;

				char buf[16] = {};
				std::snprintf(buf, sizeof(buf), "%08u", static_cast<unsigned>(value));

				// ':' の直後（空白なし想定）に 8 桁を書き込む
				const size_t wpos = colon + 1;
				if (wpos + 8 <= header.size())
				{
					for (int i = 0; i < 8; ++i)
					{
						header[wpos + i] = buf[i];
					}
				}
			};

			Patch8("StartHTML", startHTML);
			Patch8("EndHTML", endHTML);
			Patch8("StartFragment", startFragment);
			Patch8("EndFragment", endFragment);

			return header + htmlDoc;
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
			ClipboardGuard clipboard{ m_hWnd };
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
			ClipboardGuard clipboard{ m_hWnd };
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
			ClipboardGuard clipboard{ m_hWnd };
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
					ScopeExit unlock{ [&]() { ::GlobalUnlock(hData); } };
					const size_t size_bytes = static_cast<size_t>(::GlobalSize(hData));
					if (0 < size_bytes)
					{
						Image decoded{ MemoryViewReader { p, size_bytes }, premultiplyAlpha, ImageFormat::PNG };

						if (decoded)
						{
							image = std::move(decoded);
							return true;
						}
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

			ScopeExit unlock{ [&]() { ::GlobalUnlock(hData); } };

			// DIB は先頭が BITMAPINFOHEADER / BITMAPV4HEADER / BITMAPV5HEADER のいずれか
			if (sizeof(BITMAPINFOHEADER) > ::GlobalSize(hData))
			{
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
				return false;
			}

			const int32 width = static_cast<int32>(bih->biWidth);
			const int32 heightAbs = static_cast<int32>(std::abs(bih->biHeight));
			const bool topDown = (bih->biHeight < 0);

			if ((Image::MaxWidth < width) || (Image::MaxHeight < heightAbs))
			{
				return false;
			}

			const uint16 bpp = static_cast<uint16>(bih->biBitCount);
			const DWORD compression = bih->biCompression;

			if ((bpp != 32) && (bpp != 24))
			{
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
				return false;
			}

			// スキャンラインは 4byte アライン
			const size_t srcStride = ((static_cast<size_t>(width) * bpp + 31) / 32) * 4;

			// biSizeImage が 0 のこともあるので stride*height で見る
			const size_t needed = offset + (srcStride * static_cast<size_t>(heightAbs));
			if (::GlobalSize(hData) < needed)
			{
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

							if (alphaMask)
							{
								const uint8 a = ExtractChannel(v, alphaMask);
								
								if (premultiplyAlpha)
								{
									*dst++ = Color::PremultiplyAlpha(Color{ r, g, b, a });
								}
								else
								{
									*dst++ = Color{ r, g, b, a };
								}
							}
							else
							{
								*dst++ = Color{ r, g, b, 255 };
							}
						}
					}
					else
					{
						// BI_RGB の 32bit
						for (int32 x = 0; x < width; ++x)
						{
							const uint8 b = *src++;
							const uint8 g = *src++;
							const uint8 r = *src++;
							++src; // A（予約領域の場合もあるため無視）
							*dst++ = Color{ r, g, b, 255 };
						}
					}
				}
			}

			return true;
		};

		// 2) CF_DIBV5
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
		if (paths.isEmpty())
		{
			return;
		}

		{
			ClipboardGuard clipboard{ m_hWnd };
			if (not clipboard)
			{
				return;
			}

			if (not ::EmptyClipboard())
			{
				return;
			}

			// DROPFILES に続けて、wchar_t のファイルパス列（\0 区切り、最後は \0\0）
			std::wstring all;
			all.reserve(256);

			for (const auto& p : paths)
			{
				if (p.isEmpty())
				{
					continue;
				}

				// Windows の CF_HDROP は通常、フルパスが無難（相対も動く場合はあるが推奨しない）
				const std::wstring w = FileSystem::NativePath(p);
				if (w.empty())
				{
					continue;
				}

				all.append(w);
				all.push_back(L'\0');
			}

			// 有効なパスが無ければ何もしない
			if (all.empty())
			{
				return;
			}

			// 二重 NUL 終端
			all.push_back(L'\0');

			const size_t dropfilesSize = sizeof(DROPFILES);
			const size_t pathsSizeBytes = (all.size() * sizeof(wchar_t));
			const size_t totalBytes = (dropfilesSize + pathsSizeBytes);

			HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, totalBytes);
			if (not hMem)
			{
				return;
			}

			auto* pMem = static_cast<uint8_t*>(::GlobalLock(hMem));
			if (not pMem)
			{
				::GlobalFree(hMem);
				return;
			}

			auto* df = reinterpret_cast<DROPFILES*>(pMem);
			df->pFiles = static_cast<DWORD>(dropfilesSize); // 文字列列の開始オフセット
			df->pt.x = 0;
			df->pt.y = 0;
			df->fNC = FALSE;
			df->fWide = TRUE; // wchar_t

			std::memcpy((pMem + dropfilesSize), all.data(), pathsSizeBytes);

			::GlobalUnlock(hMem);

			if (not ::SetClipboardData(CF_HDROP, hMem))
			{
				::GlobalFree(hMem); // 失敗時のみ解放（成功時は OS 所有）
				return;
			}

			// 任意：コピー扱いを明示（Explorer などで挙動が安定する場合がある）
			// CF_PREFERREDDROPEFFECT に DROPEFFECT_COPY を入れる
			{
				const DWORD effect = DROPEFFECT_COPY;
				if (HGLOBAL hEffect = MakeGlobalCopy(&effect, sizeof(effect)))
				{
					const UINT cfPreferredDropEffect = ::RegisterClipboardFormatW(CFSTR_PREFERREDDROPEFFECT);
					if (not ::SetClipboardData(cfPreferredDropEffect, hEffect))
					{
						::GlobalFree(hEffect);
					}
				}
			}
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getFilePaths(Array<FilePath>& paths)
	{
		paths.clear();

		ClipboardGuard clipboard{ m_hWnd };
		if (not clipboard)
		{
			return false;
		}

		if (::IsClipboardFormatAvailable(CF_HDROP) == 0)
		{
			return false;
		}

		HANDLE hData = ::GetClipboardData(CF_HDROP);
		if (not hData)
		{
			return false;
		}

		HDROP hDrop = static_cast<HDROP>(hData);

		// ファイル数取得
		const UINT count = ::DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
		if (count == 0)
		{
			return false;
		}

		paths.reserve(count);

		std::wstring buf;
		for (UINT i = 0; i < count; ++i)
		{
			// 文字数（終端 NUL を含まない）
			const UINT len = ::DragQueryFileW(hDrop, i, nullptr, 0);
			if (len == 0)
			{
				continue;
			}

			buf.resize(len + 1);

			if (::DragQueryFileW(hDrop, i, buf.data(), len + 1) == 0)
			{
				continue;
			}

			// Win32 のパスを Siv3D の FilePath に変換
			FilePath p = Unicode::FromWstring(std::wstring_view(buf.data(), len));

			// 末尾が NUL のときに混ざるのを避ける（念のため）
			if (p && (p.back() == U'\0'))
			{
				p.pop_back();
			}

			p = FileSystem::FullPath(p);

			if (p)
			{
				paths << std::move(p);
			}
		}

		return (not paths.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasFilePaths()
	{
		return (::IsClipboardFormatAvailable(CF_HDROP) != 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setRichText
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setRichText(const StringView richText, const StringView plainTextFallback)
	{
		{
			ClipboardGuard clipboard{ m_hWnd };
			if (not clipboard)
			{
				return;
			}

			if (not ::EmptyClipboard())
			{
				return;
			}

			// 1) RTF
			if (m_cfRTF && richText)
			{
				// 非 ASCII を \u に置換
				const std::wstring w = richText.toWstr();
				std::string rtfBytes = RTF_EscapeUnicodeToU(w);

				// 念のため \uc1 が無い場合に追加（既にあるなら不要）
				if (rtfBytes.find("\\uc") == std::string::npos)
				{
					if (const auto pos = rtfBytes.find("\\rtf1"); pos != std::string::npos)
					{
						// "\rtf1" の直後に入れる
						const auto insertPos = pos + 5;
						rtfBytes.insert(insertPos, "\\uc1");
					}
				}

				const size_t size_bytes = (rtfBytes.size() + 1); // 終端のヌル文字分も含める
				if (HGLOBAL hMem = MakeGlobalCopy(rtfBytes.c_str(), size_bytes))
				{
					if (not ::SetClipboardData(m_cfRTF, hMem))
					{
						::GlobalFree(hMem);
						// 失敗してもフォールバックは続行
					}
				}
			}

			// 2) プレーンテキスト（フォールバック）
			if (plainTextFallback)
			{
				const std::wstring wstr = plainTextFallback.toWstr();
				const size_t size_bytes = ((wstr.size() + 1) * sizeof(wchar_t));

				if (HGLOBAL hMem = MakeGlobalCopy(wstr.data(), size_bytes))
				{
					if (not ::SetClipboardData(CF_UNICODETEXT, hMem))
					{
						::GlobalFree(hMem);
						return;
					}
				}
			}
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setHTML
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setHTML(const StringView html, const StringView plainTextFallback)
	{
		{
			ClipboardGuard clipboard{ m_hWnd };
			if (not clipboard)
			{
				return;
			}

			if (not ::EmptyClipboard())
			{
				return;
			}

			// 1) HTML
			if (m_cfHTML)
			{
				const std::string fragmentUtf8 = Unicode::ToUTF8(html);
				const std::string cfhtml = BuildCFHTML(fragmentUtf8);
				const size_t size_bytes = (cfhtml.size() + 1); // 終端のヌル文字分も含める

				if (HGLOBAL hMem = MakeGlobalCopy(cfhtml.c_str(), size_bytes))
				{
					if (not ::SetClipboardData(m_cfHTML, hMem))
					{
						::GlobalFree(hMem);
						// 失敗してもフォールバックは続行
					}
				}
			}

			// 2) プレーンテキスト（フォールバック）
			if (plainTextFallback)
			{
				const std::wstring wstr = plainTextFallback.toWstr();
				const size_t size_bytes = ((wstr.size() + 1) * sizeof(wchar_t)); // 終端のヌル文字分も含める
				
				if (HGLOBAL hMem = MakeGlobalCopy(wstr.data(), size_bytes))
				{
					if (not ::SetClipboardData(CF_UNICODETEXT, hMem))
					{
						::GlobalFree(hMem); // 失敗時のみ解放（成功時は OS 所有）
						return;
					}
				}
			}
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setData
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setData(const StringView mimeType, const void* data, const size_t size)
	{
		if ((not mimeType) || (not data) || (size == 0))
		{
			return;
		}

		const std::wstring fmtName = mimeType.toWstr();
		const UINT format = ::RegisterClipboardFormatW(fmtName.c_str());
		if (format == 0)
		{
			return;
		}

		ClipboardGuard clipboard{ m_hWnd };
		if (not clipboard)
		{
			return;
		}

		if (not ::EmptyClipboard())
		{
			return;
		}

		HGLOBAL hMem = MakeGlobalCopy(data, size);
		if (not hMem)
		{
			return;
		}

		if (not ::SetClipboardData(format, hMem))
		{
			::GlobalFree(hMem); // 失敗時のみ解放（成功時は OS 所有）
			return;
		}

		m_sequenceNumber = ::GetClipboardSequenceNumber();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getData
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getData(const StringView mimeType, Blob& data)
	{
		data.clear();

		if (not mimeType)
		{
			return false;
		}

		const std::wstring fmtName = mimeType.toWstr();
		const UINT format = ::RegisterClipboardFormatW(fmtName.c_str());
		if (format == 0)
		{
			return false;
		}

		ClipboardGuard clipboard{ m_hWnd };
		if (not clipboard)
		{
			return false;
		}

		if (::IsClipboardFormatAvailable(format) == 0)
		{
			return false;
		}

		HANDLE hData = ::GetClipboardData(format);
		if (not hData)
		{
			return false;
		}

		const void* pSrc = ::GlobalLock(hData);
		if (not pSrc)
		{
			return false;
		}

		ScopeExit unlock{ [&]() { ::GlobalUnlock(hData); } };

		const size_t size_bytes = static_cast<size_t>(::GlobalSize(hData));
		if (size_bytes == 0)
		{
			return false;
		}

		data.resize(size_bytes);
		std::memcpy(data.data(), pSrc, size_bytes);

		return true;
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
