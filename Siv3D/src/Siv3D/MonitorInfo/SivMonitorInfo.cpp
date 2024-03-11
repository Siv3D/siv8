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

# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/FmtExtension.hpp>
# include <Siv3D/Error.hpp>

namespace s3d
{
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
			output += U"scaling: {} %\n"_fmt(scaling.value() * 100);
		}
		else
		{
			output += U"scaling: N/A\n";
		}

		if (refreshRate)
		{
			output += U"refreshRate: {} Hz\n"_fmt(refreshRate.value());
		}
		else
		{
			output += U"refreshRate: N/A\n";
		}

		output += U"isPrimary: {}"_fmt(isPrimary);

		return output;
	}

	void Formatter(FormatData& formatData, const MonitorInfo& value)
	{
		formatData.string.append(value.format());
	}

	namespace System
	{
		MonitorInfo GetCurrentMonitor()
		{
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
		}
	}
}
