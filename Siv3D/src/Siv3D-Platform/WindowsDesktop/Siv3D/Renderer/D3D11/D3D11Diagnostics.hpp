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

# pragma once
# include "D3D11.hpp"
# include <string>

namespace s3d::D3D11Diagnostics
{
	// Includes the 32-bit code even when no symbolic name is known.
	[[nodiscard]]
	std::string FormatHRESULT(HRESULT hr);

	// Called only after Present fails. Queries the device without retrying Present,
	// and preserves the original error even if the additional query succeeds or fails.
	[[nodiscard]]
	std::string GetPresentFailureMessage(IDXGISwapChain* swapChain, HRESULT presentResult,
		uint32 syncInterval, uint32 flags);
}
