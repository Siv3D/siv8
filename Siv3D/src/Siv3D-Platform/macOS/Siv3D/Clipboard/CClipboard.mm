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
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
# include <AppKit/AppKit.h>
# include <CoreGraphics/CoreGraphics.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool LooksLikeRTF(const StringView s)
		{
			if (not s) return false;
			const std::wstring w = s.toWstr();
			// ざっくり判定
			return (w.rfind(L"{\\rtf", 0) == 0);
		}

		// Windows 版と同等の \u エスケープ（UTF-16 code unit 前提）
		[[nodiscard]]
		static std::string RTF_EscapeUnicodeToU(const std::wstring_view rtf)
		{
			std::string out;
			out.reserve(rtf.size() * 2);
			
			for (wchar_t wc : rtf)
			{
				if (wc >= 0x20 && wc <= 0x7E)
				{
					out.push_back(static_cast<char>(wc));
					continue;
				}
				
				if (wc == L'\r' || wc == L'\n' || wc == L'\t')
				{
					out.push_back(static_cast<char>(wc));
					continue;
				}
				
				const int16_t s = static_cast<int16_t>(wc);
				char buf[32];
				const int n = std::snprintf(buf, sizeof(buf), "\\u%d?", static_cast<int>(s));
				out.append(buf, buf + n);
			}
			
			return out;
		}
	
		[[nodiscard]]
		static std::string WrapHTMLDocumentUTF8(const std::string& fragmentUtf8)
		{
			std::string doc;
			doc.reserve(fragmentUtf8.size() + 256);
			doc += "<html><head>";
			doc += "<meta charset=\"utf-8\">";
			doc += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
			doc += "</head><body>";
			doc += fragmentUtf8;
			doc += "</body></html>";
			return doc;
		}
	}

	struct CClipboard::Impl
	{
		NSPasteboard* pasteboard = nil;
		NSInteger sequenceNumber = 0;
	};
	
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CClipboard::CClipboard()
		: m_pImpl{ std::make_unique<CClipboard::Impl>() } {}

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
		
		@autoreleasepool
		{
			m_pImpl->pasteboard = [NSPasteboard generalPasteboard];
			m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasChanged
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasChanged()
	{
		@autoreleasepool
		{
			const NSInteger currentSequenceNumber = [m_pImpl->pasteboard changeCount];
			return (std::exchange(m_pImpl->sequenceNumber, currentSequenceNumber) != currentSequenceNumber);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSequenceNumber
	//
	////////////////////////////////////////////////////////////////

	uint64 CClipboard::getSequenceNumber()
	{
		@autoreleasepool
		{
			const NSInteger currentSequenceNumber = [m_pImpl->pasteboard changeCount];
			return static_cast<uint64>(currentSequenceNumber);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::clear()
	{
		@autoreleasepool
		{
			[m_pImpl->pasteboard clearContents];
			m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setText
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setText(const StringView text)
	{
		const std::string utf8Text = Unicode::ToUTF8(text);
		
		@autoreleasepool
		{
			NSString* ns = [[NSString alloc] initWithBytes:utf8Text.data() length:utf8Text.size() encoding:NSUTF8StringEncoding];
			
			if (not ns)
			{
				// 変換失敗時は空文字列
				ns = @"";
			}
			
			[m_pImpl->pasteboard clearContents];
			[m_pImpl->pasteboard setString:ns forType:NSPasteboardTypeString];
			
			m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getText
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getText(String& text)
	{
		text.clear();
		
		@autoreleasepool
		{
			NSString* ns = [m_pImpl->pasteboard stringForType:NSPasteboardTypeString];
			if (not ns)
			{
				return false;
			}
			
			const char* utf8Text = [ns UTF8String];
			if (not utf8Text)
			{
				return false;
			}

			text = Unicode::FromUTF8(utf8Text);
			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasText
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasText()
	{
		@autoreleasepool
		{
			NSArray<NSPasteboardType>* types = [m_pImpl->pasteboard types];
			return [types containsObject:NSPasteboardTypeString];
		}
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
		
		Image premultiplied{ image };
		premultiplied.premultiplyAlpha();
		
		@autoreleasepool
		{
			const int32 w = premultiplied.width();
			const int32 h = premultiplied.height();
			
			void* pSrc = premultiplied.data();
			const size_t bytesPerRow = premultiplied.bytesPerRow();
			const CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			const CGBitmapInfo bitmapInfo = (kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast);
			const CGContextRef context = CGBitmapContextCreate(pSrc, w, h, 8, bytesPerRow, colorSpace, bitmapInfo);
			CGColorSpaceRelease(colorSpace);
			
			if (not context)
			{
				return;
			}
			
			CGImageRef cg = CGBitmapContextCreateImage(context);
			CGContextRelease(context);
			
			if (not cg)
			{
				return;
			}
			
			NSImage* nsImage = [[NSImage alloc] initWithCGImage:cg size:NSMakeSize(w, h)];
			CGImageRelease(cg);
			
			if (not nsImage)
			{
				return;
			}
			
			[m_pImpl->pasteboard clearContents];
			
			// TIFF 形式でペーストボードに画像をセット
			NSData* tiff = [nsImage TIFFRepresentation];
			if (tiff)
			{
				[m_pImpl->pasteboard setData:tiff forType:NSPasteboardTypeTIFF];
				
				m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImage
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getImage(Image& image, const PremultiplyAlpha premultiplyAlpha)
	{
		image.clear();
		
		@autoreleasepool
		{
			NSData* data = [m_pImpl->pasteboard dataForType:NSPasteboardTypeTIFF];
			if (not data)
			{
				data = [m_pImpl->pasteboard dataForType:NSPasteboardTypePNG];
				
				if (not data)
				{
					return false;
				}
			}
			
			NSImage* nsImage = [[NSImage alloc] initWithData:data];
			if (not nsImage)
			{
				return false;
			}
			
			CGImageRef cg = [nsImage CGImageForProposedRect:nil context:nil hints:nil];
			if (not cg)
			{
				return false;
			}
			
			const size_t w = CGImageGetWidth(cg);
			const size_t h = CGImageGetHeight(cg);
			if ((w == 0) || (h == 0))
			{
				return false;
			}
			
			Image dst{ Size{ w, h } };
			const size_t bytesPerRow = dst.bytesPerRow();
			void* pDst = dst.data();
			const CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			const CGBitmapInfo bitmapInfo = (kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast);
			const CGContextRef context = CGBitmapContextCreate(pDst, w, h, 8, bytesPerRow, colorSpace, bitmapInfo);
			CGColorSpaceRelease(colorSpace);
			
			if (not context)
			{
				return false;
			}

			CGContextDrawImage(context, CGRectMake(0, 0, (CGFloat)w, (CGFloat)h), cg);
			CGContextRelease(context);
			
			if (premultiplyAlpha == PremultiplyAlpha::No)
			{
				dst.unpremultiplyAlpha();
			}
			
			image = std::move(dst);
			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasImage
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasImage()
	{
		@autoreleasepool
		{
			NSArray<NSPasteboardType>* types = [m_pImpl->pasteboard types];
			return ([types containsObject:NSPasteboardTypeTIFF] || [types containsObject:NSPasteboardTypePNG]);
		}
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

		@autoreleasepool
		{
			NSMutableArray<NSURL*>* urls = [NSMutableArray array];
			
			for (const auto& path : paths)
			{
				if (path.isEmpty())
				{
					continue;
				}
				
				const std::string utf8Path = FileSystem::NativePath(path);
				if (utf8Path.empty())
				{
					continue;
				}
				
				NSString* nsPath = [[NSString alloc] initWithBytes:utf8Path.data() length:utf8Path.size() encoding:NSUTF8StringEncoding];
				if (not nsPath)
				{
					continue;
				}
				
				if (NSURL* url = [NSURL fileURLWithPath:nsPath])
				{
					[urls addObject:url];
				}
			}
			
			if ([urls count] == 0)
			{
				return;
			}
			
			[m_pImpl->pasteboard clearContents];
			
			[m_pImpl->pasteboard writeObjects:urls];
			
			m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::getFilePaths(Array<FilePath>& paths)
	{
		paths.clear();
		
		@autoreleasepool
		{
			NSDictionary* options = @{ NSPasteboardURLReadingFileURLsOnlyKey: @YES };
			
			NSArray* objects = [m_pImpl->pasteboard readObjectsForClasses:@[[NSURL class]] options:options];
			if (objects && (0 < [objects count]))
			{
				for (id obj in objects)
				{
					if (not [obj isKindOfClass:[NSURL class]])
					{
						continue;
					}
					
					NSURL* url = (NSURL*)obj;
					if (not [url isFileURL])
					{
						continue;
					}
					
					NSString* nsPath = [url path];
					if (not nsPath)
					{
						continue;
					}
					
					const char* utf8Path = [nsPath UTF8String];
					if (not utf8Path)
					{
						continue;
					}
					
					if (FilePath path = FileSystem::FullPath(Unicode::FromUTF8(utf8Path)))
					{
						paths.push_back(std::move(path));
					}
				}
				
				return (not paths.empty());
			}
		}

		return false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasFilePaths()
	{
		@autoreleasepool
		{
			NSDictionary* options = @{ NSPasteboardURLReadingFileURLsOnlyKey: @YES };
			return [m_pImpl->pasteboard canReadObjectForClasses:@[[NSURL class]] options:options];
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setRichText
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setRichText(const StringView richText, const StringView plainTextFallback)
	{
		@autoreleasepool
		{
			[m_pImpl->pasteboard clearContents];

			// 1) RTF
			if (richText && LooksLikeRTF(richText))
			{
				const std::wstring w = richText.toWstr();
				std::string rtfBytes = RTF_EscapeUnicodeToU(w);

				// \ucN が無い場合は \uc1 を足す（Windows 版と同じ安全策）
				if (rtfBytes.find("\\uc") == std::string::npos)
				{
					if (const auto pos = rtfBytes.find("\\rtf1"); pos != std::string::npos)
					{
						rtfBytes.insert(pos + 5, "\\uc1");
					}
				}

				NSData* rtfData = [NSData dataWithBytes:rtfBytes.data() length:rtfBytes.size()];
				if (rtfData)
				{
					[m_pImpl->pasteboard setData:rtfData forType:NSPasteboardTypeRTF];
				}
			}

			// 2) plain text fallback
			{
				StringView fallback = plainTextFallback ? plainTextFallback : richText;
				if (fallback)
				{
					const std::string utf8 = Unicode::ToUTF8(fallback);
					NSString* ns = [[NSString alloc] initWithBytes:utf8.data()
														   length:utf8.size()
														 encoding:NSUTF8StringEncoding];
					if (ns)
					{
						[m_pImpl->pasteboard setString:ns forType:NSPasteboardTypeString];
					}
				}
			}

			m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setHTML
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setHTML(const StringView html, const StringView plainTextFallback)
	{
		@autoreleasepool
		{
			[m_pImpl->pasteboard clearContents];

			// 1) HTML
			if (html)
			{
				const std::string fragmentUtf8 = Unicode::ToUTF8(html);
				const std::string docUtf8 = WrapHTMLDocumentUTF8(fragmentUtf8);

				NSData* htmlData = [NSData dataWithBytes:docUtf8.data() length:docUtf8.size()];
				if (htmlData)
				{
					[m_pImpl->pasteboard setData:htmlData forType:NSPasteboardTypeHTML];
				}
			}

			// 2) plain text fallback
			{
				StringView fallback = plainTextFallback ? plainTextFallback : html;
				if (fallback)
				{
					const std::string utf8 = Unicode::ToUTF8(fallback);
					NSString* ns = [[NSString alloc] initWithBytes:utf8.data()
														   length:utf8.size()
														 encoding:NSUTF8StringEncoding];
					if (ns)
					{
						[m_pImpl->pasteboard setString:ns forType:NSPasteboardTypeString];
					}
				}
			}

			m_pImpl->sequenceNumber = [m_pImpl->pasteboard changeCount];
		}
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
		return{};
	}
}
