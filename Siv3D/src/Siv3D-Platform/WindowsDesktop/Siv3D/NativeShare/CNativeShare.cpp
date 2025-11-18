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
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/EngineLog.hpp>

# include <winrt/Windows.Storage.h>
# include <winrt/Windows.Foundation.Collections.h>

using namespace Windows::Foundation;
using namespace Windows::Storage;

namespace s3d
{
	CNativeShare::~CNativeShare()
	{
		LOG_SCOPED_DEBUG("CNativeShare::~CNativeShare()");

		// イベントハンドラの登録解除
		if (m_dataTransferManager)
		{
			if (m_dataRequestedToken)
			{
				m_dataTransferManager.DataRequested(m_dataRequestedToken);
			}
		}

		FileSystem::Remove(m_imageCachePath);
	}

	void CNativeShare::init()
	{
		LOG_SCOPED_DEBUG("CNativeShare::init()");
		
		m_hwnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
		
		m_interop = winrt::get_activation_factory<DataTransferManager, IDataTransferManagerInterop>();

		const winrt::guid dtmIid = winrt::guid_of<DataTransferManager>();
		m_interop->GetForWindow(m_hwnd, dtmIid, winrt::put_abi(m_dataTransferManager));

		m_dataRequestedToken = m_dataTransferManager.DataRequested(
			{ this, &CNativeShare::onDataRequested }
		);

		// キャッシュ画像ファイルのパスを設定
		m_imageCachePath = (CacheDirectory::Temporary() + U"NativeShare/image.png");
	}

	bool CNativeShare::show(const Image& image)
	{
		FileSystem::Remove(m_imageCachePath);

		if (not image.save(m_imageCachePath))
		{
			return false;
		}

		if (FAILED(m_interop->ShowShareUIForWindow(m_hwnd)))
		{
			return false;
		}
		
		return true;
	}

	void CNativeShare::onDataRequested(const DataTransferManager&, const DataRequestedEventArgs& args)
	{
		auto request = args.Request();
		auto deferral = request.GetDeferral();
		auto fileOp = StorageFile::GetFileFromPathAsync(FileSystem::NativePath(m_imageCachePath));
		fileOp.Completed([request, deferral](auto asyncInfo, auto asyncStatus)
			{
				if (asyncStatus == AsyncStatus::Completed)
				{
					auto file = asyncInfo.GetResults();
					auto data = request.Data();
					data.Properties().Title(L"Image");
					data.Properties().Description(L"Shared from Siv3D");
					data.Properties().FileTypes().Append(L".png");
					data.SetStorageItems(winrt::single_threaded_vector<IStorageItem>({ file }));
				}
				deferral.Complete();
			});
	}
}
