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

# include "CDragDrop.hpp"
# include <ThirdParty/GLFW/GLFW_Siv3D_DragDropBridge.h>
# include <unordered_map>
# include <mutex>
# include <AppKit/AppKit.h>
# include <Siv3D/Time.hpp>
# include <Siv3D/Unicode.hpp>

namespace
{
	using namespace s3d;

	std::mutex g_dragDropMapMutex;
	std::unordered_map<void*, CDragDrop*> g_dragDrops;

	[[nodiscard]]
	static CDragDrop* LookupDragDrop(void* glfwWindow)
	{
		std::lock_guard lock{ g_dragDropMapMutex };

		const auto it = g_dragDrops.find(glfwWindow);

		if (it == g_dragDrops.end())
		{
			return nullptr;
		}

		return it->second;
	}

	[[nodiscard]]
	static NSImage* CreateDraggingImage()
	{
		const NSSize size = NSMakeSize(24.0, 24.0);
		NSImage* image = [[[NSImage alloc] initWithSize:size] autorelease];

		[image lockFocus];
		[[NSColor colorWithCalibratedWhite:0.15 alpha:0.75] setFill];
		[[NSBezierPath bezierPathWithRoundedRect:NSMakeRect(0.0, 0.0, size.width, size.height)
									   xRadius:6.0
									   yRadius:6.0] fill];
		[image unlockFocus];

		return image;
	}

	[[nodiscard]]
	static NSDraggingItem* CreateDraggingItem(id<NSPasteboardWriting> writer, const NSPoint localPoint, const NSInteger index)
	{
		NSDraggingItem* item = [[[NSDraggingItem alloc] initWithPasteboardWriter:writer] autorelease];

		const CGFloat offset = static_cast<CGFloat>(index) * 4.0;
		const NSRect frame = NSMakeRect((localPoint.x - 12.0 + offset),
										(localPoint.y - 12.0 - offset),
										24.0,
										24.0);

		[item setDraggingFrame:frame contents:CreateDraggingImage()];
		return item;
	}
}

extern "C"
{
	void GLFW_Siv3D_DragDropRegister(void* glfwWindow, void* dragDrop)
	{
		if ((glfwWindow == nullptr) || (dragDrop == nullptr))
		{
			return;
		}

		std::lock_guard lock{ g_dragDropMapMutex };
		g_dragDrops[glfwWindow] = static_cast<CDragDrop*>(dragDrop);
	}

	void GLFW_Siv3D_DragDropUnregister(void* glfwWindow)
	{
		if (glfwWindow == nullptr)
		{
			return;
		}

		std::lock_guard lock{ g_dragDropMapMutex };
		g_dragDrops.erase(glfwWindow);
	}

	int GLFW_Siv3D_DragDropAcceptsFilePaths(void* glfwWindow)
	{
		if (auto* p = LookupDragDrop(glfwWindow))
		{
			return p->acceptsFilePathsFromNative() ? 1 : 0;
		}

		return 0;
	}

	int GLFW_Siv3D_DragDropAcceptsText(void* glfwWindow)
	{
		if (auto* p = LookupDragDrop(glfwWindow))
		{
			return p->acceptsTextFromNative() ? 1 : 0;
		}

		return 0;
	}

	void GLFW_Siv3D_DragDropSetDragOver(void* glfwWindow, const int x, const int y, const int itemType)
	{
		auto* p = LookupDragDrop(glfwWindow);

		if (p == nullptr)
		{
			return;
		}

		DragItemType type;

		if (itemType == GLFW_SIV3D_DRAG_ITEM_FILE_PATHS)
		{
			type = DragItemType::FilePaths;
		}
		else if (itemType == GLFW_SIV3D_DRAG_ITEM_TEXT)
		{
			type = DragItemType::Text;
		}
		else
		{
			return;
		}

		p->setDragOverFromNative(s3d::Point{ x, y }, type);
	}

	void GLFW_Siv3D_DragDropClearDragOver(void* glfwWindow)
	{
		if (auto* p = LookupDragDrop(glfwWindow))
		{
			p->clearDragOverFromNative();
		}
	}

	void GLFW_Siv3D_DragDropAppendFilePaths(void* glfwWindow, const int x, const int y, const char* const* paths, const size_t count)
	{
		auto* p = LookupDragDrop(glfwWindow);

		if ((p == nullptr) || (paths == nullptr) || (count == 0))
		{
			return;
		}

		Array<FilePath> filePaths;
		filePaths.reserve(count);

		for (size_t i = 0; i < count; ++i)
		{
			if (paths[i] == nullptr)
			{
				continue;
			}

			filePaths.push_back(Unicode::FromUTF8(paths[i]));
		}

		p->appendDroppedFilePathsFromNative(filePaths, s3d::Point{ x, y }, Time::GetMillisec());
	}

	void GLFW_Siv3D_DragDropAppendText(void* glfwWindow, const int x, const int y, const char* text)
	{
		auto* p = LookupDragDrop(glfwWindow);

		if ((p == nullptr) || (text == nullptr) || (text[0] == '\0'))
		{
			return;
		}

		p->appendDroppedTextFromNative(Unicode::FromUTF8(text), s3d::Point{ x, y }, Time::GetMillisec());
	}

	int GLFW_Siv3D_DragDropProcessMouseDragged(void* glfwWindow, void* nsView, void* nsEvent)
	{
		auto* p = LookupDragDrop(glfwWindow);

		if (p == nullptr)
		{
			return 0;
		}

		CDragDrop::PendingDragRequest request;

		if (not p->tryPopPendingDragRequest(request))
		{
			return 0;
		}

		@autoreleasepool
		{
			NSView* view = static_cast<NSView*>(nsView);
			NSEvent* event = static_cast<NSEvent*>(nsEvent);

			if ((view == nil) || (event == nil))
			{
				p->sourceEndedFromNative();
				return 0;
			}

			const NSPoint localPoint = [view convertPoint:[event locationInWindow] fromView:nil];
			NSMutableArray* items = [NSMutableArray array];

			if (request.type == CDragDrop::DragRequestType::FilePaths)
			{
				for (size_t i = 0; i < request.filePaths.size(); ++i)
				{
					const std::string utf8 = Unicode::ToUTF8(request.filePaths[i]);
					NSString* nsPath = [NSString stringWithUTF8String:utf8.c_str()];

					if (nsPath == nil)
					{
						continue;
					}

					NSURL* url = [NSURL fileURLWithPath:nsPath];

					if (url == nil)
					{
						continue;
					}

					[items addObject:CreateDraggingItem(url, localPoint, static_cast<NSInteger>(i))];
				}
			}
			else
			{
				const std::string utf8 = Unicode::ToUTF8(request.text);
				NSString* nsText = [NSString stringWithUTF8String:utf8.c_str()];

				if (nsText != nil)
				{
					[items addObject:CreateDraggingItem(nsText, localPoint, 0)];
				}
			}

			if ([items count] == 0)
			{
				p->sourceEndedFromNative();
				return 0;
			}

			NSDraggingSession* session = [view beginDraggingSessionWithItems:items
																	 event:event
																	source:(id<NSDraggingSource>)view];

			if (session == nil)
			{
				p->sourceEndedFromNative();
				return 0;
			}

			[session setDraggingFormation:NSDraggingFormationPile];
			return 1;
		}
	}

	void GLFW_Siv3D_DragDropSourceEnded(void* glfwWindow)
	{
		if (auto* p = LookupDragDrop(glfwWindow))
		{
			p->sourceEndedFromNative();
		}
	}
}
