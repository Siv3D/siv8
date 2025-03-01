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
# include <Siv3D/Array.hpp>

namespace s3d
{
	class ExtensionFilter
	{
	public:

		ExtensionFilter() = default;

		void set(const Array<String>& extensions);

		bool isEmpty() const;

		explicit operator bool() const;

		bool includes(const String& extension) const;

		const Array<String>& getSortedExtensions() const;

	private:

		Array<String> m_sortedExtensions;
	};
}
