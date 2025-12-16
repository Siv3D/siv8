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

# include "CNativeShare.hpp"
# include <Siv3D/GLFW/GLFW.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CNativeShare::~CNativeShare()
	{
		LOG_SCOPED_DEBUG("CNativeShare::~CNativeShare()");

		FileSystem::Remove(m_imageCachePath);
	}

	void CNativeShare::init()
	{
		LOG_SCOPED_DEBUG("CNativeShare::init()");
		
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		m_window = ::glfwGetCocoaWindow(glfwWindow);

		m_imageCachePath = (CacheDirectory::Temporary() + U"NativeShare/image.png");
	}

	bool CNativeShare::show(const Image& image)
	{
		FileSystem::Remove(m_imageCachePath);

		if (not image.save(m_imageCachePath))
		{
			return false;
		}
		
		@autoreleasepool
		{
			// ファイルパスを NSURL に変換
			NSString* nsPath = [NSString stringWithUTF8String:m_imageCachePath.toUTF8().c_str()];
			NSURL* fileURL = [NSURL fileURLWithPath:nsPath];
			
			// ファイルの存在確認
			if (not [[NSFileManager defaultManager] fileExistsAtPath:[fileURL path]])
			{
				return false;
			}
			
			// 共有アイテムの配列を作成
			NSArray* itemsToShare = @[fileURL];
			
			// 共有ピッカーを作成
			NSSharingServicePicker* picker = [[NSSharingServicePicker alloc] initWithItems:itemsToShare];

			// ウィンドウのコンテンツビューを取得
			NSView* contentView = [m_window contentView];
			NSRect bounds = [contentView bounds];
			
			// 共有ピッカーを表示
			[picker showRelativeToRect:NSMakeRect(NSMidX(bounds), NSMidY(bounds), 0, 0)
								ofView:contentView
							preferredEdge:NSRectEdgeMinY];
			
			return true;
		}

		return false;
	}
}
