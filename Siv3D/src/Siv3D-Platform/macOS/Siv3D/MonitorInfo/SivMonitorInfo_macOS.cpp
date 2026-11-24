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
# include <Siv3D/Error.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/GLFW/GLFW.hpp>
# include "../Window/WindowMisc.hpp"

extern"C"
{
	void glfwGetMonitorInfo_Siv3D(GLFWmonitor* handle, uint32_t* displayID, uint32_t* unitNumber,
										  int* xpos, int* ypos, int* w, int* h,
										  int* wx, int* wy, int* ww, int* wh);

	int glfwIsCurrentMonitor_Siv3D(GLFWwindow* window, GLFWmonitor* handle);
}

namespace s3d
{
	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	EnumerateMonitors
		//
		////////////////////////////////////////////////////////////////

		Array<MonitorInfo> EnumerateMonitors()
		{
			Array<MonitorInfo> results;
			
			int32 numMonitors;
			GLFWmonitor** monitors = ::glfwGetMonitors(&numMonitors);
			
			for (int32 i = 0; i < numMonitors; ++i)
			{
				GLFWmonitor* monitor = monitors[i];

				uint32 displayID, unitNumber;
				int32 xPos, yPos, width, height;
				int32 wx, wy, ww, wh;
				::glfwGetMonitorInfo_Siv3D(monitor, &displayID, &unitNumber,
										 &xPos, &yPos, &width, &height,
										 nullptr, nullptr, nullptr, nullptr);
				// The standard GLFW work-area query uses desktop points.
				::glfwGetMonitorWorkarea(monitor, &wx, &wy, &ww, &wh);
				
				Optional<Size> sizeMillimeter;
				{
					int32 pw = 0, ph = 0;
					::glfwGetMonitorPhysicalSize(monitor, &pw, &ph);
					if (pw > 0 && ph > 0)
					{
						sizeMillimeter = Size{ pw, ph };
					}
				}

				Optional<double> scaling;
				{
					float xscale = 1.0f, yscale = 1.0f;
					::glfwGetMonitorContentScale(monitor, &xscale, &yscale);
					
					if (const double scale = Max(xscale, yscale); 0.0 < scale)
					{
						scaling = scale;
					}
				}
				
				Optional<double> refreshRate;
				{
					if (const GLFWvidmode* vidMode = ::glfwGetVideoMode(monitor))
					{
						if (vidMode->refreshRate > 0)
						{
							refreshRate = vidMode->refreshRate;
						}
					}
				}
				
				const Size fullscreenResolution = WindowMisc::GetFullscreenResolution(monitor);

				const MonitorInfo info =
				{
					.name					= Unicode::FromUTF8(::glfwGetMonitorName(monitor)),
					.id						= Format(displayID),
					.displayDeviceName		= Format(unitNumber),
					.displayRect			= Rect{ xPos, yPos, width, height },
					.workArea				= Rect{ wx, wy, ww, wh },
					.fullscreenResolution	= fullscreenResolution,
					.sizeMillimeter			= sizeMillimeter,
					.isPrimary				= (i == 0),
					.scaling				= scaling,
					.refreshRate			= refreshRate,
				};
				
				results.push_back(info);
			}
			
			return results;
		}
	
		////////////////////////////////////////////////////////////////
		//
		//	GetCurrentMonitor
		//
		////////////////////////////////////////////////////////////////

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

		size_t GetCurrentMonitorIndex()
		{
			GLFWwindow* window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
			
			int32 numMonitors;
			GLFWmonitor** monitors = ::glfwGetMonitors(&numMonitors);
			
			for (int32 i = 0; i < numMonitors; ++i)
			{
				if (::glfwIsCurrentMonitor_Siv3D(window, monitors[i]))
				{
					return i;
				}
			}
			
			return 0;
		}
	}
}
