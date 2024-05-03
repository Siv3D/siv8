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

# pragma once
# include <mutex>
# include <Siv3D/Array.hpp>
# include <Siv3D/HashTable.hpp>
# include <Siv3D/UniqueResource.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	class CCursorStyle final : public ISiv3DCursorStyle
	{
	public:

		CCursorStyle() = default;

		~CCursorStyle() override;

		void init() override;

		bool update() override;

		void setDefaultStyle(CursorStyle style) override;

		bool registerCustomStyle(StringView name, const Image& image, Point hotSpot, AlphaPremultiplied alphaPremultiplied) override;

		void unregisterCustomStyle(StringView name) override;

		void clearCustomStyles() override;

		void requestStyle(CursorStyle style) override;

		void requestStyle(StringView name) override;

		void onSetCursor() override;

	private:
		
		std::array<HICON, 11> m_systemCursors;
		
		HICON m_currentCursor = nullptr;
		
		HICON m_defaultCursor = nullptr;
		
		HICON m_requestedCursor = nullptr;

		static void CursorDeleter(HICON h);

		HashTable<String, UniqueResource<HICON, decltype(&CursorDeleter)>> m_customCursors;

		//////////////////////////////////////////////////
		//
		std::mutex m_mutex;

		Array<UniqueResource<HICON, decltype(&CursorDeleter)>> m_garbage;
		//
		//////////////////////////////////////////////////
	};
}
