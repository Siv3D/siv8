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
# include <Siv3D/Clipboard/IClipboard.hpp>

namespace s3d
{
	class CClipboard final : public ISiv3DClipboard
	{
	public:

		CClipboard() = default;

		~CClipboard() override;

		void init() override;

	private:

	};
}
