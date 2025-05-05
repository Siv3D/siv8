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

# include "FontData.hpp"

namespace s3d
{
	FontData::FontData(Null)
	{
		m_initialized = true;
	}

	bool FontData::isInitialized() const noexcept
	{
		return m_initialized;
	}
}
