//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "ExtensionFilter.hpp"

namespace s3d
{
	void ExtensionFilter::set(const Array<String>& extensions)
	{
		m_sortedExtensions = extensions.sorted_and_uniqued();
	}

	bool ExtensionFilter::isEmpty() const
	{
		return m_sortedExtensions.isEmpty();
	}

	ExtensionFilter::operator bool() const
	{
		return (not m_sortedExtensions.isEmpty());
	}

	bool ExtensionFilter::includes(const String& extension) const
	{
		return std::binary_search(m_sortedExtensions.begin(), m_sortedExtensions.end(), extension);
	}

	const Array<String>& ExtensionFilter::getSortedExtensions() const
	{
		return m_sortedExtensions;
	}
}
