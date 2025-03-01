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

# include <Siv3D/Time.hpp>
# include "HighTemporalResolutionCursor.hpp"

namespace s3d
{
	void HighTemporalResolutionCursor::add(const Point rawClientPos)
	{
		const int64 time = Time::GetMicrosec();

		std::lock_guard lock{ m_mutex };
		m_buffer.emplace_back(time, rawClientPos);
	}

	void HighTemporalResolutionCursor::update(const Duration& deltaTime)
	{
		const int64 currentTime = Time::GetMicrosec();
		const int64 deltaTimeMicrosec = static_cast<int64>(deltaTime.count() * 1'000'000);
		const int64 timePoint = (currentTime - deltaTimeMicrosec);

		std::lock_guard lock{ m_mutex };
		m_buffer.remove_if([timePoint](const auto& data) { return (data.first < timePoint); });
	}

	Array<std::pair<int64, Point>> HighTemporalResolutionCursor::get() const
	{
		std::lock_guard lock{ m_mutex };
		return m_buffer;
	}
}
