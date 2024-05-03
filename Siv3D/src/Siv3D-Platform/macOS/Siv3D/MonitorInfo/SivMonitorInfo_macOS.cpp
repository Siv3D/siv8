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
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

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
										 &wx, &wy, &ww, &wh);
				
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
						xPos = static_cast<int32>(xPos * scale);
						yPos = static_cast<int32>(yPos * scale);
						width = static_cast<int32>(width * scale);
						height = static_cast<int32>(height * scale);
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
				
				Size fullscreenResolution{ 0, 0 };
				{
					int32 numModes = 0;
					
					if (const GLFWvidmode* vidModes = ::glfwGetVideoModes(monitor, &numModes))
					{
						if (numModes)
						{
							fullscreenResolution.set(vidModes[numModes - 1].width, vidModes[numModes - 1].height);
						}
					}
				}

				const MonitorInfo info =
				{
					.name					= Unicode::FromUTF8(::glfwGetMonitorName(monitor)),
					.id						= Format(displayID),
					.displayDeviceName		= Format(unitNumber),
					.displayRect			= Rect{ xPos, yPos, width, height },
					.workArea				= Rect{ wx, wy, ww, wh },
					.fullscreenResolution	= fullscreenResolution,
					.isPrimary				= (i == 0),
					.sizeMillimeter			= sizeMillimeter,
					.scaling				= scaling,
					.refreshRate			= refreshRate,
				};
				
				results.push_back(info);
			}
			
			return results;
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
