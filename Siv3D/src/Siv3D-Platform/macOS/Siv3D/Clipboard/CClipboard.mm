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
# include <Siv3D/EngineLog.hpp>
# include <AppKit/AppKit.h>
# include <CoreGraphics/CoreGraphics.h>

namespace s3d
{
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
	//	hasFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CClipboard::hasFilePaths()
	{
		return(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setRichText
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setRichText(const StringView richText, const StringView plainTextFallback)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	setHTML
	//
	////////////////////////////////////////////////////////////////

	void CClipboard::setHTML(const StringView html, const StringView plainTextFallback)
	{

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
