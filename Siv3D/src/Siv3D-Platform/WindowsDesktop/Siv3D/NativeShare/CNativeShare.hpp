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
# include <Siv3D/String.hpp>
# include <Siv3D/NativeShare/INativeShare.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <shobjidl_core.h>
# include <winrt/Windows.Foundation.h>
# include <winrt/Windows.ApplicationModel.DataTransfer.h>

using namespace winrt;
using namespace Windows::ApplicationModel::DataTransfer;

namespace s3d
{
	class CNativeShare final : public ISiv3DNativeShare
	{
	public:

		CNativeShare() = default;

		~CNativeShare() override;

		void init() override;

		bool show(const Image& image) override;

	private:

		HWND m_hwnd = nullptr;

		winrt::com_ptr<IDataTransferManagerInterop> m_interop;

		DataTransferManager m_dataTransferManager{ nullptr };

		winrt::event_token m_dataRequestedToken;

		FilePath m_imagePath;

		void onDataRequested(const DataTransferManager&, const DataRequestedEventArgs& args);
	};
}
