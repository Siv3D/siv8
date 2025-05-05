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
# include "IFont.hpp"

namespace s3d
{
	class FontData
	{
	public:

		struct Null {};

		FontData() = default;

		explicit FontData(Null);

		[[nodiscard]]
		bool isInitialized() const noexcept;

	private:

		bool m_initialized = false;
	};
}
