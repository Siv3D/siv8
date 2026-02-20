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

# import <Cocoa/Cocoa.h>
# import <dispatch/dispatch.h>
# include <Siv3D/Utility.hpp>
# include "DockProgressBar.hpp"

namespace s3d
{
	class DockProgressBar::Impl
	{
	public:
		
		void setProgress(double progress0_1)
		{
			progress0_1 = Clamp(progress0_1, 0.0, 1.0);

			if (progress0_1 >= 1.0)
			{
				clear();
				return;
			}

			runOnMainThread([this, progress0_1]
			{
				if (!NSApp)
				{
					return;
				}

				NSDockTile* dockTile = NSApp.dockTile;
				if (!dockTile)
				{
					return;
				}

				ensureView(dockTile);

				if (_indicator)
				{
					_indicator.doubleValue = progress0_1;
				}

				[dockTile display];
			});
		}

		void clear()
		{
			runOnMainThread([this]
			{
				if (!NSApp)
				{
					return;
				}

				NSDockTile* dockTile = NSApp.dockTile;
				if (!dockTile)
				{
					return;
				}

				dockTile.contentView = nil;
				[dockTile display];

				_rootView = nil;
				_iconView = nil;
				_indicator = nil;
			});
		}

	private:
		// ARC 前提。非 ARC なら retain/release 対応が必要
		NSView* _rootView = nil;
		NSImageView* _iconView = nil;
		NSProgressIndicator* _indicator = nil;

		template <class F>
		static void runOnMainThread(F&& f)
		{
			if ([NSThread isMainThread])
			{
				f();
			}
			else
			{
				dispatch_async(dispatch_get_main_queue(), ^{
					f();
				});
			}
		}

		void ensureView(NSDockTile* dockTile)
		{
			if (_rootView && (dockTile.contentView == _rootView))
			{
				layoutForDockTile(dockTile);
				return;
			}

			const NSSize tileSize = dockTile.size;

			_rootView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, tileSize.width, tileSize.height)];
			_rootView.wantsLayer = YES;

			_iconView = [[NSImageView alloc] initWithFrame:_rootView.bounds];
			_iconView.imageScaling = NSImageScaleAxesIndependently;
			_iconView.image = NSApp.applicationIconImage;
			[_rootView addSubview:_iconView];

			_indicator = [[NSProgressIndicator alloc] initWithFrame:NSZeroRect];
			_indicator.indeterminate = NO;
			_indicator.minValue = 0.0;
			_indicator.maxValue = 1.0;
			_indicator.doubleValue = 0.0;
			_indicator.controlSize = NSControlSizeSmall;
			_indicator.style = NSProgressIndicatorStyleBar;

			[_rootView addSubview:_indicator];

			dockTile.contentView = _rootView;

			layoutForDockTile(dockTile);
		}

		void layoutForDockTile(NSDockTile* dockTile)
		{
			if (!_rootView || !_iconView || !_indicator)
			{
				return;
			}

			const NSSize tileSize = dockTile.size;

			_rootView.frame = NSMakeRect(0, 0, tileSize.width, tileSize.height);
			_iconView.frame = _rootView.bounds;

			const CGFloat w = tileSize.width * 0.78;
			const CGFloat h = Max<CGFloat>(8.0, tileSize.height * 0.10);
			const CGFloat x = (tileSize.width - w) * 0.5;
			const CGFloat y = tileSize.height * 0.12;

			_indicator.frame = NSMakeRect(x, y, w, h);
		}
	};

	DockProgressBar::DockProgressBar()
		: m_pImpl{ std::make_unique<Impl>() } {}

	DockProgressBar::~DockProgressBar()
	{
		if (m_pImpl)
		{
			m_pImpl->clear();
		}
	}

	void DockProgressBar::setProgress(double progress0_1)
	{
		if (m_pImpl)
		{
			m_pImpl->setProgress(progress0_1);
		}
	}

	void DockProgressBar::clear()
	{
		if (m_pImpl)
		{
			m_pImpl->clear();
		}
	}
}
