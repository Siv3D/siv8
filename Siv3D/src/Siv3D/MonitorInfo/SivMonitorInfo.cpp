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

# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error.hpp>
# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/MonitorInfo/WindowsMonitor.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# endif

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String MonitorInfo::format() const
	{
		String output;
		output += U"name: {}\n"_fmt(name);
		output += U"id: {}\n"_fmt(id);
		output += U"displayDeviceName: {}\n"_fmt(displayDeviceName);
		output += U"displayRect: {}\n"_fmt(displayRect);
		output += U"workArea: {}\n"_fmt(workArea);
		output += U"fullscreenResolution: {}\n"_fmt(fullscreenResolution);

		if (sizeMillimeter)
		{
			output += U"sizeMillimeter: {} mm x {} mm\n"_fmt(sizeMillimeter->x, sizeMillimeter->y);
		}
		else
		{
			output += U"sizeMillimeter: N/A\n";
		}

		if (scaling)
		{
			output += U"scaling: {}%\n"_fmt(*scaling * 100);
		}
		else
		{
			output += U"scaling: N/A\n";
		}

		if (refreshRate)
		{
			output += U"refreshRate: {} Hz\n"_fmt(*refreshRate);
		}
		else
		{
			output += U"refreshRate: N/A\n";
		}

		output += U"isPrimary: {}"_fmt(isPrimary);

		return output;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const MonitorInfo& value)
	{
		formatData.string.append(value.format());
	}

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetCurrentMonitor
		//
		////////////////////////////////////////////////////////////////

		MonitorInfo GetCurrentMonitor()
		{
# if SIV3D_PLATFORM(WINDOWS)
			return WindowsMonitor::GetCurrentMonitor(static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle()));
# else
			const Array<MonitorInfo> monitors = EnumerateMonitors();

			if (not monitors)
			{
				throw Error{ "System::GetCurrentMonitor(): No monitor found" };
			}

			const size_t monitorIndex = GetCurrentMonitorIndex();

			if (monitors.size() <= monitorIndex)
			{
				throw Error{ "System::GetCurrentMonitor(): Monitor index out of range" };
			}

			return monitors[monitorIndex];
# endif
		}
	}
}
